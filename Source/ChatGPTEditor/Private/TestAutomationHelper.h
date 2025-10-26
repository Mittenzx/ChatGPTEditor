// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Helper class for test automation and audit logging functionality
 * Handles test generation, execution, and logging of all operations
 */
class FTestAutomationHelper
{
public:
	/**
	 * Initialize the audit log file
	 * Creates the log directory if it doesn't exist
	 */
	static void InitializeAuditLog();
	
	/**
	 * Log a test generation request to the audit log
	 * @param UserPrompt The natural language prompt for test generation
	 * @param TestType Type of test requested (Unit, Integration, Functional)
	 */
	static void LogTestGenerationRequest(const FString& UserPrompt, const FString& TestType);
	
	/**
	 * Log a test execution request to the audit log
	 * @param TestName Name of the test being executed
	 * @param TestPath Path to the test file
	 */
	static void LogTestExecutionRequest(const FString& TestName, const FString& TestPath);
	
	/**
	 * Log test execution results to the audit log
	 * @param TestName Name of the test
	 * @param bSuccess Whether the test passed
	 * @param ResultMessage Result message or error details
	 */
	static void LogTestExecutionResult(const FString& TestName, bool bSuccess, const FString& ResultMessage);
	
	/**
	 * Log a general message to the audit log
	 * @param Category Category/tag for the log entry
	 * @param Message The message to log
	 */
	static void LogAuditMessage(const FString& Category, const FString& Message);
	
	/**
	 * Get the path to the audit log file
	 * @return Full path to audit.log
	 */
	static FString GetAuditLogPath();

	/**
	 * Parse test generation response from ChatGPT
	 * Extracts code blocks and test metadata
	 * @param Response The full response from ChatGPT
	 * @param OutTestCode The extracted test code
	 * @param OutTestName The extracted test name (if any)
	 * @return True if valid test code was found
	 */
	static bool ParseTestCodeFromResponse(const FString& Response, FString& OutTestCode, FString& OutTestName);

	/**
	 * Validate test code before execution
	 * Checks for potentially dangerous operations
	 * @param TestCode The test code to validate
	 * @param OutWarnings Any security warnings found
	 * @return True if code passes basic security checks
	 */
	static bool ValidateTestCode(const FString& TestCode, TArray<FString>& OutWarnings);

private:
	static FString AuditLogPath;
	static FCriticalSection LogMutex;
	
	/**
	 * Write a timestamped entry to the audit log
	 * Thread-safe implementation
	 */
	static void WriteToAuditLog(const FString& Message);
	
	/**
	 * Get current timestamp in ISO 8601 format
	 */
	static FString GetTimestamp();
};
