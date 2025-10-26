// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;
class SScrollBox;
class SButton;
struct FBlueprintPreviewData;
struct FBlueprintExplanation;

/**
 * Slate widget for ChatGPT window
 * Provides UI for sending messages to OpenAI API and displaying responses
 * Includes security permission toggles for destructive operations
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
	
	/** Handle keyboard input for shortcuts */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	// UI event handlers
	FReply OnSendMessageClicked();
	FReply OnClearHistoryClicked();
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
	
	// Helper functions
	void AppendMessage(const FString& Role, const FString& Message);
	FString GetAPIKey() const;
	bool IsAPIKeyValid() const;
	void HandlePermissionChange(bool& bPermissionFlag, ECheckBoxState NewState, const FText& WarningText);
	
	// Asset automation
	void ProcessAssetAutomation(const FString& Response);
	
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
	TSharedPtr<SEditableTextBox> BlueprintPromptBox;
	TSharedPtr<SEditableTextBox> BlueprintNameBox;
	TSharedPtr<SButton> SendButton;
	TSharedPtr<SButton> ClearButton;
	
	// Conversation state
	FString ConversationHistory;
	TArray<TSharedPtr<FJsonObject>> Messages;
	
	// Security permissions (default to OFF for safety)
	bool bAllowAssetWrite = false;
	bool bAllowConsoleCommands = false;
	bool bAllowFileIO = false;
	
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
