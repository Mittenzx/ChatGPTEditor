// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SceneEditingTypes.h"

/**
 * Comprehensive audit logger for tracking all ChatGPT Editor operations
 * Logs API connections, code changes, file operations, permission changes,
 * scene editing operations, and general events
 * Logs are stored in Saved/ChatGPTEditor/audit.log
 */
class FAuditLogger
{
public:
	/** Get the singleton instance */
	static FAuditLogger& Get();
	
	/** Initialize the audit logger */
	void Initialize();
	
	/** Shutdown the audit logger */
	void Shutdown();
	
	/** Log an API connection attempt */
	void LogAPIConnection(const FString& Endpoint, const FString& Method, bool bApproved);
	
	/** Log a code change */
	void LogCodeChange(const FString& Description, const FString& CodePreview, bool bApproved);
	
	/** Log a general event */
	void LogEvent(const FString& EventType, const FString& Message);
	
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
	
	/** Log a scene editing operation */
	void LogOperation(const FString& UserCommand, const FString& OperationType, 
		const FString& AffectedActors, bool bSuccess, const FString& ErrorMessage = TEXT(""));

	/** Get all audit log entries */
	TArray<FAuditLogEntry> GetLogEntries() const;

	/** Get recent log entries (last N entries) */
	TArray<FAuditLogEntry> GetRecentEntries(int32 Count) const;

	/** Clear all log entries */
	void ClearLog();

	/** Export log to string */
	FString ExportLogToString() const;
	
private:
	FAuditLogger();
	~FAuditLogger() = default;
	
	// Prevent copying
	FAuditLogger(const FAuditLogger&) = delete;
	FAuditLogger& operator=(const FAuditLogger&) = delete;
	
	void EnsureLogDirectoryExists();
	FString GetAuditLogPath() const;
	void WriteLog(const FString& Entry);
	void WriteLogEntry(const FString& Entry);
	FString GetTimestamp() const;

	FCriticalSection LogMutex;
	TArray<FAuditLogEntry> LogEntries;
	bool bInitialized;
};
