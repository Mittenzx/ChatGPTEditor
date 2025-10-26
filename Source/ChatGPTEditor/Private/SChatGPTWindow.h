// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;
class SScrollBox;
class SComboBox;

/**
 * Slate widget for ChatGPT window
 * Provides UI for sending messages to OpenAI API and displaying responses
 * Includes security permission toggles for destructive operations
 */
class SChatGPTWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SChatGPTWindow)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	// UI event handlers
	FReply OnSendMessageClicked();
	FReply OnClearHistoryClicked();
	FReply OnGenerateTestClicked();
	FReply OnRunTestClicked();
	FReply OnConfirmTestCodeClicked();
	FReply OnCancelTestCodeClicked();
	
	// HTTP request handling
	void SendRequestToOpenAI(const FString& UserMessage);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
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
	
	// Permission toggle handlers
	void OnAssetWritePermissionChanged(ECheckBoxState NewState);
	void OnConsoleCommandPermissionChanged(ECheckBoxState NewState);
	void OnFileIOPermissionChanged(ECheckBoxState NewState);
	
	ECheckBoxState GetAssetWritePermission() const;
	ECheckBoxState GetConsoleCommandPermission() const;
	ECheckBoxState GetFileIOPermission() const;
	
	// Test type combo box helpers
	TSharedRef<SWidget> MakeTestTypeComboWidget(TSharedPtr<FString> InItem);
	void OnTestTypeSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	FText GetCurrentTestTypeText() const;

private:
	// UI widgets
	TSharedPtr<SEditableTextBox> MessageInputBox;
	TSharedPtr<SMultiLineEditableTextBox> ConversationHistoryBox;
	TSharedPtr<SScrollBox> ConversationScrollBox;
	TSharedPtr<SEditableTextBox> TestPromptInputBox;
	TSharedPtr<SComboBox<TSharedPtr<FString>>> TestTypeComboBox;
	TSharedPtr<SMultiLineEditableTextBox> TestCodePreviewBox;
	TSharedPtr<SWindow> TestPreviewWindow;
	
	// Conversation state
	FString ConversationHistory;
	TArray<TSharedPtr<FJsonObject>> Messages;
	
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
};
