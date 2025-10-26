// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "DocumentationHandler.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;
class SScrollBox;

/**
 * Slate widget for ChatGPT window
 * Provides UI for sending messages to OpenAI API and displaying responses
 * Includes security permission toggles for destructive operations
 * Supports documentation generation and code review features
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
	FReply OnViewAuditLogClicked();
	
	// HTTP request handling
	void SendRequestToOpenAI(const FString& UserMessage);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	// Documentation and code review handlers
	void HandleDocumentationResponse(const FString& UserMessage, const FString& AssistantResponse);
	void ShowDocumentationPreview(const FDocumentationChange& Change);
	
	// Helper functions
	void AppendMessage(const FString& Role, const FString& Message);
	FString GetAPIKey() const;
	bool IsAPIKeyValid() const;
	void HandlePermissionChange(bool& bPermissionFlag, ECheckBoxState NewState, const FText& WarningText);
	
	// Permission toggle handlers
	void OnAssetWritePermissionChanged(ECheckBoxState NewState);
	void OnConsoleCommandPermissionChanged(ECheckBoxState NewState);
	void OnFileIOPermissionChanged(ECheckBoxState NewState);
	
	ECheckBoxState GetAssetWritePermission() const;
	ECheckBoxState GetConsoleCommandPermission() const;
	ECheckBoxState GetFileIOPermission() const;

private:
	// UI widgets
	TSharedPtr<SEditableTextBox> MessageInputBox;
	TSharedPtr<SMultiLineEditableTextBox> ConversationHistoryBox;
	TSharedPtr<SScrollBox> ConversationScrollBox;
	
	// Conversation state
	FString ConversationHistory;
	TArray<TSharedPtr<FJsonObject>> Messages;
	FString LastUserMessage;  // Track last user message for documentation requests
	
	// Security permissions (default to OFF for safety)
	bool bAllowAssetWrite = false;
	bool bAllowConsoleCommands = false;
	bool bAllowFileIO = false;
};
