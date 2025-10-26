// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExternalAPIHandler.h"
#include "AuditLogger.h"
#include "Json.h"
#include "JsonUtilities.h"

FExternalAPIHandler::FExternalAPIHandler()
{
}

FExternalAPIHandler::~FExternalAPIHandler()
{
}

bool FExternalAPIHandler::ParseAPIRequest(const FString& UserPrompt, FAPIRequestDetails& OutDetails)
{
	// Simple parsing logic - look for common patterns
	FString LowerPrompt = UserPrompt.ToLower();
	
	// Check if this is an API-related request
	if (!LowerPrompt.Contains(TEXT("api")) && 
	    !LowerPrompt.Contains(TEXT("http")) && 
	    !LowerPrompt.Contains(TEXT("rest")) &&
	    !LowerPrompt.Contains(TEXT("endpoint")) &&
	    !LowerPrompt.Contains(TEXT("web service")) &&
	    !LowerPrompt.Contains(TEXT("fetch")) &&
	    !LowerPrompt.Contains(TEXT("post")) &&
	    !LowerPrompt.Contains(TEXT("get")) &&
	    !LowerPrompt.Contains(TEXT("request")))
	{
		return false;
	}
	
	// Set description
	OutDetails.Description = UserPrompt;
	
	// Determine HTTP method
	if (LowerPrompt.Contains(TEXT("post")) || LowerPrompt.Contains(TEXT("send")) || LowerPrompt.Contains(TEXT("create")))
	{
		OutDetails.Method = TEXT("POST");
	}
	else if (LowerPrompt.Contains(TEXT("put")) || LowerPrompt.Contains(TEXT("update")))
	{
		OutDetails.Method = TEXT("PUT");
	}
	else if (LowerPrompt.Contains(TEXT("delete")) || LowerPrompt.Contains(TEXT("remove")))
	{
		OutDetails.Method = TEXT("DELETE");
	}
	else
	{
		OutDetails.Method = TEXT("GET");
	}
	
	// Try to extract URL from prompt
	// Look for http:// or https://
	int32 HttpIndex = UserPrompt.Find(TEXT("http://"), ESearchCase::IgnoreCase);
	int32 HttpsIndex = UserPrompt.Find(TEXT("https://"), ESearchCase::IgnoreCase);
	
	if (HttpIndex != INDEX_NONE || HttpsIndex != INDEX_NONE)
	{
		int32 StartIndex = (HttpsIndex != INDEX_NONE) ? HttpsIndex : HttpIndex;
		
		// Find end of URL (space, quote, or end of string)
		int32 EndIndex = UserPrompt.Len();
		for (int32 i = StartIndex; i < UserPrompt.Len(); i++)
		{
			TCHAR Ch = UserPrompt[i];
			if (Ch == ' ' || Ch == '\n' || Ch == '\r' || Ch == '"' || Ch == '\'' || Ch == ')')
			{
				EndIndex = i;
				break;
			}
		}
		
		OutDetails.Endpoint = UserPrompt.Mid(StartIndex, EndIndex - StartIndex);
	}
	else
	{
		// Provide a placeholder
		OutDetails.Endpoint = TEXT("https://api.example.com/endpoint");
	}
	
	// Set default headers
	OutDetails.Headers.Add(TEXT("Content-Type"), OutDetails.ContentType);
	OutDetails.Headers.Add(TEXT("Accept"), TEXT("application/json"));
	
	return true;
}

FString FExternalAPIHandler::GenerateAPIPreview(const FAPIRequestDetails& Details)
{
	FString Preview = TEXT("=== External API Request Preview ===\n\n");
	Preview += FString::Printf(TEXT("Description: %s\n\n"), *Details.Description);
	Preview += FString::Printf(TEXT("Endpoint: %s\n"), *Details.Endpoint);
	Preview += FString::Printf(TEXT("Method: %s\n"), *Details.Method);
	Preview += FString::Printf(TEXT("Content-Type: %s\n\n"), *Details.ContentType);
	
	if (Details.Headers.Num() > 0)
	{
		Preview += TEXT("Headers:\n");
		for (const auto& Header : Details.Headers)
		{
			Preview += FString::Printf(TEXT("  %s: %s\n"), *Header.Key, *Header.Value);
		}
		Preview += TEXT("\n");
	}
	
	if (!Details.Body.IsEmpty())
	{
		Preview += FString::Printf(TEXT("Request Body:\n%s\n\n"), *Details.Body);
	}
	
	Preview += TEXT("⚠️ WARNING: This will make an external HTTP request.\n");
	Preview += TEXT("Only approve if you trust this endpoint and understand what data will be sent.\n");
	
	return Preview;
}

void FExternalAPIHandler::ExecuteAPIRequest(const FAPIRequestDetails& Details, FOnAPIExecutionComplete Callback)
{
	// Validate request
	FString ErrorMessage;
	if (!ValidateAPIRequest(Details, ErrorMessage))
	{
		FAuditLogger::Get().LogAPIConnection(Details.Endpoint, Details.Method, false);
		Callback.ExecuteIfBound(false, FString::Printf(TEXT("Validation failed: %s"), *ErrorMessage));
		return;
	}
	
	// Log approved connection
	FAuditLogger::Get().LogAPIConnection(Details.Endpoint, Details.Method, true);
	
	// Create HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Details.Endpoint);
	HttpRequest->SetVerb(Details.Method);
	
	// Set headers
	for (const auto& Header : Details.Headers)
	{
		HttpRequest->SetHeader(Header.Key, Header.Value);
	}
	
	// Set body if provided
	if (!Details.Body.IsEmpty())
	{
		HttpRequest->SetContentAsString(Details.Body);
	}
	
	// Bind response callback using a lambda to capture the callback
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this, Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			OnHTTPResponseReceived(Request, Response, bWasSuccessful, Callback);
		}
	);
	
	// Execute request
	if (!HttpRequest->ProcessRequest())
	{
		Callback.ExecuteIfBound(false, TEXT("Failed to initiate HTTP request"));
	}
}

FString FExternalAPIHandler::GenerateIntegrationCode(const FAPIRequestDetails& Details)
{
	// Helper lambda to escape strings for C++ code generation
	auto EscapeForCpp = [](const FString& Input) -> FString
	{
		FString Result = Input;
		Result = Result.Replace(TEXT("\\"), TEXT("\\\\"));  // Escape backslashes first
		Result = Result.Replace(TEXT("\""), TEXT("\\\""));  // Escape quotes
		Result = Result.Replace(TEXT("\n"), TEXT("\\n"));   // Escape newlines
		Result = Result.Replace(TEXT("\r"), TEXT("\\r"));   // Escape carriage returns
		Result = Result.Replace(TEXT("\t"), TEXT("\\t"));   // Escape tabs
		return Result;
	};
	
	FString Code = TEXT("// Unreal Engine 5 HTTP Request Code\n");
	Code += TEXT("// Add this to your C++ class\n\n");
	Code += TEXT("#include \"HttpModule.h\"\n");
	Code += TEXT("#include \"Interfaces/IHttpRequest.h\"\n");
	Code += TEXT("#include \"Interfaces/IHttpResponse.h\"\n\n");
	
	Code += TEXT("void YourClass::MakeAPIRequest()\n");
	Code += TEXT("{\n");
	Code += TEXT("\t// Create HTTP request\n");
	Code += TEXT("\tTSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();\n");
	Code += FString::Printf(TEXT("\tHttpRequest->SetURL(TEXT(\"%s\"));\n"), *EscapeForCpp(Details.Endpoint));
	Code += FString::Printf(TEXT("\tHttpRequest->SetVerb(TEXT(\"%s\"));\n"), *EscapeForCpp(Details.Method));
	
	Code += TEXT("\t\n\t// Set headers\n");
	for (const auto& Header : Details.Headers)
	{
		Code += FString::Printf(TEXT("\tHttpRequest->SetHeader(TEXT(\"%s\"), TEXT(\"%s\"));\n"), 
			*EscapeForCpp(Header.Key), *EscapeForCpp(Header.Value));
	}
	
	if (!Details.Body.IsEmpty())
	{
		Code += TEXT("\t\n\t// Set request body\n");
		Code += FString::Printf(TEXT("\tHttpRequest->SetContentAsString(TEXT(\"%s\"));\n"), *EscapeForCpp(Details.Body));
	}
	
	Code += TEXT("\t\n\t// Set response callback\n");
	Code += TEXT("\tHttpRequest->OnProcessRequestComplete().BindUObject(\n");
	Code += TEXT("\t\tthis, &YourClass::OnResponseReceived);\n");
	Code += TEXT("\t\n\t// Send request\n");
	Code += TEXT("\tHttpRequest->ProcessRequest();\n");
	Code += TEXT("}\n\n");
	
	Code += TEXT("void YourClass::OnResponseReceived(\n");
	Code += TEXT("\tFHttpRequestPtr Request,\n");
	Code += TEXT("\tFHttpResponsePtr Response,\n");
	Code += TEXT("\tbool bWasSuccessful)\n");
	Code += TEXT("{\n");
	Code += TEXT("\tif (bWasSuccessful && Response.IsValid())\n");
	Code += TEXT("\t{\n");
	Code += TEXT("\t\tFString ResponseContent = Response->GetContentAsString();\n");
	Code += TEXT("\t\tUE_LOG(LogTemp, Log, TEXT(\"Response: %s\"), *ResponseContent);\n");
	Code += TEXT("\t\t// Handle response here\n");
	Code += TEXT("\t}\n");
	Code += TEXT("\telse\n");
	Code += TEXT("\t{\n");
	Code += TEXT("\t\tUE_LOG(LogTemp, Error, TEXT(\"HTTP request failed\"));\n");
	Code += TEXT("\t}\n");
	Code += TEXT("}\n");
	
	return Code;
}

void FExternalAPIHandler::OnHTTPResponseReceived(
	FHttpRequestPtr Request, 
	FHttpResponsePtr Response, 
	bool bWasSuccessful,
	FOnAPIExecutionComplete Callback)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		Callback.ExecuteIfBound(false, TEXT("HTTP request failed - no response received"));
		return;
	}
	
	int32 ResponseCode = Response->GetResponseCode();
	FString ResponseContent = Response->GetContentAsString();
	
	if (ResponseCode >= 200 && ResponseCode < 300)
	{
		FString SuccessMessage = FString::Printf(
			TEXT("Success (HTTP %d):\n%s"),
			ResponseCode,
			*ResponseContent
		);
		Callback.ExecuteIfBound(true, SuccessMessage);
	}
	else
	{
		FString ErrorMessage = FString::Printf(
			TEXT("HTTP Error %d:\n%s"),
			ResponseCode,
			*ResponseContent
		);
		Callback.ExecuteIfBound(false, ErrorMessage);
	}
}

bool FExternalAPIHandler::ValidateAPIRequest(const FAPIRequestDetails& Details, FString& OutErrorMessage)
{
	// Check endpoint is not empty
	if (Details.Endpoint.IsEmpty())
	{
		OutErrorMessage = TEXT("Endpoint URL is empty");
		return false;
	}
	
	// Check endpoint starts with http:// or https://
	if (!Details.Endpoint.StartsWith(TEXT("http://")) && !Details.Endpoint.StartsWith(TEXT("https://")))
	{
		OutErrorMessage = TEXT("Endpoint must start with http:// or https://");
		return false;
	}
	
	// Check method is valid
	if (Details.Method != TEXT("GET") && 
	    Details.Method != TEXT("POST") && 
	    Details.Method != TEXT("PUT") && 
	    Details.Method != TEXT("DELETE") &&
	    Details.Method != TEXT("PATCH"))
	{
		OutErrorMessage = FString::Printf(TEXT("Invalid HTTP method: %s"), *Details.Method);
		return false;
	}
	
	return true;
}
