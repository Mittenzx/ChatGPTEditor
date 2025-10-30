// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Json.h"

/**
 * Interface for MCP tools
 * Each tool represents an action that can be performed in Unreal Engine
 */
class CHATGPTEDITOR_API IMCPTool
{
public:
	virtual ~IMCPTool() = default;
	
	// Tool metadata
	virtual FString GetName() const = 0;
	virtual FString GetDescription() const = 0;
	virtual TSharedPtr<FJsonObject> GetInputSchema() const = 0;
	
	// Execution
	virtual TSharedPtr<FJsonObject> Execute(const TSharedPtr<FJsonObject>& Arguments) = 0;
	
	// Security
	virtual bool RequiresConfirmation() const { return false; }
	virtual bool IsDangerous() const { return false; }
	virtual TArray<FString> GetRequiredPermissions() const { return TArray<FString>(); }
};

/**
 * Base class for MCP tools with common functionality
 */
class CHATGPTEDITOR_API FMCPToolBase : public IMCPTool
{
public:
	FMCPToolBase(const FString& InName, const FString& InDescription);
	virtual ~FMCPToolBase() = default;
	
	virtual FString GetName() const override { return Name; }
	virtual FString GetDescription() const override { return Description; }
	
protected:
	// Helper functions for creating responses
	TSharedPtr<FJsonObject> CreateSuccessResponse(const FString& Message) const;
	TSharedPtr<FJsonObject> CreateErrorResponse(const FString& ErrorMessage) const;
	TSharedPtr<FJsonObject> CreateTextContent(const FString& Text) const;
	
	FString Name;
	FString Description;
};
