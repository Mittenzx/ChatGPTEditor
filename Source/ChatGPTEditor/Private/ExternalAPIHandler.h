// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

DECLARE_DELEGATE_OneParam(FOnAPIPreviewResponse, const FString& /* PreviewText */);
DECLARE_DELEGATE_TwoParams(FOnAPIExecutionComplete, bool /* bSuccess */, const FString& /* Response */);

/**
 * Struct containing API request details for preview and execution
 */
struct FAPIRequestDetails
{
	FString Endpoint;
	FString Method;
	FString ContentType;
	TMap<FString, FString> Headers;
	FString Body;
	FString Description;
	
	FAPIRequestDetails()
		: Method(TEXT("GET"))
		, ContentType(TEXT("application/json"))
	{}
};

/**
 * Handles external API and web integration requests
 * Provides preview, approval, and audit logging for all external connections
 */
class FExternalAPIHandler
{
public:
	FExternalAPIHandler();
	~FExternalAPIHandler();
	
	/**
	 * Parse a natural language request into API request details
	 * Returns true if parsing was successful
	 */
	bool ParseAPIRequest(const FString& UserPrompt, FAPIRequestDetails& OutDetails);
	
	/**
	 * Generate a preview of the API request for user approval
	 */
	FString GenerateAPIPreview(const FAPIRequestDetails& Details);
	
	/**
	 * Execute an approved API request
	 */
	void ExecuteAPIRequest(const FAPIRequestDetails& Details, FOnAPIExecutionComplete Callback);
	
	/**
	 * Generate code for integrating an API call into UE5
	 */
	FString GenerateIntegrationCode(const FAPIRequestDetails& Details);
	
private:
	/** Handle HTTP response */
	void OnHTTPResponseReceived(
		FHttpRequestPtr Request, 
		FHttpResponsePtr Response, 
		bool bWasSuccessful,
		FOnAPIExecutionComplete Callback
	);
	
	/** Extract API details from ChatGPT response */
	bool ExtractAPIDetailsFromResponse(const FString& Response, FAPIRequestDetails& OutDetails);
	
	/** Validate API request for security */
	bool ValidateAPIRequest(const FAPIRequestDetails& Details, FString& OutErrorMessage);
};
