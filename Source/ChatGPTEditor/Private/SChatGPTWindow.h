// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "DocumentationHandler.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;
class SScrollBox;
template <typename OptionType> class SComboBox;
class SWindow;
class FProjectFileManager;
class FChatGPTConsoleHandler;
class FChatGPTPythonHandler;
class SButton;
struct FBlueprintPreviewData;
struct FBlueprintExplanation;

/**
 * Slate widget for ChatGPT window
 * Provides UI for sending messages to OpenAI API and displaying responses
 * Includes security permission toggles for destructive operations
 * Supports documentation generation and code review features
 * Supports console command execution and Python script generation
 * Includes Blueprint Scripting Assistant for generating and explaining Blueprints
 * Includes Asset Automation for creating and managing Unreal Engine assets
 * Includes accessibility features like adjustable font sizes and keyboard shortcuts
 */
class SChatGPTWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SChatGPTWindow)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	/** Destructor */
	virtual ~SChatGPTWindow();
	/** Handle keyboard input for shortcuts */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	// UI event handlers
	FReply OnSendMessageClicked();
	FReply OnClearHistoryClicked();
	FReply OnGenerateTestClicked();
	FReply OnRunTestClicked();
	FReply OnConfirmTestCodeClicked();
	FReply OnCancelTestCodeClicked();
	FReply OnViewAuditLogClicked();
	FReply OnGenerateBlueprintClicked();
	FReply OnExplainBlueprintClicked();
	FReply OnExportAuditLogClicked();
	FReply OnIncreaseFontSize();
	FReply OnDecreaseFontSize();
	FReply OnResetFontSize();
	
	// HTTP request handling
	void SendRequestToOpenAI(const FString& UserMessage);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnBlueprintGenerationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FString& UserPrompt);
	void OnBlueprintExplanationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FString& BlueprintName);
	
	// Blueprint assistant functions
	void ShowBlueprintPreview(const FBlueprintPreviewData& PreviewData, const FString& UserPrompt);
	void ProcessBlueprintGeneration(const FString& UserPrompt, bool bApproved);
	void DisplayBlueprintExplanation(const FBlueprintExplanation& Explanation);
	FBlueprintPreviewData ParseBlueprintGenerationResponse(const FString& ResponseContent);
	FBlueprintExplanation ParseBlueprintExplanationResponse(const FString& ResponseContent);
	
	// Asset automation
	void ProcessAssetAutomation(const FString& Response);
	
	// Response processing
	void ProcessAssistantResponse(const FString& Response);
	bool TryExecuteConsoleCommand(const FString& Response);
	bool TryExecutePythonScript(const FString& Response);
	FString ExtractCodeBlock(const FString& Response, const FString& Language) const;
	
	// Documentation and code review handlers
	void HandleDocumentationResponse(const FString& UserMessage, const FString& AssistantResponse);
	void ShowDocumentationPreview(const FDocumentationChange& Change);
	
	// Helper functions
	void AppendMessage(const FString& Role, const FString& Message);
	FString GetAPIKey() const;
	bool IsAPIKeyValid() const;
	void HandlePermissionChange(bool& bPermissionFlag, ECheckBoxState NewState, const FText& WarningText);
	
	// Test automation helpers
	void SendTestGenerationRequest(const FString& TestPrompt, const FString& TestType);
	void OnTestGenerationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void ShowTestCodePreview(const FString& TestCode, const FString& TestName);
	void ExecuteTest(const FString& TestName);
	void DisplayTestResults(const FString& TestName, bool bSuccess, const FString& Results);
	// Asset automation
	void ProcessAssetAutomation(const FString& Response);
	
	// Permission toggle handlers
	void OnAssetWritePermissionChanged(ECheckBoxState NewState);
	void OnConsoleCommandPermissionChanged(ECheckBoxState NewState);
	void OnFileIOPermissionChanged(ECheckBoxState NewState);
	void OnPythonScriptingPermissionChanged(ECheckBoxState NewState);
	void OnSceneEditingPermissionChanged(ECheckBoxState NewState);
	
	ECheckBoxState GetAssetWritePermission() const;
	ECheckBoxState GetConsoleCommandPermission() const;
	ECheckBoxState GetFileIOPermission() const;
	
	// Test type combo box helpers
	TSharedRef<SWidget> MakeTestTypeComboWidget(TSharedPtr<FString> InItem);
	void OnTestTypeSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	FText GetCurrentTestTypeText() const;
	ECheckBoxState GetPythonScriptingPermission() const;
	ECheckBoxState GetSceneEditingPermission() const;
	
	// Scene editing
	void ProcessSceneEditingCommand(const FString& Command);
	FReply OnViewAuditLogClicked();
	
	// Accessibility helpers
	void UpdateFontSize();
	FText GetFontSizeButtonText() const;

	// File operation handlers
	void ProcessFileOperation(const FString& AssistantMessage);
	bool ExtractFileOperationCommand(const FString& Message, FString& OutCommand, FString& OutFilePath, FString& OutContent);
	void ShowFileChangePreview(const FString& FilePath, const FString& NewContent);

private:
	// UI widgets
	TSharedPtr<SEditableTextBox> MessageInputBox;
	TSharedPtr<SMultiLineEditableTextBox> ConversationHistoryBox;
	TSharedPtr<SScrollBox> ConversationScrollBox;
	TSharedPtr<SEditableTextBox> TestPromptInputBox;
	TSharedPtr<SComboBox<TSharedPtr<FString>>> TestTypeComboBox;
	TSharedPtr<SMultiLineEditableTextBox> TestCodePreviewBox;
	TSharedPtr<SWindow> TestPreviewWindow;
	TSharedPtr<SEditableTextBox> BlueprintPromptBox;
	TSharedPtr<SEditableTextBox> BlueprintNameBox;
	TSharedPtr<SButton> SendButton;
	TSharedPtr<SButton> ClearButton;
	
	// Conversation state
	FString ConversationHistory;
	TArray<TSharedPtr<FJsonObject>> Messages;
	FString LastUserMessage;  // Track last user message for documentation requests
	
	// Security permissions (default to OFF for safety)
	bool bAllowAssetWrite = false;
	bool bAllowConsoleCommands = false;
	bool bAllowFileIO = false;
	
	// Test automation state
	TArray<TSharedPtr<FString>> TestTypes;
	TSharedPtr<FString> CurrentTestType;
	FString PendingTestCode;
	FString PendingTestName;
	bool bWaitingForTestGeneration = false;
	bool bAllowPythonScripting = false;
	
	// Console and scripting handlers
	TSharedPtr<FChatGPTConsoleHandler> ConsoleHandler;
	TSharedPtr<FChatGPTPythonHandler> PythonHandler;
	bool bAllowSceneEditing = false;
	
	// Blueprint assistant state
	FString PendingBlueprintPrompt;
	
	// Accessibility settings
	int32 FontSize = 10;
	const int32 MinFontSize = 8;
	const int32 MaxFontSize = 24;
	const int32 DefaultFontSize = 10;
	
	// UI state
	bool bIsRequestInProgress = false;
};
