// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Audit logger for tracking API connections and code changes
 * Logs all external API calls and code modifications for security auditing
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
	
private:
	FAuditLogger() = default;
	~FAuditLogger() = default;
	
	// Prevent copying
	FAuditLogger(const FAuditLogger&) = delete;
	FAuditLogger& operator=(const FAuditLogger&) = delete;
	
	/** Get the audit log file path */
	FString GetAuditLogPath() const;
	
	/** Write a log entry */
	void WriteLog(const FString& Entry);
	
	/** Get current timestamp */
	FString GetTimestamp() const;
	
	/** Ensure log directory exists */
	void EnsureLogDirectoryExists();
	
	FCriticalSection LogMutex;
	bool bInitialized = false;
};
