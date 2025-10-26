// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Audit logging system for tracking all file operations and permission changes
 * Logs are stored in Saved/ChatGPTEditor/audit.log
 */
class FAuditLogger
{
public:
	/** Get singleton instance */
	static FAuditLogger& Get();

	/** Log a file read operation */
	void LogFileRead(const FString& FilePath);

	/** Log a file write operation */
	void LogFileWrite(const FString& FilePath, const FString& Operation);

	/** Log a permission change */
	void LogPermissionChange(const FString& PermissionName, bool bEnabled);

	/** Log a general operation */
	void LogOperation(const FString& Category, const FString& Message);

	/** Log an error */
	void LogError(const FString& Category, const FString& ErrorMessage);

private:
	/** Private constructor for singleton */
	FAuditLogger();

	/** Ensure log directory exists */
	void EnsureLogDirectoryExists();

	/** Get the full path to the audit log file */
	FString GetAuditLogPath() const;

	/** Write a log entry with timestamp */
	void WriteLogEntry(const FString& Entry);

	/** Mutex for thread-safe logging */
	FCriticalSection LogMutex;
};
