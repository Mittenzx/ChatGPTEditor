// Copyright Epic Games, Inc. All Rights Reserved.

#include "SSceneEditPreviewDialog.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SWindow.h"
#include "Styling/AppStyle.h"
#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "SSceneEditPreviewDialog"

void SSceneEditPreviewDialog::Construct(const FArguments& InArgs)
{
	PendingActions = InArgs._Actions;
	bConfirmed = false;

	ChildSlot
	[
		SNew(SBox)
		.MinDesiredWidth(600.0f)
		.MinDesiredHeight(400.0f)
		[
			SNew(SVerticalBox)
			
			// Header
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("PreviewTitle", "Scene Editing Preview"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
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
				.Text(LOCTEXT("PreviewWarning", "The following changes will be applied to your level. Please review carefully before confirming."))
				.ColorAndOpacity(FLinearColor(1.0f, 0.7f, 0.0f))
				.AutoWrapText(true)
			]
			
			// Preview text area
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(10.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(5.0f)
				[
					SAssignNew(PreviewTextBox, SMultiLineEditableTextBox)
					.IsReadOnly(true)
					.Text(FText::FromString(GeneratePreviewText(PendingActions)))
					.AutoWrapText(true)
				]
			]
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
			]
			
			// Button bar
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f)
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SBox)
				]
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 5.0f, 0.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("CancelButton", "Cancel"))
					.OnClicked(this, &SSceneEditPreviewDialog::OnCancelClicked)
				]
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("ConfirmButton", "Confirm & Apply"))
					.OnClicked(this, &SSceneEditPreviewDialog::OnConfirmClicked)
					.ButtonColorAndOpacity(FLinearColor(0.0f, 0.7f, 0.0f))
				]
			]
		]
	];
}

FReply SSceneEditPreviewDialog::OnConfirmClicked()
{
	bConfirmed = true;
	
	// Close the window
	TSharedPtr<SWindow> ContainingWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
	if (ContainingWindow.IsValid())
	{
		ContainingWindow->RequestDestroyWindow();
	}
	
	return FReply::Handled();
}

FReply SSceneEditPreviewDialog::OnCancelClicked()
{
	bConfirmed = false;
	
	// Close the window
	TSharedPtr<SWindow> ContainingWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
	if (ContainingWindow.IsValid())
	{
		ContainingWindow->RequestDestroyWindow();
	}
	
	return FReply::Handled();
}

FString SSceneEditPreviewDialog::GeneratePreviewText(const TArray<FSceneEditAction>& Actions)
{
	FString PreviewText = TEXT("=== PENDING SCENE CHANGES ===\n\n");

	for (int32 i = 0; i < Actions.Num(); ++i)
	{
		const FSceneEditAction& Action = Actions[i];
		
		PreviewText += FString::Printf(TEXT("Action %d:\n"), i + 1);
		
		switch (Action.Operation)
		{
			case ESceneEditOperation::SpawnActor:
				PreviewText += FString::Printf(TEXT("  Operation: SPAWN %d x %s\n"), Action.Count, *Action.ActorClass);
				if (!Action.PropertyName.IsEmpty())
				{
					PreviewText += FString::Printf(TEXT("  Location: %s\n"), *Action.PropertyName);
				}
				else
				{
					PreviewText += FString::Printf(TEXT("  Location: %s\n"), *Action.Location.ToString());
				}
				break;
				
			case ESceneEditOperation::DeleteActor:
				PreviewText += FString::Printf(TEXT("  Operation: DELETE\n"));
				PreviewText += FString::Printf(TEXT("  Target: All actors matching '%s'\n"), *Action.SearchPattern);
				PreviewText += TEXT("  WARNING: This operation cannot be undone!\n");
				break;
				
			case ESceneEditOperation::MoveActor:
				PreviewText += FString::Printf(TEXT("  Operation: MOVE\n"));
				PreviewText += FString::Printf(TEXT("  Target: All actors matching '%s'\n"), *Action.SearchPattern);
				PreviewText += FString::Printf(TEXT("  Offset: %s\n"), *Action.Location.ToString());
				break;
				
			case ESceneEditOperation::ModifyProperty:
				PreviewText += FString::Printf(TEXT("  Operation: MODIFY PROPERTY\n"));
				PreviewText += FString::Printf(TEXT("  Target: All actors matching '%s'\n"), *Action.SearchPattern);
				PreviewText += FString::Printf(TEXT("  Property: %s = %s\n"), *Action.PropertyName, *Action.PropertyValue);
				break;
		}
		
		PreviewText += FString::Printf(TEXT("  Command: \"%s\"\n"), *Action.Description);
		PreviewText += TEXT("\n");
	}

	PreviewText += TEXT("=== END OF PREVIEW ===\n");
	return PreviewText;
}

#undef LOCTEXT_NAMESPACE
