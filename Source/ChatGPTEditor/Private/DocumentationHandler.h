// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuditLog.h"

/**
 * Structure to represent a documentation change request
 */
struct FDocumentationChange
{
	FString FilePath;
	FString OriginalContent;
	FString ProposedContent;
	FString Description;
	bool bIsNewFile;

	FDocumentationChange()
		: bIsNewFile(false)
	{
	}
};

/**
 * Handler for documentation generation and code review operations
 */
class FDocumentationHandler
{
public:
	/**
	 * Parse ChatGPT response for documentation operations
	 * Returns true if the response contains a documentation operation
	 */
	static bool ParseDocumentationRequest(const FString& UserMessage, const FString& AssistantResponse, FDocumentationChange& OutChange);

	/**
	 * Preview documentation changes
	 */
	static FString PreviewChange(const FDocumentationChange& Change);

	/**
	 * Apply documentation changes to file
	 * Returns true if successful
	 */
	static bool ApplyChange(const FDocumentationChange& Change, FString& OutError);

	/**
	 * Check if a message is a documentation/code review request
	 */
	static bool IsDocumentationRequest(const FString& UserMessage);

	/**
	 * Check if a message is a code explanation request
	 */
	static bool IsCodeExplanationRequest(const FString& UserMessage);

	/**
	 * Read file content safely
	 */
	static bool ReadFile(const FString& FilePath, FString& OutContent, FString& OutError);

	/**
	 * Write file content safely
	 */
	static bool WriteFile(const FString& FilePath, const FString& Content, FString& OutError);

	/**
	 * Get plugin directory path
	 */
	static FString GetPluginDirectory();

	/**
	 * Check if file path is safe (within plugin directory)
	 */
	static bool IsSafeFilePath(const FString& FilePath);
};
