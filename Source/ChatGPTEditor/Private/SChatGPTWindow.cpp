// Copyright Epic Games, Inc. All Rights Reserved.

#include "SChatGPTWindow.h"
#include "SBlueprintAssistantPanel.h"
#include "BlueprintAuditLog.h"
#include "AssetAutomation.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Styling/AppStyle.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/MessageDialog.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/FileHelper.h"
#include "DesktopPlatformModule.h"
#include "Framework/Commands/InputChord.h"

#define LOCTEXT_NAMESPACE "SChatGPTWindow"

void SChatGPTWindow::Construct(const FArguments& InArgs)
{
	ConversationHistory = TEXT("");
	
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
		
		// Blueprint Scripting Assistant Section
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
				.Text(LOCTEXT("BlueprintAssistantTitle", "Blueprint Scripting Assistant"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]
			
			// Blueprint Generation
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.0f)
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(0.0f, 0.0f, 5.0f, 0.0f)
				[
					SAssignNew(BlueprintPromptBox, SEditableTextBox)
					.HintText(LOCTEXT("BlueprintPromptHint", "Describe the Blueprint logic to generate..."))
				]
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("GenerateBlueprintButton", "Generate Blueprint"))
					.OnClicked(this, &SChatGPTWindow::OnGenerateBlueprintClicked)
					.IsEnabled_Lambda([this]() { return bAllowAssetWrite; })
					.ToolTipText(LOCTEXT("GenerateBlueprintTooltip", "Enable 'Allow Asset Write Operations' to use this feature"))
				]
			]
			
			// Blueprint Explanation
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.0f)
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(0.0f, 0.0f, 5.0f, 0.0f)
				[
					SAssignNew(BlueprintNameBox, SEditableTextBox)
					.HintText(LOCTEXT("BlueprintNameHint", "Enter Blueprint name to explain..."))
				]
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("ExplainBlueprintButton", "Explain Blueprint"))
					.OnClicked(this, &SChatGPTWindow::OnExplainBlueprintClicked)
				]
			]
			
			// Audit Log Export
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("ExportAuditLogButton", "Export Audit Log"))
				.OnClicked(this, &SChatGPTWindow::OnExportAuditLogClicked)
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
		
		// Process asset automation if enabled
		ProcessAssetAutomation(AssistantMessage);
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

// Blueprint Scripting Assistant Implementation

FReply SChatGPTWindow::OnGenerateBlueprintClicked()
{
	FString UserPrompt = BlueprintPromptBox->GetText().ToString();
	
	if (UserPrompt.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			LOCTEXT("EmptyBlueprintPrompt", "Please enter a description of the Blueprint logic to generate."));
		return FReply::Handled();
	}
	
	if (!bAllowAssetWrite)
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			LOCTEXT("AssetWriteRequired", "Please enable 'Allow Asset Write Operations' to generate Blueprints."));
		return FReply::Handled();
	}
	
	if (!IsAPIKeyValid())
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			LOCTEXT("NoAPIKey", "Please set the OPENAI_API_KEY environment variable with your OpenAI API key."));
		return FReply::Handled();
	}
	
	// Log the generation request
	FBlueprintAuditLog::Get().LogGeneration(UserPrompt, TEXT("Request sent to AI"));
	
	// Store the prompt for later use
	PendingBlueprintPrompt = UserPrompt;
	
	// Create system message for Blueprint generation
	FString SystemPrompt = TEXT("You are a Blueprint scripting assistant for Unreal Engine. Generate Blueprint node descriptions in JSON format. "
		"Response must be valid JSON with this structure: "
		"{\"description\": \"Brief description\", \"nodes\": [\"Node1\", \"Node2\"], \"connections\": [\"Connection1\"]}");
	
	// Create request
	TArray<TSharedPtr<FJsonObject>> BlueprintMessages;
	
	TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject);
	SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
	SystemMessage->SetStringField(TEXT("content"), SystemPrompt);
	BlueprintMessages.Add(SystemMessage);
	
	TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject);
	UserMessage->SetStringField(TEXT("role"), TEXT("user"));
	UserMessage->SetStringField(TEXT("content"), UserPrompt);
	BlueprintMessages.Add(UserMessage);
	
	// Create request body
	TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject);
	RequestBody->SetStringField(TEXT("model"), TEXT("gpt-3.5-turbo"));
	
	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	for (const auto& Msg : BlueprintMessages)
	{
		MessagesArray.Add(MakeShareable(new FJsonValueObject(Msg)));
	}
	RequestBody->SetArrayField(TEXT("messages"), MessagesArray);
	RequestBody->SetNumberField(TEXT("max_tokens"), 1000);
	RequestBody->SetNumberField(TEXT("temperature"), 0.3);
	
	// Serialize to JSON
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
	
	// Bind callback with user prompt
	HttpRequest->OnProcessRequestComplete().BindSP(this, &SChatGPTWindow::OnBlueprintGenerationResponseReceived, UserPrompt);
	
	// Send request
	HttpRequest->ProcessRequest();
	
	AppendMessage(TEXT("Blueprint Assistant"), TEXT("Generating Blueprint preview..."));
	
	return FReply::Handled();
}

FReply SChatGPTWindow::OnExplainBlueprintClicked()
{
	FString BlueprintName = BlueprintNameBox->GetText().ToString();
	
	if (BlueprintName.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			LOCTEXT("EmptyBlueprintName", "Please enter a Blueprint name to explain."));
		return FReply::Handled();
	}
	
	if (!IsAPIKeyValid())
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			LOCTEXT("NoAPIKey", "Please set the OPENAI_API_KEY environment variable with your OpenAI API key."));
		return FReply::Handled();
	}
	
	// Create prompt for explanation
	FString ExplanationPrompt = FString::Printf(
		TEXT("Explain the Blueprint named '%s' in Unreal Engine. Provide: "
		"1) A brief summary of its purpose "
		"2) A step-by-step breakdown of its logic. "
		"Format as JSON: {\"summary\": \"...\", \"steps\": \"...\"}"),
		*BlueprintName);
	
	// Create request
	TArray<TSharedPtr<FJsonObject>> ExplanationMessages;
	
	TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject);
	UserMessage->SetStringField(TEXT("role"), TEXT("user"));
	UserMessage->SetStringField(TEXT("content"), ExplanationPrompt);
	ExplanationMessages.Add(UserMessage);
	
	// Create request body
	TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject);
	RequestBody->SetStringField(TEXT("model"), TEXT("gpt-3.5-turbo"));
	
	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	for (const auto& Msg : ExplanationMessages)
	{
		MessagesArray.Add(MakeShareable(new FJsonValueObject(Msg)));
	}
	RequestBody->SetArrayField(TEXT("messages"), MessagesArray);
	RequestBody->SetNumberField(TEXT("max_tokens"), 1000);
	RequestBody->SetNumberField(TEXT("temperature"), 0.5);
	
	// Serialize to JSON
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
	
	// Bind callback
	HttpRequest->OnProcessRequestComplete().BindSP(this, &SChatGPTWindow::OnBlueprintExplanationResponseReceived, BlueprintName);
	
	// Send request
	HttpRequest->ProcessRequest();
	
	AppendMessage(TEXT("Blueprint Assistant"), FString::Printf(TEXT("Generating explanation for '%s'..."), *BlueprintName));
	
	return FReply::Handled();
}

FReply SChatGPTWindow::OnExportAuditLogClicked()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			LOCTEXT("PlatformError", "Failed to access desktop platform."));
		return FReply::Handled();
	}
	
	TArray<FString> OutFiles;
	const FString DefaultPath = FPaths::ProjectSavedDir();
	const FString DefaultFile = FString::Printf(TEXT("BlueprintAuditLog_%s.txt"), *FDateTime::Now().ToString());
	
	if (DesktopPlatform->SaveFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(AsShared()),
		TEXT("Export Audit Log"),
		DefaultPath,
		DefaultFile,
		TEXT("Text Files (*.txt)|*.txt"),
		EFileDialogFlags::None,
		OutFiles))
	{
		if (OutFiles.Num() > 0)
		{
			if (FBlueprintAuditLog::Get().ExportToFile(OutFiles[0]))
			{
				FMessageDialog::Open(EAppMsgType::Ok, 
					FText::Format(LOCTEXT("ExportSuccess", "Audit log exported successfully to:\n{0}"), 
					FText::FromString(OutFiles[0])));
			}
			else
			{
				FMessageDialog::Open(EAppMsgType::Ok, 
					LOCTEXT("ExportFailed", "Failed to export audit log."));
			}
		}
	}
	
	return FReply::Handled();
}

void SChatGPTWindow::OnBlueprintGenerationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FString& UserPrompt)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		AppendMessage(TEXT("Error"), TEXT("Failed to connect to OpenAI API for Blueprint generation."));
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
	
	// Extract content
	const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
	if (ResponseObject->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray->Num() > 0)
	{
		TSharedPtr<FJsonObject> Choice = (*ChoicesArray)[0]->AsObject();
		if (!Choice.IsValid())
		{
			AppendMessage(TEXT("Error"), TEXT("Invalid response format."));
			return;
		}
		
		TSharedPtr<FJsonObject> Message = Choice->GetObjectField(TEXT("message"));
		if (!Message.IsValid())
		{
			AppendMessage(TEXT("Error"), TEXT("Missing message in response."));
			return;
		}
		
		FString Content = Message->GetStringField(TEXT("content"));
		
		// Parse Blueprint data
		FBlueprintPreviewData PreviewData = ParseBlueprintGenerationResponse(Content);
		PreviewData.UserPrompt = UserPrompt;
		
		if (PreviewData.bIsValid)
		{
			// Log preview shown
			FBlueprintAuditLog::Get().LogPreviewShown(Content);
			
			// Show preview dialog
			ShowBlueprintPreview(PreviewData, UserPrompt);
		}
		else
		{
			AppendMessage(TEXT("Error"), TEXT("Failed to parse Blueprint generation response. Please try a different prompt."));
		}
	}
}

void SChatGPTWindow::OnBlueprintExplanationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FString& BlueprintName)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		AppendMessage(TEXT("Error"), TEXT("Failed to connect to OpenAI API for Blueprint explanation."));
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
	
	// Extract content
	const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
	if (ResponseObject->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray->Num() > 0)
	{
		TSharedPtr<FJsonObject> Choice = (*ChoicesArray)[0]->AsObject();
		if (!Choice.IsValid())
		{
			AppendMessage(TEXT("Error"), TEXT("Invalid response format."));
			return;
		}
		
		TSharedPtr<FJsonObject> Message = Choice->GetObjectField(TEXT("message"));
		if (!Message.IsValid())
		{
			AppendMessage(TEXT("Error"), TEXT("Missing message in response."));
			return;
		}
		
		FString Content = Message->GetStringField(TEXT("content"));
		
		// Parse explanation
		FBlueprintExplanation Explanation = ParseBlueprintExplanationResponse(Content);
		Explanation.BlueprintName = BlueprintName;
		
		if (Explanation.bIsValid)
		{
			// Log explanation
			FBlueprintAuditLog::Get().LogExplanation(BlueprintName, Content);
			
			// Display explanation
			DisplayBlueprintExplanation(Explanation);
		}
		else
		{
			AppendMessage(TEXT("Blueprint Explanation"), Content);
		}
	}
}

void SChatGPTWindow::ShowBlueprintPreview(const FBlueprintPreviewData& PreviewData, const FString& UserPrompt)
{
	TSharedRef<SWindow> PreviewWindow = SNew(SWindow)
		.Title(LOCTEXT("PreviewWindowTitle", "Blueprint Generation Preview"))
		.ClientSize(FVector2D(800, 600))
		.SupportsMaximize(false)
		.SupportsMinimize(false);
	
	TSharedRef<SBlueprintPreviewDialog> PreviewDialog = SNew(SBlueprintPreviewDialog)
		.PreviewData(PreviewData);
	
	PreviewWindow->SetContent(PreviewDialog);
	
	FSlateApplication::Get().AddModalWindow(PreviewWindow, FSlateApplication::Get().GetActiveTopLevelWindow());
	
	// Check if user approved
	if (PreviewDialog->WasApproved())
	{
		ProcessBlueprintGeneration(UserPrompt, true);
		FBlueprintAuditLog::Get().LogUserApproval(TEXT("Generated Blueprint"));
		AppendMessage(TEXT("Blueprint Assistant"), TEXT("Blueprint approved! In a full implementation, the Blueprint would be created now."));
		AppendMessage(TEXT("Info"), TEXT("Note: Actual Blueprint creation requires integration with Blueprint editor APIs."));
	}
	else
	{
		FBlueprintAuditLog::Get().LogUserRejection(TEXT("User rejected preview"));
		AppendMessage(TEXT("Blueprint Assistant"), TEXT("Blueprint generation cancelled."));
	}
}

void SChatGPTWindow::ProcessBlueprintGeneration(const FString& UserPrompt, bool bApproved)
{
	// Note: This is a placeholder for actual Blueprint creation
	// In a full implementation, this would use Blueprint editor APIs to create nodes
	
	if (bApproved)
	{
		UE_LOG(LogTemp, Log, TEXT("Blueprint generation approved for prompt: %s"), *UserPrompt);
		// TODO: Implement actual Blueprint node creation using BlueprintGraph APIs
	}
}

void SChatGPTWindow::DisplayBlueprintExplanation(const FBlueprintExplanation& Explanation)
{
	FString FormattedExplanation = FString::Printf(
		TEXT("Blueprint: %s\n\nSummary:\n%s\n\nStep-by-Step:\n%s"),
		*Explanation.BlueprintName,
		*Explanation.Summary,
		*Explanation.StepByStep);
	
	AppendMessage(TEXT("Blueprint Explanation"), FormattedExplanation);
}

FBlueprintPreviewData SChatGPTWindow::ParseBlueprintGenerationResponse(const FString& ResponseContent)
{
	FBlueprintPreviewData PreviewData;
	
	// Try to parse as JSON
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
	
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		// Extract fields
		if (JsonObject->HasField(TEXT("description")))
		{
			PreviewData.GeneratedDescription = JsonObject->GetStringField(TEXT("description"));
		}
		
		if (JsonObject->HasField(TEXT("nodes")))
		{
			const TArray<TSharedPtr<FJsonValue>>* NodesArray;
			if (JsonObject->TryGetArrayField(TEXT("nodes"), NodesArray))
			{
				for (const auto& NodeValue : *NodesArray)
				{
					PreviewData.NodesList += TEXT("• ") + NodeValue->AsString() + TEXT("\n");
				}
			}
		}
		
		if (JsonObject->HasField(TEXT("connections")))
		{
			const TArray<TSharedPtr<FJsonValue>>* ConnectionsArray;
			if (JsonObject->TryGetArrayField(TEXT("connections"), ConnectionsArray))
			{
				for (const auto& ConnectionValue : *ConnectionsArray)
				{
					PreviewData.ConnectionsList += TEXT("• ") + ConnectionValue->AsString() + TEXT("\n");
				}
			}
		}
		
		PreviewData.bIsValid = !PreviewData.GeneratedDescription.IsEmpty();
	}
	else
	{
		// Fallback: treat entire response as description
		PreviewData.GeneratedDescription = ResponseContent;
		PreviewData.NodesList = TEXT("(Parse the response manually)");
		PreviewData.ConnectionsList = TEXT("(Parse the response manually)");
		PreviewData.bIsValid = true;
	}
	
	return PreviewData;
}

FBlueprintExplanation SChatGPTWindow::ParseBlueprintExplanationResponse(const FString& ResponseContent)
{
	FBlueprintExplanation Explanation;
	
	// Try to parse as JSON
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
	
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		if (JsonObject->HasField(TEXT("summary")))
		{
			Explanation.Summary = JsonObject->GetStringField(TEXT("summary"));
		}
		
		if (JsonObject->HasField(TEXT("steps")))
		{
			Explanation.StepByStep = JsonObject->GetStringField(TEXT("steps"));
		}
		
		Explanation.bIsValid = !Explanation.Summary.IsEmpty();
	}
	else
	{
		// Fallback: use raw response
		Explanation.Summary = ResponseContent;
		Explanation.StepByStep = TEXT("");
		Explanation.bIsValid = true;
	}
	
	return Explanation;
}

// Keyboard shortcut handling
FReply SChatGPTWindow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	// Ctrl+Enter to send message
	if (InKeyEvent.GetKey() == EKeys::Enter && InKeyEvent.IsControlDown())
	{
		return OnSendMessageClicked();
	}
	
	// Ctrl+L to clear history
	if (InKeyEvent.GetKey() == EKeys::L && InKeyEvent.IsControlDown())
	{
		return OnClearHistoryClicked();
	}
	
	// Ctrl++ to increase font size
	if ((InKeyEvent.GetKey() == EKeys::Equals || InKeyEvent.GetKey() == EKeys::Add) && InKeyEvent.IsControlDown())
	{
		return OnIncreaseFontSize();
	}
	
	// Ctrl+- to decrease font size
	if ((InKeyEvent.GetKey() == EKeys::Hyphen || InKeyEvent.GetKey() == EKeys::Subtract) && InKeyEvent.IsControlDown())
	{
		return OnDecreaseFontSize();
	}
	
	// Ctrl+0 to reset font size
	if (InKeyEvent.GetKey() == EKeys::Zero && InKeyEvent.IsControlDown())
	{
		return OnResetFontSize();
	}
	
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

// Font size management
FReply SChatGPTWindow::OnIncreaseFontSize()
{
	if (FontSize < MaxFontSize)
	{
		FontSize++;
		UpdateFontSize();
	}
	return FReply::Handled();
}

FReply SChatGPTWindow::OnDecreaseFontSize()
{
	if (FontSize > MinFontSize)
	{
		FontSize--;
		UpdateFontSize();
	}
	return FReply::Handled();
}

FReply SChatGPTWindow::OnResetFontSize()
{
	FontSize = DefaultFontSize;
	UpdateFontSize();
	return FReply::Handled();
}

void SChatGPTWindow::UpdateFontSize()
{
	// The font size is updated through TAttribute binding in the UI
	// This just triggers a re-evaluation
	if (ConversationHistoryBox.IsValid())
	{
		ConversationHistoryBox->Invalidate(EInvalidateWidget::Layout);
	}
}

FText SChatGPTWindow::GetFontSizeButtonText() const
{
	return FText::Format(LOCTEXT("FontSizeDisplay", "{0}pt"), FontSize);
}

// Asset automation processing
void SChatGPTWindow::ProcessAssetAutomation(const FString& Response)
{
	if (!bAllowAssetWrite)
	{
		// Asset automation is disabled
		return;
	}
	
	// Parse and execute asset commands
	FAssetAutomation Automation;
	TArray<FAssetOperation> Operations = Automation.ParseResponse(Response);
	
	if (Operations.Num() > 0)
	{
		AppendMessage(TEXT("System"), FString::Printf(TEXT("Detected %d asset operation(s) in response."), Operations.Num()));
		Automation.ExecuteCommands(Operations, bAllowAssetWrite);
	}
}

#undef LOCTEXT_NAMESPACE
