// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SceneEditingTypes.h"

/**
 * Audit logger for tracking all scene editing operations
 * Maintains a persistent log of all actions with timestamps
 */
class FAuditLogger
{
public:
	/** Get singleton instance */
	static FAuditLogger& Get();

	/** Log a scene editing operation */
	void LogOperation(const FString& UserCommand, const FString& OperationType, 
		const FString& AffectedActors, bool bSuccess, const FString& ErrorMessage = TEXT(""));

	/** Get all audit log entries */
	const TArray<FAuditLogEntry>& GetLogEntries() const { return LogEntries; }

	/** Get recent log entries (last N entries) */
	TArray<FAuditLogEntry> GetRecentEntries(int32 Count) const;

	/** Clear all log entries */
	void ClearLog();

	/** Export log to string */
	FString ExportLogToString() const;

private:
	FAuditLogger() = default;
	
	TArray<FAuditLogEntry> LogEntries;
	FCriticalSection LogMutex;
};
