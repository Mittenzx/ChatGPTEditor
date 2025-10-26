// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;

/**
 * Blueprint preview data structure
 */
struct FBlueprintPreviewData
{
	FString UserPrompt;
	FString GeneratedDescription;
	FString NodesList;
	FString ConnectionsList;
	bool bIsValid;

	FBlueprintPreviewData()
		: bIsValid(false)
	{
	}
};

/**
 * Slate widget for Blueprint Scripting Assistant preview and confirmation dialog
 * Shows a preview of generated Blueprint logic before creation
 */
class SBlueprintPreviewDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBlueprintPreviewDialog)
		: _PreviewData()
	{}
		SLATE_ARGUMENT(FBlueprintPreviewData, PreviewData)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	/** Get user decision */
	bool WasApproved() const { return bWasApproved; }

private:
	FReply OnApproveClicked();
	FReply OnRejectClicked();

	FBlueprintPreviewData PreviewData;
	bool bWasApproved = false;
	TSharedPtr<class SWindow> ParentWindow;
};

/**
 * Blueprint explanation result structure
 */
struct FBlueprintExplanation
{
	FString BlueprintName;
	FString Summary;
	FString StepByStep;
	bool bIsValid;

	FBlueprintExplanation()
		: bIsValid(false)
	{
	}
};
