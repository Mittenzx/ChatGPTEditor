// Copyright Epic Games, Inc. All Rights Reserved.

#include "SChatGPTWindow.h"
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
				.ToolTipText(LOCTEXT("AssetWriteTooltip", "Enable to allow ChatGPT to create, modify, or delete project assets. DANGEROUS - Use with caution!"))
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AssetWritePermission", "🔒 Allow Asset Write Operations (DANGEROUS)"))
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
				.ToolTipText(LOCTEXT("ConsoleCommandTooltip", "Enable to allow ChatGPT to execute editor console commands. DANGEROUS - Use with caution!"))
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ConsoleCommandPermission", "🔒 Allow Console Commands (DANGEROUS)"))
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
				.ToolTipText(LOCTEXT("FileIOTooltip", "Enable to allow ChatGPT to read and write files on your system. DANGEROUS - Use with caution!"))
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("FileIOPermission", "🔒 Allow File I/O Operations (DANGEROUS)"))
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
					.Font(TAttribute<FSlateFontInfo>::Create(TAttribute<FSlateFontInfo>::FGetter::CreateLambda([this]()
					{
						return FCoreStyle::GetDefaultFontStyle("Regular", FontSize);
					})))
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
				.HintText(LOCTEXT("MessageInputHint", "Type your message here... (Ctrl+Enter to send)"))
				.ToolTipText(LOCTEXT("MessageInputTooltip", "Enter your message to ChatGPT. Press Ctrl+Enter to send quickly."))
			]
			
			// Send button
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SAssignNew(SendButton, SButton)
				.Text(LOCTEXT("SendButton", "Send"))
				.ToolTipText(LOCTEXT("SendButtonTooltip", "Send message to ChatGPT (Ctrl+Enter)"))
				.OnClicked(this, &SChatGPTWindow::OnSendMessageClicked)
			]
			
			// Clear history button
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(ClearButton, SButton)
				.Text(LOCTEXT("ClearButton", "Clear"))
				.ToolTipText(LOCTEXT("ClearButtonTooltip", "Clear conversation history (Ctrl+L)"))
				.OnClicked(this, &SChatGPTWindow::OnClearHistoryClicked)
			]
		]
		
		// Accessibility controls section
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("AccessibilityLabel", "Accessibility:"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("DecreaseFontButton", "A-"))
				.ToolTipText(LOCTEXT("DecreaseFontTooltip", "Decrease font size (Ctrl+-)"))
				.OnClicked(this, &SChatGPTWindow::OnDecreaseFontSize)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(SButton)
				.Text(this, &SChatGPTWindow::GetFontSizeButtonText)
				.ToolTipText(LOCTEXT("ResetFontTooltip", "Reset font size to default (Ctrl+0)"))
				.OnClicked(this, &SChatGPTWindow::OnResetFontSize)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("IncreaseFontButton", "A+"))
				.ToolTipText(LOCTEXT("IncreaseFontTooltip", "Increase font size (Ctrl++)"))
				.OnClicked(this, &SChatGPTWindow::OnIncreaseFontSize)
			]
		]
		
		// Info text at bottom
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 5.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("APIKeyInfo", "💡 Tip: Set OPENAI_API_KEY environment variable with your API key"))
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)))
		]
		
		// Keyboard shortcuts info
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 2.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("KeyboardShortcuts", "⌨️ Shortcuts: Ctrl+Enter=Send | Ctrl+L=Clear | Ctrl+/-=Font Size"))
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f)))
		]
	];
}

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
	
	// Ctrl+Plus to increase font size
	if ((InKeyEvent.GetKey() == EKeys::Equals || InKeyEvent.GetKey() == EKeys::Add) && InKeyEvent.IsControlDown())
	{
		return OnIncreaseFontSize();
	}
	
	// Ctrl+Minus to decrease font size
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
	
	// Ctrl+Plus to increase font size
	if ((InKeyEvent.GetKey() == EKeys::Equals || InKeyEvent.GetKey() == EKeys::Add) && InKeyEvent.IsControlDown())
	{
		return OnIncreaseFontSize();
	}
	
	// Ctrl+Minus to decrease font size
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

void SChatGPTWindow::SendRequestToOpenAI(const FString& UserMessage)
{
	// Set request in progress
	bIsRequestInProgress = true;
	
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
	
	AppendMessage(TEXT("System"), TEXT("⏳ Sending request to OpenAI..."));
}

void SChatGPTWindow::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// Clear request in progress flag
	bIsRequestInProgress = false;
	
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

void SChatGPTWindow::ProcessAssetAutomation(const FString& Response)
{
	// Parse the response for asset operations
	TArray<FAssetOperation> Operations = FAssetAutomation::ParseResponse(Response);
	
	if (Operations.Num() == 0)
	{
		return;
	}
	
	// Notify user about detected operations
	AppendMessage(TEXT("System"), FString::Printf(TEXT("Detected %d asset operation(s) in response."), Operations.Num()));
	
	// Execute each operation
	for (const FAssetOperation& Op : Operations)
	{
		AppendMessage(TEXT("System"), FString::Printf(TEXT("Processing: %s - %s"), *Op.GetTypeAsString(), *Op.AssetName));
		
		bool bSuccess = FAssetAutomation::ExecuteOperation(Op, bAllowAssetWrite);
		
		if (bSuccess)
		{
			AppendMessage(TEXT("System"), FString::Printf(TEXT("✓ Successfully executed: %s"), *Op.GetTypeAsString()));
		}
		else
		{
			AppendMessage(TEXT("System"), FString::Printf(TEXT("✗ Failed to execute: %s"), *Op.GetTypeAsString()));
		}
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

void SChatGPTWindow::UpdateFontSize()
{
	// Font is updated automatically through the TAttribute lambda
	// Just invalidate the widget to force a refresh
	if (ConversationHistoryBox.IsValid())
	{
		ConversationHistoryBox->Invalidate(EInvalidateWidget::Layout);
	}
}

FText SChatGPTWindow::GetFontSizeButtonText() const
{
	return FText::Format(LOCTEXT("FontSizeText", "Font Size: {0}"), FontSize);
}

#undef LOCTEXT_NAMESPACE
