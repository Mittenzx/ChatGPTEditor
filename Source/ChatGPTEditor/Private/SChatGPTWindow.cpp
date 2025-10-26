// Copyright Epic Games, Inc. All Rights Reserved.

#include "SChatGPTWindow.h"
#include "TestAutomationHelper.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SWindow.h"
#include "Styling/AppStyle.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/MessageDialog.h"
#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "SChatGPTWindow"

void SChatGPTWindow::Construct(const FArguments& InArgs)
{
	ConversationHistory = TEXT("");
	
	// Initialize audit logging
	FTestAutomationHelper::InitializeAuditLog();
	
	// Initialize test types
	TestTypes.Add(MakeShareable(new FString(TEXT("Unit Test"))));
	TestTypes.Add(MakeShareable(new FString(TEXT("Integration Test"))));
	TestTypes.Add(MakeShareable(new FString(TEXT("Functional Test"))));
	CurrentTestType = TestTypes[0];
	
	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Header section with title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ChatGPTTitle", "ChatGPT - OpenAI Integration"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]
		
		// Security Permissions Section
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SVerticalBox)
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 5.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SecurityPermissions", "Security Permissions (Default: OFF)"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]
			
			// Asset Write Permission
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 2.0f)
			[
				SNew(SCheckBox)
				.IsChecked(this, &SChatGPTWindow::GetAssetWritePermission)
				.OnCheckStateChanged(this, &SChatGPTWindow::OnAssetWritePermissionChanged)
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AssetWritePermission", "Allow Asset Write Operations (DANGEROUS)"))
				]
			]
			
			// Console Command Permission
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 2.0f)
			[
				SNew(SCheckBox)
				.IsChecked(this, &SChatGPTWindow::GetConsoleCommandPermission)
				.OnCheckStateChanged(this, &SChatGPTWindow::OnConsoleCommandPermissionChanged)
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ConsoleCommandPermission", "Allow Console Commands (DANGEROUS)"))
				]
			]
			
			// File I/O Permission
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 2.0f)
			[
				SNew(SCheckBox)
				.IsChecked(this, &SChatGPTWindow::GetFileIOPermission)
				.OnCheckStateChanged(this, &SChatGPTWindow::OnFileIOPermissionChanged)
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("FileIOPermission", "Allow File I/O Operations (DANGEROUS)"))
				]
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]
		
		// Test Automation Section
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SVerticalBox)
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 5.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("TestAutomation", "Test Automation & QA"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]
			
			// Test generation input row
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				SNew(SHorizontalBox)
				
				// Test type selector
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 5.0f, 0.0f)
				[
					SNew(SBox)
					.WidthOverride(150.0f)
					[
						SAssignNew(TestTypeComboBox, SComboBox<TSharedPtr<FString>>)
						.OptionsSource(&TestTypes)
						.OnGenerateWidget(this, &SChatGPTWindow::MakeTestTypeComboWidget)
						.OnSelectionChanged(this, &SChatGPTWindow::OnTestTypeSelectionChanged)
						.Content()
						[
							SNew(STextBlock)
							.Text(this, &SChatGPTWindow::GetCurrentTestTypeText)
						]
					]
				]
				
				// Test prompt input
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(0.0f, 0.0f, 5.0f, 0.0f)
				[
					SAssignNew(TestPromptInputBox, SEditableTextBox)
					.HintText(LOCTEXT("TestPromptHint", "e.g., 'Write tests for Asset Automation'"))
				]
				
				// Generate test button
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 5.0f, 0.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("GenerateTestButton", "Generate Test"))
					.OnClicked(this, &SChatGPTWindow::OnGenerateTestClicked)
					.ToolTipText(LOCTEXT("GenerateTestTooltip", "Generate test code based on your prompt"))
				]
				
				// Run test button
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("RunTestButton", "Run Test"))
					.OnClicked(this, &SChatGPTWindow::OnRunTestClicked)
					.ToolTipText(LOCTEXT("RunTestTooltip", "Run the currently selected test"))
				]
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]
		
		// Conversation history area
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(10.0f)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(5.0f)
			[
				SAssignNew(ConversationScrollBox, SScrollBox)
				
				+ SScrollBox::Slot()
				[
					SAssignNew(ConversationHistoryBox, SMultiLineEditableTextBox)
					.IsReadOnly(true)
					.Text(FText::FromString(ConversationHistory))
					.AutoWrapText(true)
				]
			]
		]
		
		// Input section
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SHorizontalBox)
			
			// Message input box
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SAssignNew(MessageInputBox, SEditableTextBox)
				.HintText(LOCTEXT("MessageInputHint", "Type your message here..."))
			]
			
			// Send button
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("SendButton", "Send"))
				.OnClicked(this, &SChatGPTWindow::OnSendMessageClicked)
			]
			
			// Clear history button
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("ClearButton", "Clear"))
				.OnClicked(this, &SChatGPTWindow::OnClearHistoryClicked)
			]
		]
		
		// Info text at bottom
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 5.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("APIKeyInfo", "Set OPENAI_API_KEY environment variable with your API key"))
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)))
		]
	];
}

FReply SChatGPTWindow::OnSendMessageClicked()
{
	FString UserMessage = MessageInputBox->GetText().ToString();
	
	if (UserMessage.IsEmpty())
	{
		return FReply::Handled();
	}
	
	if (!IsAPIKeyValid())
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			LOCTEXT("NoAPIKey", "Please set the OPENAI_API_KEY environment variable with your OpenAI API key."));
		return FReply::Handled();
	}
	
	// Append user message to conversation
	AppendMessage(TEXT("User"), UserMessage);
	
	// Clear input box
	MessageInputBox->SetText(FText::GetEmpty());
	
	// Send request to OpenAI
	SendRequestToOpenAI(UserMessage);
	
	return FReply::Handled();
}

FReply SChatGPTWindow::OnClearHistoryClicked()
{
	ConversationHistory = TEXT("");
	Messages.Empty();
	ConversationHistoryBox->SetText(FText::FromString(ConversationHistory));
	
	return FReply::Handled();
}

void SChatGPTWindow::SendRequestToOpenAI(const FString& UserMessage)
{
	// Create message object
	TSharedPtr<FJsonObject> MessageObject = MakeShareable(new FJsonObject);
	MessageObject->SetStringField(TEXT("role"), TEXT("user"));
	MessageObject->SetStringField(TEXT("content"), UserMessage);
	Messages.Add(MessageObject);
	
	// Create request body
	TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject);
	RequestBody->SetStringField(TEXT("model"), TEXT("gpt-3.5-turbo"));
	
	// Add messages array
	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	for (const auto& Msg : Messages)
	{
		MessagesArray.Add(MakeShareable(new FJsonValueObject(Msg)));
	}
	RequestBody->SetArrayField(TEXT("messages"), MessagesArray);
	RequestBody->SetNumberField(TEXT("max_tokens"), 1000);
	RequestBody->SetNumberField(TEXT("temperature"), 0.7);
	
	// Serialize to JSON string
	FString RequestBodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBodyString);
	FJsonSerializer::Serialize(RequestBody.ToSharedRef(), Writer);
	
	// Create HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(TEXT("https://api.openai.com/v1/chat/completions"));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *GetAPIKey()));
	HttpRequest->SetContentAsString(RequestBodyString);
	
	// Bind response callback - Use BindSP for safer widget lifetime management
	HttpRequest->OnProcessRequestComplete().BindSP(this, &SChatGPTWindow::OnResponseReceived);
	
	// Send request
	HttpRequest->ProcessRequest();
	
	AppendMessage(TEXT("System"), TEXT("Sending request to OpenAI..."));
}

void SChatGPTWindow::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		AppendMessage(TEXT("Error"), TEXT("Failed to connect to OpenAI API. Check your internet connection."));
		return;
	}
	
	int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		FString ErrorMessage = FString::Printf(TEXT("API Error (HTTP %d): %s"), 
			ResponseCode, *Response->GetContentAsString());
		AppendMessage(TEXT("Error"), ErrorMessage);
		return;
	}
	
	// Parse response
	FString ResponseString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> ResponseObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	
	if (!FJsonSerializer::Deserialize(Reader, ResponseObject) || !ResponseObject.IsValid())
	{
		AppendMessage(TEXT("Error"), TEXT("Failed to parse API response."));
		return;
	}
	
	// Extract assistant message
	const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
	if (ResponseObject->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray->Num() > 0)
	{
		TSharedPtr<FJsonObject> Choice = (*ChoicesArray)[0]->AsObject();
		if (!Choice.IsValid())
		{
			AppendMessage(TEXT("Error"), TEXT("Unexpected API response format: 'choices[0]' is not an object."));
			return;
		}
		
		TSharedPtr<FJsonObject> Message = Choice->GetObjectField(TEXT("message"));
		if (!Message.IsValid())
		{
			AppendMessage(TEXT("Error"), TEXT("Unexpected API response format: missing or invalid 'message' field."));
			return;
		}
		
		FString AssistantMessage = Message->GetStringField(TEXT("content"));
		
		// Add assistant message to conversation
		TSharedPtr<FJsonObject> AssistantMessageObject = MakeShareable(new FJsonObject);
		AssistantMessageObject->SetStringField(TEXT("role"), TEXT("assistant"));
		AssistantMessageObject->SetStringField(TEXT("content"), AssistantMessage);
		Messages.Add(AssistantMessageObject);
		
		AppendMessage(TEXT("Assistant"), AssistantMessage);
	}
	else
	{
		AppendMessage(TEXT("Error"), TEXT("Unexpected API response format."));
	}
}

void SChatGPTWindow::AppendMessage(const FString& Role, const FString& Message)
{
	FString FormattedMessage = FString::Printf(TEXT("[%s]: %s\n\n"), *Role, *Message);
	ConversationHistory += FormattedMessage;
	ConversationHistoryBox->SetText(FText::FromString(ConversationHistory));
	
	// Scroll to bottom
	ConversationScrollBox->ScrollToEnd();
}

FString SChatGPTWindow::GetAPIKey() const
{
	return FPlatformMisc::GetEnvironmentVariable(TEXT("OPENAI_API_KEY"));
}

bool SChatGPTWindow::IsAPIKeyValid() const
{
	FString APIKey = GetAPIKey();
	return !APIKey.IsEmpty();
}

void SChatGPTWindow::HandlePermissionChange(bool& bPermissionFlag, ECheckBoxState NewState, const FText& WarningText)
{
	bool bNewValue = (NewState == ECheckBoxState::Checked);
	
	if (bNewValue && !bPermissionFlag)
	{
		EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, WarningText);
		
		if (Result == EAppReturnType::Yes)
		{
			bPermissionFlag = true;
		}
	}
	else
	{
		bPermissionFlag = bNewValue;
	}
}

void SChatGPTWindow::OnAssetWritePermissionChanged(ECheckBoxState NewState)
{
	HandlePermissionChange(
		bAllowAssetWrite,
		NewState,
		LOCTEXT("AssetWriteWarning", 
			"WARNING: Enabling Asset Write operations allows ChatGPT to modify your project assets.\n\n"
			"This can lead to:\n"
			"- Data loss\n"
			"- Project corruption\n"
			"- Unintended changes\n\n"
			"Only enable this if you understand the risks and have backups.\n\n"
			"Do you want to continue?")
	);
}

void SChatGPTWindow::OnConsoleCommandPermissionChanged(ECheckBoxState NewState)
{
	HandlePermissionChange(
		bAllowConsoleCommands,
		NewState,
		LOCTEXT("ConsoleCommandWarning", 
			"WARNING: Enabling Console Commands allows ChatGPT to execute arbitrary commands in your editor.\n\n"
			"This can lead to:\n"
			"- System changes\n"
			"- Security vulnerabilities\n"
			"- Unexpected behavior\n\n"
			"Only enable this if you understand the risks.\n\n"
			"Do you want to continue?")
	);
}

void SChatGPTWindow::OnFileIOPermissionChanged(ECheckBoxState NewState)
{
	HandlePermissionChange(
		bAllowFileIO,
		NewState,
		LOCTEXT("FileIOWarning", 
			"WARNING: Enabling File I/O operations allows ChatGPT to read and write files on your system.\n\n"
			"This can lead to:\n"
			"- Data loss\n"
			"- File corruption\n"
			"- Security vulnerabilities\n\n"
			"Only enable this if you understand the risks and have backups.\n\n"
			"Do you want to continue?")
	);
}

ECheckBoxState SChatGPTWindow::GetAssetWritePermission() const
{
	return bAllowAssetWrite ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState SChatGPTWindow::GetConsoleCommandPermission() const
{
	return bAllowConsoleCommands ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState SChatGPTWindow::GetFileIOPermission() const
{
	return bAllowFileIO ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

// Test automation UI handlers

TSharedRef<SWidget> SChatGPTWindow::MakeTestTypeComboWidget(TSharedPtr<FString> InItem)
{
	return SNew(STextBlock)
		.Text(FText::FromString(*InItem));
}

void SChatGPTWindow::OnTestTypeSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	CurrentTestType = NewSelection;
}

FText SChatGPTWindow::GetCurrentTestTypeText() const
{
	return CurrentTestType.IsValid() ? FText::FromString(*CurrentTestType) : FText::FromString(TEXT("Unit Test"));
}

FReply SChatGPTWindow::OnGenerateTestClicked()
{
	FString TestPrompt = TestPromptInputBox->GetText().ToString();
	
	if (TestPrompt.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			LOCTEXT("NoTestPrompt", "Please enter a test description or prompt."));
		return FReply::Handled();
	}
	
	if (!IsAPIKeyValid())
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			LOCTEXT("NoAPIKey", "Please set the OPENAI_API_KEY environment variable with your OpenAI API key."));
		return FReply::Handled();
	}
	
	if (bWaitingForTestGeneration)
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			LOCTEXT("TestGenerationInProgress", "Test generation is already in progress. Please wait."));
		return FReply::Handled();
	}
	
	FString TestType = CurrentTestType.IsValid() ? *CurrentTestType : TEXT("Unit Test");
	
	// Log the test generation request
	FTestAutomationHelper::LogTestGenerationRequest(TestPrompt, TestType);
	
	// Send request to OpenAI
	SendTestGenerationRequest(TestPrompt, TestType);
	
	// Clear input
	TestPromptInputBox->SetText(FText::GetEmpty());
	
	return FReply::Handled();
}

FReply SChatGPTWindow::OnRunTestClicked()
{
	// For now, show a message that this requires an actual test to be selected
	// In a full implementation, this would integrate with Unreal's automation framework
	AppendMessage(TEXT("Test Runner"), 
		TEXT("Test execution requires integration with Unreal's Automation Testing Framework.\n"
		     "Tests generated from this tool should be:\n"
		     "1. Reviewed and saved to your project's Tests folder\n"
		     "2. Compiled with your project\n"
		     "3. Run via Window → Test Automation or Session Frontend\n\n"
		     "See the README for detailed instructions."));
	
	FTestAutomationHelper::LogAuditMessage(TEXT("TEST_RUNNER"), 
		TEXT("User requested test execution - directed to manual process"));
	
	return FReply::Handled();
}

FReply SChatGPTWindow::OnConfirmTestCodeClicked()
{
	if (!PendingTestCode.IsEmpty())
	{
		// Log the confirmation
		FTestAutomationHelper::LogAuditMessage(TEXT("TEST_CODE_CONFIRMED"), 
			FString::Printf(TEXT("User confirmed test code: %s"), *PendingTestName));
		
		// Add test code to conversation history for reference
		AppendMessage(TEXT("Test Code Confirmed"), 
			FString::Printf(TEXT("Test '%s' code has been confirmed.\n\nNext steps:\n"
			                     "1. Copy the code from above\n"
			                     "2. Create a new .cpp file in your Tests folder\n"
			                     "3. Compile your project\n"
			                     "4. Run the test via Window → Test Automation\n\n"
			                     "The test code has been logged to: %s"),
			                *PendingTestName,
			                *FTestAutomationHelper::GetAuditLogPath()));
		
		// Clear pending state
		PendingTestCode.Empty();
		PendingTestName.Empty();
	}
	
	// Close preview window if it exists
	if (TestPreviewWindow.IsValid())
	{
		TestPreviewWindow->RequestDestroyWindow();
		TestPreviewWindow.Reset();
	}
	
	return FReply::Handled();
}

FReply SChatGPTWindow::OnCancelTestCodeClicked()
{
	// Log the cancellation
	FTestAutomationHelper::LogAuditMessage(TEXT("TEST_CODE_REJECTED"), 
		FString::Printf(TEXT("User rejected test code: %s"), *PendingTestName));
	
	AppendMessage(TEXT("System"), TEXT("Test code generation cancelled."));
	
	// Clear pending state
	PendingTestCode.Empty();
	PendingTestName.Empty();
	
	// Close preview window if it exists
	if (TestPreviewWindow.IsValid())
	{
		TestPreviewWindow->RequestDestroyWindow();
		TestPreviewWindow.Reset();
	}
	
	return FReply::Handled();
}

// Test automation implementation

void SChatGPTWindow::SendTestGenerationRequest(const FString& TestPrompt, const FString& TestType)
{
	bWaitingForTestGeneration = true;
	
	// Create a specialized prompt for test generation
	FString SystemPrompt = FString::Printf(
		TEXT("You are a test automation expert for Unreal Engine 5.5. "
		     "Generate a %s for Unreal Engine based on the following description. "
		     "The test should use Unreal's Automation Framework macros (IMPLEMENT_SIMPLE_AUTOMATION_TEST or similar). "
		     "Include proper #include directives, follow Unreal coding standards, and add comments explaining the test. "
		     "Format the code in a ```cpp code block. "
		     "Test description: %s"),
		*TestType,
		*TestPrompt
	);
	
	// Create message objects for test generation
	TArray<TSharedPtr<FJsonObject>> TestMessages;
	
	// System message
	TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject);
	SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
	SystemMessage->SetStringField(TEXT("content"), TEXT("You are a test automation expert for Unreal Engine 5.5."));
	TestMessages.Add(SystemMessage);
	
	// User message
	TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject);
	UserMessage->SetStringField(TEXT("role"), TEXT("user"));
	UserMessage->SetStringField(TEXT("content"), SystemPrompt);
	TestMessages.Add(UserMessage);
	
	// Create request body
	TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject);
	RequestBody->SetStringField(TEXT("model"), TEXT("gpt-3.5-turbo"));
	
	// Add messages array
	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	for (const auto& Msg : TestMessages)
	{
		MessagesArray.Add(MakeShareable(new FJsonValueObject(Msg)));
	}
	RequestBody->SetArrayField(TEXT("messages"), MessagesArray);
	RequestBody->SetNumberField(TEXT("max_tokens"), 2000); // More tokens for code generation
	RequestBody->SetNumberField(TEXT("temperature"), 0.3); // Lower temperature for more focused code generation
	
	// Serialize to JSON string
	FString RequestBodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBodyString);
	FJsonSerializer::Serialize(RequestBody.ToSharedRef(), Writer);
	
	// Create HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(TEXT("https://api.openai.com/v1/chat/completions"));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *GetAPIKey()));
	HttpRequest->SetContentAsString(RequestBodyString);
	
	// Bind response callback
	HttpRequest->OnProcessRequestComplete().BindSP(this, &SChatGPTWindow::OnTestGenerationResponseReceived);
	
	// Send request
	HttpRequest->ProcessRequest();
	
	AppendMessage(TEXT("System"), FString::Printf(TEXT("Generating %s for: %s..."), *TestType, *TestPrompt));
}

void SChatGPTWindow::OnTestGenerationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bWaitingForTestGeneration = false;
	
	if (!bWasSuccessful || !Response.IsValid())
	{
		AppendMessage(TEXT("Error"), TEXT("Failed to connect to OpenAI API for test generation."));
		FTestAutomationHelper::LogAuditMessage(TEXT("ERROR"), TEXT("Test generation API request failed"));
		return;
	}
	
	int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		FString ErrorMessage = FString::Printf(TEXT("API Error (HTTP %d): %s"), 
			ResponseCode, *Response->GetContentAsString());
		AppendMessage(TEXT("Error"), ErrorMessage);
		FTestAutomationHelper::LogAuditMessage(TEXT("ERROR"), ErrorMessage);
		return;
	}
	
	// Parse response
	FString ResponseString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> ResponseObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	
	if (!FJsonSerializer::Deserialize(Reader, ResponseObject) || !ResponseObject.IsValid())
	{
		AppendMessage(TEXT("Error"), TEXT("Failed to parse API response."));
		return;
	}
	
	// Extract assistant message
	const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
	if (ResponseObject->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray->Num() > 0)
	{
		TSharedPtr<FJsonObject> Choice = (*ChoicesArray)[0]->AsObject();
		if (!Choice.IsValid())
		{
			AppendMessage(TEXT("Error"), TEXT("Unexpected API response format."));
			return;
		}
		
		TSharedPtr<FJsonObject> Message = Choice->GetObjectField(TEXT("message"));
		if (!Message.IsValid())
		{
			AppendMessage(TEXT("Error"), TEXT("Unexpected API response format."));
			return;
		}
		
		FString AssistantResponse = Message->GetStringField(TEXT("content"));
		
		// Parse test code from response
		FString TestCode;
		FString TestName;
		if (FTestAutomationHelper::ParseTestCodeFromResponse(AssistantResponse, TestCode, TestName))
		{
			// Validate test code
			TArray<FString> Warnings;
			bool bIsValid = FTestAutomationHelper::ValidateTestCode(TestCode, Warnings);
			
			if (!bIsValid)
			{
				AppendMessage(TEXT("Security Warning"), 
					TEXT("Generated test code contains potentially dangerous operations and cannot be used.\n"
					     "Please review the security warnings and modify your request."));
				
				for (const FString& Warning : Warnings)
				{
					AppendMessage(TEXT("Warning"), Warning);
				}
				
				FTestAutomationHelper::LogAuditMessage(TEXT("SECURITY_BLOCK"), 
					TEXT("Test code rejected due to security concerns"));
				return;
			}
			
			// Show warnings if any (non-critical)
			if (Warnings.Num() > 0)
			{
				for (const FString& Warning : Warnings)
				{
					AppendMessage(TEXT("Warning"), Warning);
				}
			}
			
			// Store pending test code
			PendingTestCode = TestCode;
			PendingTestName = TestName.IsEmpty() ? TEXT("GeneratedTest") : TestName;
			
			// Show preview window
			ShowTestCodePreview(TestCode, PendingTestName);
			
			// Also show in conversation for reference
			AppendMessage(TEXT("Test Code Generated"), 
				FString::Printf(TEXT("Test Name: %s\n\nGenerated test code (review in preview window):\n\n%s"),
				                *PendingTestName, *TestCode));
			
			FTestAutomationHelper::LogAuditMessage(TEXT("TEST_CODE_GENERATED"), 
				FString::Printf(TEXT("Test: %s | Length: %d chars"), *PendingTestName, TestCode.Len()));
		}
		else
		{
			// No code block found, just show the response
			AppendMessage(TEXT("Assistant"), AssistantResponse);
			AppendMessage(TEXT("System"), 
				TEXT("No test code block found in response. Please try rephrasing your request."));
		}
	}
}

void SChatGPTWindow::ShowTestCodePreview(const FString& TestCode, const FString& TestName)
{
	// Create a modal window to preview and confirm test code
	TSharedRef<SWindow> PreviewWindow = SNew(SWindow)
		.Title(FText::FromString(FString::Printf(TEXT("Test Code Preview: %s"), *TestName)))
		.ClientSize(FVector2D(800.0f, 600.0f))
		.SupportsMaximize(true)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::UserSized);
	
	TSharedRef<SWidget> PreviewContent = 
		SNew(SVerticalBox)
		
		// Header
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(FString::Printf(TEXT("Review Test Code: %s"), *TestName)))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]
		
		// Warning message
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("TestCodeWarning", 
				"⚠️ IMPORTANT: Review this code carefully before using it.\n"
				"AI-generated code should always be reviewed for correctness and security.\n"
				"This code has passed basic security validation but may still need modifications."))
			.ColorAndOpacity(FLinearColor(1.0f, 0.8f, 0.0f))
			.AutoWrapText(true)
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]
		
		// Code preview
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(10.0f)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SAssignNew(TestCodePreviewBox, SMultiLineEditableTextBox)
				.Text(FText::FromString(TestCode))
				.IsReadOnly(true)
				.Font(FCoreStyle::GetDefaultFontStyle("Mono", 9))
			]
		]
		
		// Button row
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("TestCodeInstructions", 
					"Click 'Confirm' to accept this code, or 'Cancel' to reject it."))
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("ConfirmTestCode", "Confirm"))
				.OnClicked(this, &SChatGPTWindow::OnConfirmTestCodeClicked)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("CancelTestCode", "Cancel"))
				.OnClicked(this, &SChatGPTWindow::OnCancelTestCodeClicked)
			]
		];
	
	PreviewWindow->SetContent(PreviewContent);
	
	// Store window reference
	TestPreviewWindow = PreviewWindow;
	
	// Show the window
	FSlateApplication::Get().AddWindow(PreviewWindow);
}

void SChatGPTWindow::ExecuteTest(const FString& TestName)
{
	// This would integrate with Unreal's automation framework
	// For now, we log the request and provide guidance
	FTestAutomationHelper::LogTestExecutionRequest(TestName, TEXT("Manual"));
	
	AppendMessage(TEXT("Test Execution"), 
		FString::Printf(TEXT("To execute test '%s':\n"
		                     "1. Ensure the test code is saved in your project's Tests folder\n"
		                     "2. Compile your project\n"
		                     "3. Open Window → Test Automation\n"
		                     "4. Find and run your test\n"
		                     "5. View results in the Test Automation window"),
		                *TestName));
}

void SChatGPTWindow::DisplayTestResults(const FString& TestName, bool bSuccess, const FString& Results)
{
	FTestAutomationHelper::LogTestExecutionResult(TestName, bSuccess, Results);
	
	FString StatusEmoji = bSuccess ? TEXT("✅") : TEXT("❌");
	AppendMessage(TEXT("Test Results"), 
		FString::Printf(TEXT("%s Test: %s\n\n%s"), *StatusEmoji, *TestName, *Results));
}

#undef LOCTEXT_NAMESPACE
