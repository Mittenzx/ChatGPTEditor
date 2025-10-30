// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "MCPTool.h"

/**
 * Main MCP Server for Unreal Engine
 * Implements JSON-RPC 2.0 protocol for Model Context Protocol
 */
class CHATGPTEDITOR_API FMCPServer
{
public:
	FMCPServer();
	~FMCPServer();
	
	// Server lifecycle
	bool Initialize();
	void Shutdown();
	bool IsInitialized() const { return bIsInitialized; }
	
	// Tool registration
	void RegisterTool(TSharedPtr<IMCPTool> Tool);
	void UnregisterTool(const FString& ToolName);
	TArray<TSharedPtr<IMCPTool>> GetRegisteredTools() const;
	
	// Message processing
	FString ProcessMessage(const FString& JsonMessage);
	
protected:
	// Protocol handlers
	TSharedPtr<FJsonObject> HandleInitialize(int32 Id, const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleToolsList(int32 Id);
	TSharedPtr<FJsonObject> HandleToolsCall(int32 Id, const TSharedPtr<FJsonObject>& Params);
	
	// Response builders
	FString CreateSuccessResponse(int32 Id, const TSharedPtr<FJsonObject>& Result) const;
	FString CreateErrorResponse(int32 Id, int32 Code, const FString& Message) const;
	
	// Request parsing
	bool ParseRequest(const FString& JsonMessage, FString& OutMethod, int32& OutId, TSharedPtr<FJsonObject>& OutParams);
	
private:
	// Registered tools
	TMap<FString, TSharedPtr<IMCPTool>> RegisteredTools;
	
	// Server state
	FString ProtocolVersion;
	bool bIsInitialized;
	
	// Configuration
	TSharedPtr<FJsonObject> ServerCapabilities;
	
	// Thread safety
	FCriticalSection RegistrationLock;
	
	// Statistics
	int32 RequestsProcessed;
	int32 ErrorsEncountered;
};
