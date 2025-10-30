// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MCP/MCPTool.h"

/**
 * Simple echo tool for testing MCP
 * Echoes back the input message
 */
class FEchoTool : public FMCPToolBase
{
public:
	FEchoTool();
	
	virtual TSharedPtr<FJsonObject> GetInputSchema() const override;
	virtual TSharedPtr<FJsonObject> Execute(const TSharedPtr<FJsonObject>& Arguments) override;
};
