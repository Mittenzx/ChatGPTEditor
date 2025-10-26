// Copyright Epic Games, Inc. All Rights Reserved.

#include "SChatGPTWindow.h"
#include "SSceneEditPreviewDialog.h"
#include "SceneEditingManager.h"
#include "AuditLogger.h"
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
#include "Widgets/SWindow.h"
#include "Editor.h"

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
			
			// Scene Editing Permission
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 2.0f)
			[
				SNew(SCheckBox)
				.IsChecked(this, &SChatGPTWindow::GetSceneEditingPermission)
				.OnCheckStateChanged(this, &SChatGPTWindow::OnSceneEditingPermissionChanged)
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SceneEditingPermission", "Allow Scene Editing (DANGEROUS)"))
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
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("ClearButton", "Clear"))
				.OnClicked(this, &SChatGPTWindow::OnClearHistoryClicked)
			]
			
			// View Audit Log button
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("AuditLogButton", "View Audit Log"))
				.OnClicked(this, &SChatGPTWindow::OnViewAuditLogClicked)
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
	
	// Append user message to conversation
	AppendMessage(TEXT("User"), UserMessage);
	
	// Clear input box
	MessageInputBox->SetText(FText::GetEmpty());
	
	// Check if this is a scene editing command
	FString LowerMessage = UserMessage.ToLower();
	bool bIsSceneEditCommand = LowerMessage.Contains(TEXT("add ")) || 
	                           LowerMessage.Contains(TEXT("spawn ")) || 
	                           LowerMessage.Contains(TEXT("place ")) || 
	                           LowerMessage.Contains(TEXT("delete ")) || 
	                           LowerMessage.Contains(TEXT("remove ")) || 
	                           LowerMessage.Contains(TEXT("move ")) ||
	                           (LowerMessage.Contains(TEXT("change ")) && (LowerMessage.Contains(TEXT("light")) || LowerMessage.Contains(TEXT("actor")))) ||
	                           (LowerMessage.Contains(TEXT("set ")) && (LowerMessage.Contains(TEXT("light")) || LowerMessage.Contains(TEXT("actor"))));
	
	if (bIsSceneEditCommand)
	{
		ProcessSceneEditingCommand(UserMessage);
	}
	else
	{
		// Send regular request to OpenAI
		if (!IsAPIKeyValid())
		{
			FMessageDialog::Open(EAppMsgType::Ok, 
				LOCTEXT("NoAPIKey", "Please set the OPENAI_API_KEY environment variable with your OpenAI API key."));
			return FReply::Handled();
		}
		
		SendRequestToOpenAI(UserMessage);
	}
	
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

void SChatGPTWindow::OnSceneEditingPermissionChanged(ECheckBoxState NewState)
{
	HandlePermissionChange(
		bAllowSceneEditing,
		NewState,
		LOCTEXT("SceneEditingWarning", 
			"WARNING: Enabling Scene Editing allows ChatGPT to modify your level.\n\n"
			"This can lead to:\n"
			"- Unwanted changes to your level\n"
			"- Deletion of actors\n"
			"- Data loss\n\n"
			"All changes will require preview and confirmation.\n\n"
			"Only enable this if you understand the risks and have backups.\n\n"
			"Do you want to continue?")
	);
}

ECheckBoxState SChatGPTWindow::GetSceneEditingPermission() const
{
	return bAllowSceneEditing ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SChatGPTWindow::ProcessSceneEditingCommand(const FString& Command)
{
	if (!bAllowSceneEditing)
	{
		AppendMessage(TEXT("System"), TEXT("Scene editing is disabled. Enable 'Allow Scene Editing' permission to use this feature."));
		return;
	}

	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		AppendMessage(TEXT("Error"), TEXT("No active world found. Please open a level first."));
		return;
	}

	// Parse the command into actions
	TArray<FSceneEditAction> Actions = FSceneEditingManager::Get().ParseCommand(Command);

	if (Actions.Num() == 0)
	{
		AppendMessage(TEXT("System"), TEXT("Could not parse scene editing command. Try commands like:\n"
			"- 'Add 5 lights to this room'\n"
			"- 'Place a camera at PlayerStart'\n"
			"- 'Move all props up by 100 units'\n"
			"- 'Delete all trigger volumes'"));
		return;
	}

	// Show preview dialog
	TSharedRef<SSceneEditPreviewDialog> PreviewDialog = SNew(SSceneEditPreviewDialog)
		.Actions(Actions);

	TSharedRef<SWindow> PreviewWindow = SNew(SWindow)
		.Title(LOCTEXT("PreviewWindowTitle", "Scene Editing Preview"))
		.ClientSize(FVector2D(700, 500))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			PreviewDialog
		];

	FSlateApplication::Get().AddModalWindow(PreviewWindow, FSlateApplication::Get().GetActiveTopLevelWindow());

	// Check if user confirmed
	if (PreviewDialog->WasConfirmed())
	{
		// Execute the actions
		bool bSuccess = FSceneEditingManager::Get().ExecuteActions(Actions, World, false);
		
		if (bSuccess)
		{
			AppendMessage(TEXT("System"), TEXT("Scene editing actions applied successfully. Check the audit log for details."));
		}
		else
		{
			AppendMessage(TEXT("Error"), TEXT("Failed to apply some scene editing actions. Check the audit log for details."));
		}
	}
	else
	{
		AppendMessage(TEXT("System"), TEXT("Scene editing actions cancelled by user."));
	}
}

FReply SChatGPTWindow::OnViewAuditLogClicked()
{
	// Get the audit log
	FString LogText = FAuditLogger::Get().ExportLogToString();

	// Create a dialog to show the log
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(LogText));

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
