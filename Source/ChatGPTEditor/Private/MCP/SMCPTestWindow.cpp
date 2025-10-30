// Copyright Epic Games, Inc. All Rights Reserved.

#include "SMCPTestWindow.h"
#include "MCP/MCPServer.h"
#include "MCP/Tools/EchoTool.h"
#include "MCP/Tools/SpawnActorTool.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SMCPTestWindow"

void SMCPTestWindow::Construct(const FArguments& InArgs)
{
	MessageIdCounter = 1;
	
	// Create MCP Server
	MCPServer = MakeShared<FMCPServer>();
	MCPServer->Initialize();
	
	// Register tools
	MCPServer->RegisterTool(MakeShared<FEchoTool>());
	MCPServer->RegisterTool(MakeShared<FSpawnActorTool>());
	
	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Header
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("Title", "MCP Test Window"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
		]
		
		// Quick action buttons
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("Initialize", "Initialize"))
				.OnClicked(this, &SMCPTestWindow::OnInitializeClicked)
				.ToolTipText(LOCTEXT("InitializeTooltip", "Send initialize request to MCP server"))
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("ListTools", "List Tools"))
				.OnClicked(this, &SMCPTestWindow::OnListToolsClicked)
				.ToolTipText(LOCTEXT("ListToolsTooltip", "List all available MCP tools"))
			]
		]
		
		// Input section
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 5.0f)
		[
			SNew(SVerticalBox)
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 5.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("InputLabel", "JSON-RPC Request:"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
			]
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(InputTextBox, SEditableTextBox)
				.HintText(LOCTEXT("InputHint", "Enter JSON-RPC 2.0 message..."))
			]
		]
		
		// Action buttons
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 5.0f)
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("Send", "Send"))
				.OnClicked(this, &SMCPTestWindow::OnSendMessageClicked)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("Clear", "Clear Output"))
				.OnClicked(this, &SMCPTestWindow::OnClearClicked)
			]
		]
		
		// Output section
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(10.0f, 5.0f)
		[
			SNew(SVerticalBox)
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 5.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("OutputLabel", "Output:"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
			]
			
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SAssignNew(OutputTextBox, SMultiLineEditableTextBox)
				.IsReadOnly(true)
				.AlwaysShowScrollbars(true)
			]
		]
	];
	
	AppendOutput(TEXT("MCP Server initialized and ready.\n"));
	AppendOutput(TEXT("Registered tools: echo, spawn_actor\n"));
	AppendOutput(TEXT("Click 'Initialize' to start, or enter custom JSON-RPC messages.\n\n"));
}

SMCPTestWindow::~SMCPTestWindow()
{
	if (MCPServer.IsValid())
	{
		MCPServer->Shutdown();
	}
}

FReply SMCPTestWindow::OnSendMessageClicked()
{
	if (!InputTextBox.IsValid() || !OutputTextBox.IsValid() || !MCPServer.IsValid())
	{
		return FReply::Handled();
	}
	
	FString InputMessage = InputTextBox->GetText().ToString();
	if (InputMessage.IsEmpty())
	{
		return FReply::Handled();
	}
	
	AppendOutput(FString::Printf(TEXT(">> Request:\n%s\n\n"), *InputMessage));
	
	// Process message
	FString Response = MCPServer->ProcessMessage(InputMessage);
	
	AppendOutput(FString::Printf(TEXT("<< Response:\n%s\n\n"), *Response));
	
	// Clear input
	InputTextBox->SetText(FText::GetEmpty());
	
	return FReply::Handled();
}

FReply SMCPTestWindow::OnClearClicked()
{
	if (OutputTextBox.IsValid())
	{
		OutputTextBox->SetText(FText::GetEmpty());
	}
	return FReply::Handled();
}

FReply SMCPTestWindow::OnInitializeClicked()
{
	FString InitMessage = FString::Printf(TEXT(R"({
  "jsonrpc": "2.0",
  "id": %d,
  "method": "initialize",
  "params": {
    "protocolVersion": "2025-03-26",
    "capabilities": {},
    "clientInfo": {"name": "test-client", "version": "1.0.0"}
  }
})"), MessageIdCounter++);
	
	if (InputTextBox.IsValid())
	{
		InputTextBox->SetText(FText::FromString(InitMessage));
	}
	
	return OnSendMessageClicked();
}

FReply SMCPTestWindow::OnListToolsClicked()
{
	FString ListMessage = FString::Printf(TEXT(R"({
  "jsonrpc": "2.0",
  "id": %d,
  "method": "tools/list"
})"), MessageIdCounter++);
	
	if (InputTextBox.IsValid())
	{
		InputTextBox->SetText(FText::FromString(ListMessage));
	}
	
	return OnSendMessageClicked();
}

void SMCPTestWindow::AppendOutput(const FString& Text)
{
	if (OutputTextBox.IsValid())
	{
		FString CurrentText = OutputTextBox->GetText().ToString();
		OutputTextBox->SetText(FText::FromString(CurrentText + Text));
	}
}

#undef LOCTEXT_NAMESPACE
