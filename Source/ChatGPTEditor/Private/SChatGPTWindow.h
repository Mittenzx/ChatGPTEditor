// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;
class SScrollBox;

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
	
	/** Handle keyboard input for shortcuts */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	// UI event handlers
	FReply OnSendMessageClicked();
	FReply OnClearHistoryClicked();
	FReply OnIncreaseFontSize();
	FReply OnDecreaseFontSize();
	FReply OnResetFontSize();
	
	// HTTP request handling
	void SendRequestToOpenAI(const FString& UserMessage);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
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
	
	// Accessibility helpers
	void UpdateFontSize();
	FText GetFontSizeButtonText() const;

private:
	// UI widgets
	TSharedPtr<SEditableTextBox> MessageInputBox;
	TSharedPtr<SMultiLineEditableTextBox> ConversationHistoryBox;
	TSharedPtr<SScrollBox> ConversationScrollBox;
	TSharedPtr<SButton> SendButton;
	TSharedPtr<SButton> ClearButton;
	
	// Conversation state
	FString ConversationHistory;
	TArray<TSharedPtr<FJsonObject>> Messages;
	
	// Security permissions (default to OFF for safety)
	bool bAllowAssetWrite = false;
	bool bAllowConsoleCommands = false;
	bool bAllowFileIO = false;
	
	// Accessibility settings
	int32 FontSize = 10;
	const int32 MinFontSize = 8;
	const int32 MaxFontSize = 24;
	const int32 DefaultFontSize = 10;
	
	// UI state
	bool bIsRequestInProgress = false;
};
