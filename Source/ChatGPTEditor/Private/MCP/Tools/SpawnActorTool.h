// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MCP/MCPTool.h"

/**
 * Tool for spawning actors in the Unreal Engine level
 */
class FSpawnActorTool : public FMCPToolBase
{
public:
	FSpawnActorTool();
	
	virtual TSharedPtr<FJsonObject> GetInputSchema() const override;
	virtual TSharedPtr<FJsonObject> Execute(const TSharedPtr<FJsonObject>& Arguments) override;
	
	virtual bool RequiresConfirmation() const override { return true; }
	virtual TArray<FString> GetRequiredPermissions() const override;
};
