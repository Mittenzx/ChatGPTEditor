// Copyright Epic Games, Inc. All Rights Reserved.

#include "SBlueprintAssistantPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SBlueprintAssistantPanel"

void SBlueprintPreviewDialog::Construct(const FArguments& InArgs)
{
	PreviewData = InArgs._PreviewData;
	bWasApproved = false;

	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("PreviewTitle", "Blueprint Generation Preview"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]
		
		// User Prompt
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SVerticalBox)
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("UserPromptLabel", "Your Request:"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(PreviewData.UserPrompt))
				.AutoWrapText(true)
			]
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]
		
		// Generated Description
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SVerticalBox)
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("DescriptionLabel", "Generated Blueprint Description:"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(PreviewData.GeneratedDescription))
				.AutoWrapText(true)
			]
		]
		
		// Preview Content Area
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(10.0f)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(5.0f)
			[
				SNew(SScrollBox)
				
				+ SScrollBox::Slot()
				[
					SNew(SVerticalBox)
					
					// Nodes section
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(5.0f)
					[
						SNew(SVerticalBox)
						
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("NodesLabel", "Nodes to be created:"))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
						]
						
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(5.0f, 2.0f)
						[
							SNew(STextBlock)
							.Text(FText::FromString(PreviewData.NodesList))
							.AutoWrapText(true)
						]
					]
					
					// Connections section
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(5.0f)
					[
						SNew(SVerticalBox)
						
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("ConnectionsLabel", "Connections:"))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
						]
						
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(5.0f, 2.0f)
						[
							SNew(STextBlock)
							.Text(FText::FromString(PreviewData.ConnectionsList))
							.AutoWrapText(true)
						]
					]
				]
			]
		]
		
		// Warning message
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.DarkGroupBorder"))
			.Padding(5.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SecurityWarning", 
					"⚠️ WARNING: Review this Blueprint carefully before approval.\n"
					"This will create actual Blueprint nodes in your project.\n"
					"Make sure you have backups before proceeding."))
				.ColorAndOpacity(FLinearColor(1.0f, 0.8f, 0.0f))
				.AutoWrapText(true)
			]
		]
		
		// Buttons
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
				.Text(LOCTEXT("RejectButton", "Reject"))
				.OnClicked(this, &SBlueprintPreviewDialog::OnRejectClicked)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("ApproveButton", "Approve & Create"))
				.OnClicked(this, &SBlueprintPreviewDialog::OnApproveClicked)
				.ButtonColorAndOpacity(FLinearColor(0.0f, 0.6f, 0.0f))
			]
		]
	];
}

FReply SBlueprintPreviewDialog::OnApproveClicked()
{
	bWasApproved = true;
	
	if (ParentWindow.IsValid())
	{
		ParentWindow->RequestDestroyWindow();
	}
	else
	{
		FSlateApplication::Get().FindWidgetWindow(AsShared())->RequestDestroyWindow();
	}
	
	return FReply::Handled();
}

FReply SBlueprintPreviewDialog::OnRejectClicked()
{
	bWasApproved = false;
	
	if (ParentWindow.IsValid())
	{
		ParentWindow->RequestDestroyWindow();
	}
	else
	{
		FSlateApplication::Get().FindWidgetWindow(AsShared())->RequestDestroyWindow();
	}
	
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
