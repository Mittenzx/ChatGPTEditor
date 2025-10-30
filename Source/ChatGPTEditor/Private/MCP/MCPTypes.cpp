// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCP/MCPTypes.h"
#include "JsonUtilities.h"

FString FMCPResponse::ToJsonString() const
{
	TSharedPtr<FJsonObject> ResponseObj = MakeShared<FJsonObject>();
	ResponseObj->SetStringField(TEXT("jsonrpc"), JsonRpc);
	ResponseObj->SetNumberField(TEXT("id"), Id);
	
	if (Result.IsValid())
	{
		ResponseObj->SetObjectField(TEXT("result"), Result);
	}
	else if (Error.IsValid())
	{
		ResponseObj->SetObjectField(TEXT("error"), Error);
	}
	
	FString ResponseString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
	FJsonSerializer::Serialize(ResponseObj.ToSharedRef(), Writer);
	
	return ResponseString;
}
