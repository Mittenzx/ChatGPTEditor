// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class FMCPServer;
class SMultiLineEditableTextBox;
class SEditableTextBox;

/**
 * Simple test window for MCP server
 * Allows manual JSON-RPC message testing
 */
class SMCPTestWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMCPTestWindow)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SMCPTestWindow();

private:
	// UI event handlers
	FReply OnSendMessageClicked();
	FReply OnClearClicked();
	FReply OnInitializeClicked();
	FReply OnListToolsClicked();
	
	// Helper to append text to output
	void AppendOutput(const FString& Text);
	
	// MCP Server
	TSharedPtr<FMCPServer> MCPServer;
	
	// UI widgets
	TSharedPtr<SEditableTextBox> InputTextBox;
	TSharedPtr<SMultiLineEditableTextBox> OutputTextBox;
	
	// Message ID counter
	int32 MessageIdCounter;
};
