// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Handles Python script generation and execution with safety checks and user confirmation
 * Provides script preview, validation, and execution with audit logging
 */
class FChatGPTPythonHandler
{
public:
	FChatGPTPythonHandler();
	~FChatGPTPythonHandler();
	
	/**
	 * Execute a Python script with safety checks and user confirmation
	 * @param Script The Python script to execute
	 * @param bRequireConfirmation Whether to require user confirmation
	 * @return True if script was executed successfully
	 */
	bool ExecuteScript(const FString& Script, bool bRequireConfirmation = true);
	
	/**
	 * Preview a Python script before execution
	 * @param Script The script to preview
	 * @param OutPreview The formatted preview text
	 */
	void PreviewScript(const FString& Script, FString& OutPreview) const;
	
	/**
	 * Check if Python scripting is available in the editor
	 * @return True if Python scripting is enabled and available
	 */
	bool IsPythonAvailable() const;
	
	/**
	 * Validate a Python script for security concerns
	 * @param Script The script to validate
	 * @param OutWarnings Any warnings or security concerns found
	 * @return True if the script passes basic validation
	 */
	bool ValidateScript(const FString& Script, TArray<FString>& OutWarnings) const;
	
	/**
	 * Parse natural language input to check if it's a Python script request
	 * @param NaturalLanguageInput The natural language description
	 * @return True if this appears to be a Python scripting request
	 */
	bool IsPythonScriptRequest(const FString& NaturalLanguageInput) const;
	
	/**
	 * Log script execution to audit log
	 * @param Script The script that was executed
	 * @param bSuccess Whether execution was successful
	 * @param ErrorMessage Optional error message
	 */
	void LogScriptExecution(const FString& Script, bool bSuccess, const FString& ErrorMessage = TEXT(""));

private:
	// Dangerous Python operations that require extra warnings
	TArray<FString> DangerousOperations;
	
	// Forbidden Python operations that should reject scripts
	TArray<FString> ForbiddenOperations;
	
	void InitializeDangerousOperations();
	FString SanitizeScriptForPreview(const FString& Script) const;
};
