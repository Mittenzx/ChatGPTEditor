// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Json.h"

/**
 * Shared types and constants for MCP (Model Context Protocol) implementation
 */

namespace MCPProtocol
{
	// Protocol version
	static const FString Version = TEXT("2025-03-26");
	
	// JSON-RPC error codes
	static constexpr int32 ParseError = -32700;
	static constexpr int32 InvalidRequest = -32600;
	static constexpr int32 MethodNotFound = -32601;
	static constexpr int32 InvalidParams = -32602;
	static constexpr int32 InternalError = -32603;
	
	// MCP methods
	static const FString Method_Initialize = TEXT("initialize");
	static const FString Method_ToolsList = TEXT("tools/list");
	static const FString Method_ToolsCall = TEXT("tools/call");
	static const FString Method_ResourcesList = TEXT("resources/list");
	static const FString Method_ResourcesRead = TEXT("resources/read");
	static const FString Method_PromptsList = TEXT("prompts/list");
}

/**
 * JSON-RPC request structure
 */
struct FMCPRequest
{
	FString JsonRpc;
	int32 Id;
	FString Method;
	TSharedPtr<FJsonObject> Params;
	
	FMCPRequest()
		: JsonRpc(TEXT("2.0"))
		, Id(0)
	{}
};

/**
 * JSON-RPC response structure
 */
struct FMCPResponse
{
	FString JsonRpc;
	int32 Id;
	TSharedPtr<FJsonObject> Result;
	TSharedPtr<FJsonObject> Error;
	
	FMCPResponse()
		: JsonRpc(TEXT("2.0"))
		, Id(0)
	{}
	
	FString ToJsonString() const;
};

/**
 * MCP tool metadata
 */
struct FMCPToolInfo
{
	FString Name;
	FString Description;
	TSharedPtr<FJsonObject> InputSchema;
};

/**
 * MCP resource metadata
 */
struct FMCPResourceInfo
{
	FString Uri;
	FString Name;
	FString Description;
	FString MimeType;
};
