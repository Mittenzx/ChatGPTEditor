// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Dom/JsonObject.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "DocumentationHandler.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;
class SScrollBox;
class FExternalAPIHandler;
struct FAPIRequestDetails;
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
 * Supports external API and web integration with preview and approval
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
	/**
	 * Callback when test generation response is received from OpenAI
	 * @param Request The HTTP request
	 * @param Response The HTTP response containing test code
	 * @param bWasSuccessful Whether the request was successful
	 */
	void OnTestGenerationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	/**
	 * Show preview dialog for generated test code
	 * @param TestCode The generated test code
	 * @param TestName Name of the test
	 */
	void ShowTestCodePreview(const FString& TestCode, const FString& TestName);
	
	/**
	 * Execute a test by name
	 * @param TestName Name of the test to execute
	 */
	void ExecuteTest(const FString& TestName);
	
	/**
	 * Display test execution results
	 * @param TestName Name of the test
	 * @param bSuccess Whether test passed
	 * @param Results Test output/results
	 */
	void DisplayTestResults(const FString& TestName, bool bSuccess, const FString& Results);
	
	// Permission toggle handlers - called when security checkboxes are changed
	void OnAssetWritePermissionChanged(ECheckBoxState NewState);
	void OnConsoleCommandPermissionChanged(ECheckBoxState NewState);
	void OnFileIOPermissionChanged(ECheckBoxState NewState);
	void OnExternalAPIPermissionChanged(ECheckBoxState NewState);
	void OnPythonScriptingPermissionChanged(ECheckBoxState NewState);
	void OnSceneEditingPermissionChanged(ECheckBoxState NewState);
	
	// Permission state getters - return current state of security checkboxes
	ECheckBoxState GetAssetWritePermission() const;
	ECheckBoxState GetConsoleCommandPermission() const;
	ECheckBoxState GetFileIOPermission() const;
	ECheckBoxState GetExternalAPIPermission() const;
	
	/**
	 * Process potential external API request from user message
	 * @param UserMessage The user's message that might contain an API request
	 */
	void ProcessPotentialAPIRequest(const FString& UserMessage);
	
	/**
	 * Show preview dialog for external API request
	 * @param Details The API request details to preview
	 */
	void ShowAPIPreviewDialog(const FAPIRequestDetails& Details);
	
	/**
	 * Callback when user approves an API request
	 * @param Details The approved API request details
	 */
	void OnAPIRequestApproved(const FAPIRequestDetails& Details);
	
	/**
	 * Callback when user denies an API request
	 * @param Details The denied API request details
	 */
	void OnAPIRequestDenied(const FAPIRequestDetails& Details);
	
	/**
	 * Callback when API execution completes
	 * @param bSuccess Whether the API call succeeded
	 * @param Response The API response
	 */
	void OnAPIExecutionComplete(bool bSuccess, const FString& Response);
	
	/**
	 * Show preview dialog for generated code
	 * @param Code The generated code
	 * @param Description Description of what the code does
	 */
	void ShowCodePreviewDialog(const FString& Code, const FString& Description);
	
	// Test type combo box helpers
	TSharedRef<SWidget> MakeTestTypeComboWidget(TSharedPtr<FString> InItem);
	void OnTestTypeSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	FText GetCurrentTestTypeText() const;
	ECheckBoxState GetPythonScriptingPermission() const;
	ECheckBoxState GetSceneEditingPermission() const;
	
	/**
	 * Process scene editing command from ChatGPT
	 * @param Command The scene editing command to process
	 */
	void ProcessSceneEditingCommand(const FString& Command);
	
	// Accessibility helpers
	void UpdateFontSize();
	FText GetFontSizeButtonText() const;

	/**
	 * Process file operation from ChatGPT response
	 * @param AssistantMessage The assistant's message containing file operation
	 */
	void ProcessFileOperation(const FString& AssistantMessage);
	
	/**
	 * Extract file operation command from message
	 * @param Message The message to parse
	 * @param OutCommand The extracted command
	 * @param OutFilePath The target file path
	 * @param OutContent The file content
	 * @return True if command was successfully extracted
	 */
	bool ExtractFileOperationCommand(const FString& Message, FString& OutCommand, FString& OutFilePath, FString& OutContent);
	
	/**
	 * Show file change preview dialog
	 * @param FilePath The file to be changed
	 * @param NewContent The new file content
	 */
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
	bool bAllowExternalAPI = false;
	
	// External API handler
	TSharedPtr<FExternalAPIHandler> APIHandler;
	
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
