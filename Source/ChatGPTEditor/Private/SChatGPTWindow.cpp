// Copyright Epic Games, Inc. All Rights Reserved.

#include "SChatGPTWindow.h"
#include "TestAutomationHelper.h"
#include "DocumentationHandler.h"
#include "AuditLog.h"
#include "AuditLogger.h"
#include "ProjectFileManager.h"
#include "ChatGPTConsoleHandler.h"
#include "ChatGPTPythonHandler.h"
#include "SSceneEditPreviewDialog.h"
#include "SceneEditingManager.h"
#include "AuditLogger.h"
#include "SBlueprintAssistantPanel.h"
#include "BlueprintAuditLog.h"
#include "AssetAutomation.h"
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
#include "Widgets/SWindow.h"
#include "Editor.h"
#include "Misc/FileHelper.h"
#include "DesktopPlatformModule.h"
#include "Framework/Commands/InputChord.h"

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
	// Initialize handlers
	ConsoleHandler = MakeShared<FChatGPTConsoleHandler>();
	PythonHandler = MakeShared<FChatGPTPythonHandler>();
	
	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Header section with title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ChatGPTTitle", "ChatGPT - OpenAI Integration"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
			]
			
			// Audit Log button
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("ViewAuditLog", "View Audit Log"))
				.OnClicked(this, &SChatGPTWindow::OnViewAuditLogClicked)
			]
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
			
			// Python Scripting Permission
			// Scene Editing Permission
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 2.0f)
			[
				SNew(SCheckBox)
				.IsChecked(this, &SChatGPTWindow::GetPythonScriptingPermission)
				.OnCheckStateChanged(this, &SChatGPTWindow::OnPythonScriptingPermissionChanged)
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("PythonScriptingPermission", "Allow Python Scripting (DANGEROUS)"))
				]
			]
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
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SAssignNew(ClearButton, SButton)
				.Text(LOCTEXT("ClearButton", "Clear"))
				.ToolTipText(LOCTEXT("ClearButtonTooltip", "Clear conversation history (Ctrl+L)"))
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
			.Text(LOCTEXT("APIKeyInfo", "Set OPENAI_API_KEY environment variable. Supports: code explanations, documentation generation, and code review."))
			.Text(LOCTEXT("APIKeyInfo", "💡 Tip: Set OPENAI_API_KEY environment variable with your API key"))
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)))
			.AutoWrapText(true)
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

SChatGPTWindow::~SChatGPTWindow()
{
	// Cleanup handlers
	ConsoleHandler.Reset();
	PythonHandler.Reset();
}

FReply SChatGPTWindow::OnSendMessageClicked()
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
	
	// Store the user message for later processing
	LastUserMessage = UserMessage;
	
	// Log the request
	if (FDocumentationHandler::IsDocumentationRequest(UserMessage))
	{
		FAuditLog::Get().LogOperation(TEXT("DocumentationRequest"), UserMessage);
	}
	else if (FDocumentationHandler::IsCodeExplanationRequest(UserMessage))
	{
		FAuditLog::Get().LogOperation(TEXT("CodeExplanationRequest"), UserMessage);
	}
	else
	{
		FAuditLog::Get().LogOperation(TEXT("ChatRequest"), UserMessage);
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
	// Add system message if this is the first user message
	if (Messages.Num() == 0)
	{
		TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject);
		SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
		
		FString SystemPrompt = TEXT("You are an AI assistant integrated into Unreal Engine 5.5 editor. ");
		
		if (bAllowFileIO)
		{
			SystemPrompt += TEXT("File I/O is ENABLED. You can read and write project files using these commands:\n\n");
			SystemPrompt += TEXT("To read a file:\nREAD_FILE: <filepath>\n\n");
			SystemPrompt += TEXT("To write a file:\nWRITE_FILE: <filepath>\nCONTENT_START\n<file content here>\nCONTENT_END\n\n");
			SystemPrompt += TEXT("Supported files: .ini config files (DefaultEngine.ini, DefaultGame.ini, etc.), .uproject files, and other project configuration files. ");
			SystemPrompt += TEXT("All file operations will be logged and require user confirmation before writing. ");
			SystemPrompt += TEXT("Use relative paths from project root (e.g., 'Config/DefaultEngine.ini').");
		}
		else
		{
			SystemPrompt += TEXT("File I/O is DISABLED. You cannot read or write project files. Inform the user if they need to enable File I/O permission.");
		}
		
		SystemMessage->SetStringField(TEXT("content"), SystemPrompt);
		Messages.Add(SystemMessage);
	}
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
		
		// Check if this was a documentation request and handle accordingly
		HandleDocumentationResponse(LastUserMessage, AssistantMessage);
		// Process any file operations in the response
		ProcessFileOperation(AssistantMessage);
		// Process the response for executable content
		ProcessAssistantResponse(AssistantMessage);
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
	
	// Log permission change
	FAuditLogger::Get().LogPermissionChange(TEXT("AssetWrite"), bAllowAssetWrite);
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
	
	// Log permission change
	FAuditLogger::Get().LogPermissionChange(TEXT("ConsoleCommands"), bAllowConsoleCommands);
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
	
	// Log permission change
	FAuditLogger::Get().LogPermissionChange(TEXT("FileIO"), bAllowFileIO);
}

void SChatGPTWindow::OnPythonScriptingPermissionChanged(ECheckBoxState NewState)
{
	HandlePermissionChange(
		bAllowPythonScripting,
		NewState,
		LOCTEXT("PythonScriptingWarning", 
			"WARNING: Enabling Python Scripting allows ChatGPT to execute Python code in your editor.\n\n"
			"This can lead to:\n"
			"- Asset modification or deletion\n"
			"- Project corruption\n"
			"- Arbitrary code execution\n"
			"- System-level operations\n\n"
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
void SChatGPTWindow::HandleDocumentationResponse(const FString& UserMessage, const FString& AssistantResponse)
{
	// Only process if it's a documentation request and File I/O permission is enabled
	if (!FDocumentationHandler::IsDocumentationRequest(UserMessage))
	{
		return;
	}
	
	if (!bAllowFileIO)
	{
		AppendMessage(TEXT("System"), 
			TEXT("Note: This appears to be a documentation request. Enable 'Allow File I/O Operations' to save documentation changes."));
		return;
	}
	
	// Try to parse the response for documentation changes
	FDocumentationChange Change;
	if (FDocumentationHandler::ParseDocumentationRequest(UserMessage, AssistantResponse, Change))
	{
		ShowDocumentationPreview(Change);
	}
}

void SChatGPTWindow::ShowDocumentationPreview(const FDocumentationChange& Change)
{
	// Show preview in conversation
	FString Preview = FDocumentationHandler::PreviewChange(Change);
	AppendMessage(TEXT("System"), Preview);
	
	// Ask user for confirmation
	FText ConfirmMessage = FText::Format(
		LOCTEXT("ConfirmDocChange", 
			"Do you want to apply this documentation change?\n\nFile: {0}\n\nThis will modify the file on disk."),
		FText::FromString(Change.FilePath)
	);
	
	EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, ConfirmMessage);
	
	if (Result == EAppReturnType::Yes)
	{
		FString Error;
		if (FDocumentationHandler::ApplyChange(Change, Error))
		{
			AppendMessage(TEXT("System"), 
				FString::Printf(TEXT("Successfully applied documentation change to: %s"), *Change.FilePath));
		}
		else
		{
			AppendMessage(TEXT("Error"), 
				FString::Printf(TEXT("Failed to apply documentation change: %s"), *Error));
bool SChatGPTWindow::ExtractFileOperationCommand(const FString& Message, FString& OutCommand, FString& OutFilePath, FString& OutContent)
{
	// Look for file operation commands in the message
	// Expected format: 
	// READ_FILE: <filepath>
	// WRITE_FILE: <filepath>
	// CONTENT_START
	// <content>
	// CONTENT_END

	const FString ReadFilePrefix = TEXT("READ_FILE:");
	const FString WriteFilePrefix = TEXT("WRITE_FILE:");
	const FString ContentStartMarker = TEXT("CONTENT_START");
	const FString ContentEndMarker = TEXT("CONTENT_END");

	if (Message.Contains(ReadFilePrefix))
	{
		OutCommand = TEXT("READ");
		int32 StartIdx = Message.Find(ReadFilePrefix) + ReadFilePrefix.Len();
		int32 EndIdx = Message.Find(TEXT("\n"), ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIdx);
		if (EndIdx == INDEX_NONE) EndIdx = Message.Len();
		OutFilePath = Message.Mid(StartIdx, EndIdx - StartIdx).TrimStartAndEnd();
		return true;
	}
	else if (Message.Contains(WriteFilePrefix))
	{
		OutCommand = TEXT("WRITE");
		int32 FilePathStart = Message.Find(WriteFilePrefix) + WriteFilePrefix.Len();
		int32 FilePathEnd = Message.Find(TEXT("\n"), ESearchCase::CaseSensitive, ESearchDir::FromStart, FilePathStart);
		if (FilePathEnd == INDEX_NONE) return false;
		
		OutFilePath = Message.Mid(FilePathStart, FilePathEnd - FilePathStart).TrimStartAndEnd();
		
		// Extract content between CONTENT_START and CONTENT_END
		int32 ContentStart = Message.Find(ContentStartMarker);
		int32 ContentEnd = Message.Find(ContentEndMarker);
		
		if (ContentStart != INDEX_NONE && ContentEnd != INDEX_NONE && ContentEnd > ContentStart)
		{
			ContentStart += ContentStartMarker.Len();
			OutContent = Message.Mid(ContentStart, ContentEnd - ContentStart).TrimStartAndEnd();
			return true;
		}
ECheckBoxState SChatGPTWindow::GetPythonScriptingPermission() const
{
	return bAllowPythonScripting ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SChatGPTWindow::ProcessAssistantResponse(const FString& Response)
{
	// Try to execute console command if permissions allow
	if (bAllowConsoleCommands)
	{
		if (TryExecuteConsoleCommand(Response))
		{
			return; // Command was executed
		}
	}
	
	// Try to execute Python script if permissions allow
	if (bAllowPythonScripting)
	{
		TryExecutePythonScript(Response);
	}
}

bool SChatGPTWindow::TryExecuteConsoleCommand(const FString& Response)
{
	if (!ConsoleHandler.IsValid())
	{
		return false;
	}
	
	// Look for console command markers in the response
	FString LowerResponse = Response.ToLower();
	if (!LowerResponse.Contains(TEXT("console command")) && 
		!LowerResponse.Contains(TEXT("execute command")) &&
		!LowerResponse.Contains(TEXT("run command")))
	{
		return false;
	}
	
	// Try to extract command from code blocks
	FString Command = ExtractCodeBlock(Response, TEXT("console"));
	if (Command.IsEmpty())
	{
		Command = ExtractCodeBlock(Response, TEXT(""));
	}
	
	if (!Command.IsEmpty())
	{
		AppendMessage(TEXT("System"), FString::Printf(TEXT("Attempting to execute console command: %s"), *Command));
		bool bSuccess = ConsoleHandler->ExecuteCommand(Command, true);
		
		if (bSuccess)
		{
			AppendMessage(TEXT("System"), TEXT("Command executed successfully."));
		}
		else
		{
			AppendMessage(TEXT("System"), TEXT("Command execution failed or was cancelled."));
		}
		
		return true;
	}
	
	return false;
}

bool SChatGPTWindow::TryExecutePythonScript(const FString& Response)
{
	if (!PythonHandler.IsValid())
	{
		return false;
	}
	
	// Check if this looks like a Python script response
	if (!PythonHandler->IsPythonScriptRequest(Response))
	{
		return false;
	}
	
	// Try to extract Python code from code blocks
	FString Script = ExtractCodeBlock(Response, TEXT("python"));
	if (Script.IsEmpty())
	{
		Script = ExtractCodeBlock(Response, TEXT("py"));
	}
	
	if (!Script.IsEmpty())
	{
		AppendMessage(TEXT("System"), TEXT("Python script detected. Preparing for execution..."));
		bool bSuccess = PythonHandler->ExecuteScript(Script, true);
		
		if (bSuccess)
		{
			AppendMessage(TEXT("System"), TEXT("Script executed successfully."));
		}
		else
		{
			AppendMessage(TEXT("System"), TEXT("Script execution failed or was cancelled."));
		}
		
		return true;
	}
	
	return false;
}

void SChatGPTWindow::ProcessFileOperation(const FString& AssistantMessage)
{
	// Check if file I/O permission is enabled
	if (!bAllowFileIO)
	{
		return; // Silently ignore if permission not enabled
	}

	FString Command, FilePath, Content;
	if (!ExtractFileOperationCommand(AssistantMessage, Command, FilePath, Content))
	{
		return; // No file operation command found
	}

	if (Command == TEXT("READ"))
	{
		FString FileContent;
		if (FProjectFileManager::Get().ReadProjectFile(FilePath, FileContent))
		{
			AppendMessage(TEXT("System"), FString::Printf(TEXT("Successfully read file: %s\n\nContent:\n%s"), *FilePath, *FileContent));
		}
		else
		{
			AppendMessage(TEXT("System"), FString::Printf(TEXT("Failed to read file: %s"), *FilePath));
		}
	}
	else if (Command == TEXT("WRITE"))
	{
		// Show preview and require confirmation
		ShowFileChangePreview(FilePath, Content);
	}
}

void SChatGPTWindow::ShowFileChangePreview(const FString& FilePath, const FString& NewContent)
{
	// Generate preview
	FProjectFileManager::FFileChangePreview Preview = FProjectFileManager::Get().PreviewFileChanges(FilePath, NewContent);

	if (!Preview.bIsValid)
	{
		AppendMessage(TEXT("System"), FString::Printf(TEXT("Failed to generate preview for: %s"), *FilePath));
		return;
	}

	// Display preview to user
	FString PreviewMessage = FString::Printf(
		TEXT("FILE CHANGE PREVIEW for: %s\n\n%s\n\nDo you want to apply these changes?"),
		*Preview.FilePath,
		*Preview.DiffPreview
	);

	EAppReturnType::Type Result = FMessageDialog::Open(
		EAppMsgType::YesNo,
		FText::FromString(PreviewMessage)
	);

	if (Result == EAppReturnType::Yes)
	{
		// Apply the changes
		if (FProjectFileManager::Get().WriteProjectFile(Preview))
		{
			AppendMessage(TEXT("System"), FString::Printf(TEXT("Successfully wrote file: %s"), *Preview.FilePath));
		}
		else
		{
			AppendMessage(TEXT("System"), FString::Printf(TEXT("Failed to write file: %s"), *Preview.FilePath));
		}
	}
	else
	{
		AppendMessage(TEXT("System"), TEXT("File changes cancelled by user"));
		FAuditLogger::Get().LogOperation("FileOperation", FString::Printf(TEXT("User cancelled changes to: %s"), *Preview.FilePath));
	}
FString SChatGPTWindow::ExtractCodeBlock(const FString& Response, const FString& Language) const
{
	// Look for markdown code blocks with optional language specifier
	FString StartMarker = Language.IsEmpty() ? TEXT("```") : FString::Printf(TEXT("```%s"), *Language);
	FString EndMarker = TEXT("```");
	
	int32 StartPos = Response.Find(StartMarker);
	if (StartPos == INDEX_NONE)
	{
		return TEXT("");
	}
	
	// Move past the start marker and newline
	StartPos += StartMarker.Len();
	if (StartPos < Response.Len() && (Response[StartPos] == '\n' || Response[StartPos] == '\r'))
	{
		StartPos++;
		if (StartPos < Response.Len() && Response[StartPos] == '\n')
		{
			StartPos++;
		}
	}
	
	// Find the end marker
	int32 EndPos = Response.Find(EndMarker, ESearchCase::IgnoreCase, ESearchDir::FromStart, StartPos);
	if (EndPos == INDEX_NONE)
	{
		return TEXT("");
	}
	
	// Extract the code
	FString Code = Response.Mid(StartPos, EndPos - StartPos);
	Code.TrimStartAndEndInline();
	
	return Code;
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
		AppendMessage(TEXT("System"), TEXT("Documentation change cancelled by user."));
		FAuditLog::Get().LogOperation(TEXT("DocChangeCancelled"), TEXT("User declined to apply changes"), Change.FilePath, true);
		AppendMessage(TEXT("System"), TEXT("Scene editing actions cancelled by user."));
	}
}

FReply SChatGPTWindow::OnViewAuditLogClicked()
{
	TArray<FAuditLogEntry> Entries = FAuditLog::Get().GetEntries();
	
	FString AuditLogText = TEXT("=== AUDIT LOG ===\n\n");
	
	if (Entries.Num() == 0)
	{
		AuditLogText += TEXT("No entries yet.\n");
	}
	else
	{
		// Show last 50 entries
		int32 StartIdx = FMath::Max(0, Entries.Num() - 50);
		for (int32 i = StartIdx; i < Entries.Num(); ++i)
		{
			AuditLogText += Entries[i].ToString() + TEXT("\n");
		}
	}
	
	AuditLogText += TEXT("\n=== END OF AUDIT LOG ===");
	
	// Display in a message dialog
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(AuditLogText));
	
	return FReply::Handled();
}

	// Get the audit log
	FString LogText = FAuditLogger::Get().ExportLogToString();

	// Create a dialog to show the log
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(LogText));

	return FReply::Handled();
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
	
	// Add messages array
	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	for (const auto& Msg : TestMessages)
	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	for (const auto& Msg : ExplanationMessages)
	{
		MessagesArray.Add(MakeShareable(new FJsonValueObject(Msg)));
	}
	RequestBody->SetArrayField(TEXT("messages"), MessagesArray);
	RequestBody->SetNumberField(TEXT("max_tokens"), 2000); // More tokens for code generation
	RequestBody->SetNumberField(TEXT("temperature"), 0.3); // Lower temperature for more focused code generation
	
	// Serialize to JSON string
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
	
	// Bind response callback
	HttpRequest->OnProcessRequestComplete().BindSP(this, &SChatGPTWindow::OnTestGenerationResponseReceived);
	// Bind callback
	HttpRequest->OnProcessRequestComplete().BindSP(this, &SChatGPTWindow::OnBlueprintExplanationResponseReceived, BlueprintName);
	
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
	// Extract content
	const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
	if (ResponseObject->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray->Num() > 0)
	{
		TSharedPtr<FJsonObject> Choice = (*ChoicesArray)[0]->AsObject();
		if (!Choice.IsValid())
		{
			AppendMessage(TEXT("Error"), TEXT("Unexpected API response format."));
			AppendMessage(TEXT("Error"), TEXT("Invalid response format."));
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
	return FText::Format(LOCTEXT("FontSizeText", "Font Size: {0}"), FontSize);
}

#undef LOCTEXT_NAMESPACE
