// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Handles console command execution with safety checks and user confirmation
 * Provides natural language command parsing and execution with audit logging
 */
class FChatGPTConsoleHandler
{
public:
	FChatGPTConsoleHandler();
	~FChatGPTConsoleHandler();
	
	/**
	 * Execute a console command with safety checks and user confirmation
	 * @param Command The command to execute
	 * @param bRequireConfirmation Whether to require user confirmation
	 * @return True if command was executed successfully
	 */
	bool ExecuteCommand(const FString& Command, bool bRequireConfirmation = true);
	
	/**
	 * Check if a command is safe to execute without confirmation
	 * @param Command The command to check
	 * @return True if the command is considered safe
	 */
	bool IsCommandSafe(const FString& Command) const;
	
	/**
	 * Check if a command is blacklisted (forbidden)
	 * @param Command The command to check
	 * @return True if the command is blacklisted
	 */
	bool IsCommandBlacklisted(const FString& Command) const;
	
	/**
	 * Parse natural language input to extract console command
	 * @param NaturalLanguageInput The natural language description
	 * @param OutCommand The extracted command (if found)
	 * @return True if a command was successfully extracted
	 */
	bool ParseNaturalLanguageCommand(const FString& NaturalLanguageInput, FString& OutCommand) const;
	
	/**
	 * Log command execution to audit log
	 * @param Command The command that was executed
	 * @param bSuccess Whether execution was successful
	 * @param ErrorMessage Optional error message
	 */
	void LogCommandExecution(const FString& Command, bool bSuccess, const FString& ErrorMessage = TEXT(""));

private:
	// Safe commands that don't require confirmation (read-only operations)
	TArray<FString> SafeCommands;
	
	// Blacklisted commands that are never allowed
	TArray<FString> BlacklistedCommands;
	
	// Destructive command patterns that always require confirmation
	TArray<FString> DestructivePatterns;
	
	void InitializeCommandLists();
};
