// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCP/MCPServer.h"
#include "MCP/MCPTypes.h"
#include "JsonUtilities.h"

FMCPServer::FMCPServer()
	: ProtocolVersion(MCPProtocol::Version)
	, bIsInitialized(false)
	, RequestsProcessed(0)
	, ErrorsEncountered(0)
{
	ServerCapabilities = MakeShared<FJsonObject>();
	
	// Set up capabilities
	TSharedPtr<FJsonObject> ToolsCap = MakeShared<FJsonObject>();
	ToolsCap->SetBoolField(TEXT("listChanged"), true);
	ServerCapabilities->SetObjectField(TEXT("tools"), ToolsCap);
}

FMCPServer::~FMCPServer()
{
	Shutdown();
}

bool FMCPServer::Initialize()
{
	if (bIsInitialized)
	{
		return true;
	}
	
	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("MCP Server initialized"));
	
	return true;
}

void FMCPServer::Shutdown()
{
	if (!bIsInitialized)
	{
		return;
	}
	
	FScopeLock Lock(&RegistrationLock);
	RegisteredTools.Empty();
	bIsInitialized = false;
	
	UE_LOG(LogTemp, Log, TEXT("MCP Server shutdown"));
}

void FMCPServer::RegisterTool(TSharedPtr<IMCPTool> Tool)
{
	if (!Tool.IsValid())
	{
		return;
	}
	
	FScopeLock Lock(&RegistrationLock);
	FString ToolName = Tool->GetName();
	RegisteredTools.Add(ToolName, Tool);
	
	UE_LOG(LogTemp, Log, TEXT("MCP Tool registered: %s"), *ToolName);
}

void FMCPServer::UnregisterTool(const FString& ToolName)
{
	FScopeLock Lock(&RegistrationLock);
	RegisteredTools.Remove(ToolName);
	
	UE_LOG(LogTemp, Log, TEXT("MCP Tool unregistered: %s"), *ToolName);
}

TArray<TSharedPtr<IMCPTool>> FMCPServer::GetRegisteredTools() const
{
	FScopeLock Lock(&RegistrationLock);
	
	TArray<TSharedPtr<IMCPTool>> Tools;
	RegisteredTools.GenerateValueArray(Tools);
	return Tools;
}

FString FMCPServer::ProcessMessage(const FString& JsonMessage)
{
	RequestsProcessed++;
	
	// Parse request
	FString Method;
	int32 Id;
	TSharedPtr<FJsonObject> Params;
	
	if (!ParseRequest(JsonMessage, Method, Id, Params))
	{
		ErrorsEncountered++;
		return CreateErrorResponse(0, MCPProtocol::ParseError, TEXT("Failed to parse JSON-RPC request"));
	}
	
	// Handle method
	TSharedPtr<FJsonObject> Result;
	
	if (Method == MCPProtocol::Method_Initialize)
	{
		Result = HandleInitialize(Id, Params);
	}
	else if (Method == MCPProtocol::Method_ToolsList)
	{
		Result = HandleToolsList(Id);
	}
	else if (Method == MCPProtocol::Method_ToolsCall)
	{
		Result = HandleToolsCall(Id, Params);
	}
	else
	{
		ErrorsEncountered++;
		return CreateErrorResponse(Id, MCPProtocol::MethodNotFound, 
			FString::Printf(TEXT("Method not found: %s"), *Method));
	}
	
	if (!Result.IsValid())
	{
		ErrorsEncountered++;
		return CreateErrorResponse(Id, MCPProtocol::InternalError, TEXT("Internal server error"));
	}
	
	return CreateSuccessResponse(Id, Result);
}

bool FMCPServer::ParseRequest(const FString& JsonMessage, FString& OutMethod, int32& OutId, TSharedPtr<FJsonObject>& OutParams)
{
	TSharedPtr<FJsonObject> Request;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonMessage);
	
	if (!FJsonSerializer::Deserialize(Reader, Request) || !Request.IsValid())
	{
		return false;
	}
	
	// Validate JSON-RPC 2.0
	if (!Request->HasField(TEXT("jsonrpc")) || Request->GetStringField(TEXT("jsonrpc")) != TEXT("2.0"))
	{
		return false;
	}
	
	// Extract method
	if (!Request->HasField(TEXT("method")))
	{
		return false;
	}
	OutMethod = Request->GetStringField(TEXT("method"));
	
	// Extract id
	OutId = Request->HasField(TEXT("id")) ? static_cast<int32>(Request->GetNumberField(TEXT("id"))) : 0;
	
	// Extract params (optional)
	if (Request->HasField(TEXT("params")))
	{
		OutParams = Request->GetObjectField(TEXT("params"));
	}
	
	return true;
}

TSharedPtr<FJsonObject> FMCPServer::HandleInitialize(int32 Id, const TSharedPtr<FJsonObject>& Params)
{
	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("protocolVersion"), ProtocolVersion);
	Result->SetObjectField(TEXT("capabilities"), ServerCapabilities);
	
	// Server info
	TSharedPtr<FJsonObject> ServerInfo = MakeShared<FJsonObject>();
	ServerInfo->SetStringField(TEXT("name"), TEXT("ChatGPTEditor-MCP"));
	ServerInfo->SetStringField(TEXT("version"), TEXT("1.0.0"));
	Result->SetObjectField(TEXT("serverInfo"), ServerInfo);
	
	bIsInitialized = true;
	
	return Result;
}

TSharedPtr<FJsonObject> FMCPServer::HandleToolsList(int32 Id)
{
	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> ToolsArray;
	
	FScopeLock Lock(&RegistrationLock);
	
	for (const auto& ToolPair : RegisteredTools)
	{
		TSharedPtr<IMCPTool> Tool = ToolPair.Value;
		
		TSharedPtr<FJsonObject> ToolObj = MakeShared<FJsonObject>();
		ToolObj->SetStringField(TEXT("name"), Tool->GetName());
		ToolObj->SetStringField(TEXT("description"), Tool->GetDescription());
		ToolObj->SetObjectField(TEXT("inputSchema"), Tool->GetInputSchema());
		
		ToolsArray.Add(MakeShared<FJsonValueObject>(ToolObj));
	}
	
	Result->SetArrayField(TEXT("tools"), ToolsArray);
	
	return Result;
}

TSharedPtr<FJsonObject> FMCPServer::HandleToolsCall(int32 Id, const TSharedPtr<FJsonObject>& Params)
{
	if (!Params.IsValid() || !Params->HasField(TEXT("name")))
	{
		return nullptr;
	}
	
	FString ToolName = Params->GetStringField(TEXT("name"));
	TSharedPtr<FJsonObject> Arguments = Params->HasField(TEXT("arguments")) 
		? Params->GetObjectField(TEXT("arguments"))
		: MakeShared<FJsonObject>();
	
	// Find tool
	FScopeLock Lock(&RegistrationLock);
	TSharedPtr<IMCPTool>* ToolPtr = RegisteredTools.Find(ToolName);
	
	if (!ToolPtr || !ToolPtr->IsValid())
	{
		TSharedPtr<FJsonObject> ErrorResult = MakeShared<FJsonObject>();
		ErrorResult->SetStringField(TEXT("error"), FString::Printf(TEXT("Tool not found: %s"), *ToolName));
		return ErrorResult;
	}
	
	// Execute tool
	TSharedPtr<IMCPTool> Tool = *ToolPtr;
	TSharedPtr<FJsonObject> Result = Tool->Execute(Arguments);
	
	return Result;
}

FString FMCPServer::CreateSuccessResponse(int32 Id, const TSharedPtr<FJsonObject>& Result) const
{
	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("jsonrpc"), TEXT("2.0"));
	Response->SetNumberField(TEXT("id"), Id);
	Response->SetObjectField(TEXT("result"), Result);
	
	FString ResponseString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
	FJsonSerializer::Serialize(Response.ToSharedRef(), Writer);
	
	return ResponseString;
}

FString FMCPServer::CreateErrorResponse(int32 Id, int32 Code, const FString& Message) const
{
	TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetStringField(TEXT("jsonrpc"), TEXT("2.0"));
	Response->SetNumberField(TEXT("id"), Id);
	
	TSharedPtr<FJsonObject> Error = MakeShared<FJsonObject>();
	Error->SetNumberField(TEXT("code"), Code);
	Error->SetStringField(TEXT("message"), Message);
	Response->SetObjectField(TEXT("error"), Error);
	
	FString ResponseString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
	FJsonSerializer::Serialize(Response.ToSharedRef(), Writer);
	
	return ResponseString;
}
