// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCP/MCPTool.h"

FMCPToolBase::FMCPToolBase(const FString& InName, const FString& InDescription)
	: Name(InName)
	, Description(InDescription)
{
}

TSharedPtr<FJsonObject> FMCPToolBase::CreateSuccessResponse(const FString& Message) const
{
	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	
	// Create content array with text content
	TArray<TSharedPtr<FJsonValue>> ContentArray;
	TSharedPtr<FJsonObject> TextContent = MakeShared<FJsonObject>();
	TextContent->SetStringField(TEXT("type"), TEXT("text"));
	TextContent->SetStringField(TEXT("text"), Message);
	ContentArray.Add(MakeShared<FJsonValueObject>(TextContent));
	
	Result->SetArrayField(TEXT("content"), ContentArray);
	
	return Result;
}

TSharedPtr<FJsonObject> FMCPToolBase::CreateErrorResponse(const FString& ErrorMessage) const
{
	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), false);
	Result->SetStringField(TEXT("error"), ErrorMessage);
	
	// Also include in content array for MCP compatibility
	TArray<TSharedPtr<FJsonValue>> ContentArray;
	TSharedPtr<FJsonObject> TextContent = MakeShared<FJsonObject>();
	TextContent->SetStringField(TEXT("type"), TEXT("text"));
	TextContent->SetStringField(TEXT("text"), FString::Printf(TEXT("Error: %s"), *ErrorMessage));
	ContentArray.Add(MakeShared<FJsonValueObject>(TextContent));
	
	Result->SetArrayField(TEXT("content"), ContentArray);
	
	return Result;
}

TSharedPtr<FJsonObject> FMCPToolBase::CreateTextContent(const FString& Text) const
{
	TSharedPtr<FJsonObject> Content = MakeShared<FJsonObject>();
	Content->SetStringField(TEXT("type"), TEXT("text"));
	Content->SetStringField(TEXT("text"), Text);
	return Content;
}
