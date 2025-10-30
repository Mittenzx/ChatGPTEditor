// Copyright Epic Games, Inc. All Rights Reserved.

#include "EchoTool.h"

FEchoTool::FEchoTool()
	: FMCPToolBase(TEXT("echo"), TEXT("Echo back the input message"))
{
}

TSharedPtr<FJsonObject> FEchoTool::GetInputSchema() const
{
	TSharedPtr<FJsonObject> Schema = MakeShared<FJsonObject>();
	Schema->SetStringField(TEXT("type"), TEXT("object"));
	
	TSharedPtr<FJsonObject> Properties = MakeShared<FJsonObject>();
	
	// message property
	TSharedPtr<FJsonObject> MessageProp = MakeShared<FJsonObject>();
	MessageProp->SetStringField(TEXT("type"), TEXT("string"));
	MessageProp->SetStringField(TEXT("description"), TEXT("Message to echo back"));
	Properties->SetObjectField(TEXT("message"), MessageProp);
	
	Schema->SetObjectField(TEXT("properties"), Properties);
	
	// Required fields
	TArray<TSharedPtr<FJsonValue>> Required;
	Required.Add(MakeShared<FJsonValueString>(TEXT("message")));
	Schema->SetArrayField(TEXT("required"), Required);
	
	return Schema;
}

TSharedPtr<FJsonObject> FEchoTool::Execute(const TSharedPtr<FJsonObject>& Arguments)
{
	if (!Arguments.IsValid() || !Arguments->HasField(TEXT("message")))
	{
		return CreateErrorResponse(TEXT("Missing required parameter: message"));
	}
	
	FString Message = Arguments->GetStringField(TEXT("message"));
	return CreateSuccessResponse(FString::Printf(TEXT("Echo: %s"), *Message));
}
