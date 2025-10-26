// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SceneEditingTypes.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;

/**
 * Preview dialog for scene editing operations
 * Shows pending changes and requires explicit confirmation
 */
class SSceneEditPreviewDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSceneEditPreviewDialog)
	{}
		SLATE_ARGUMENT(TArray<FSceneEditAction>, Actions)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	/** Check if user confirmed the actions */
	bool WasConfirmed() const { return bConfirmed; }

private:
	// Button handlers
	FReply OnConfirmClicked();
	FReply OnCancelClicked();

	// Generate preview text
	FString GeneratePreviewText(const TArray<FSceneEditAction>& Actions);

private:
	TArray<FSceneEditAction> PendingActions;
	bool bConfirmed = false;
	TSharedPtr<SMultiLineEditableTextBox> PreviewTextBox;
};
