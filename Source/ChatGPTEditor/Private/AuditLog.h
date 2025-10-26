// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Audit log entry for tracking documentation and code review operations
 */
struct FAuditLogEntry
{
	FDateTime Timestamp;
	FString Operation;
	FString Details;
	FString FilePath;
	bool bSuccess;

	FAuditLogEntry()
		: Timestamp(FDateTime::Now())
		, bSuccess(false)
	{
	}

	FAuditLogEntry(const FString& InOperation, const FString& InDetails, const FString& InFilePath = TEXT(""), bool bInSuccess = true)
		: Timestamp(FDateTime::Now())
		, Operation(InOperation)
		, Details(InDetails)
		, FilePath(InFilePath)
		, bSuccess(bInSuccess)
	{
	}

	FString ToString() const
	{
		FString FileInfo = FilePath.IsEmpty() ? TEXT("") : FString::Printf(TEXT(" (File: %s)"), *FilePath);
		FString DetailInfo = Details.IsEmpty() ? TEXT("") : FString::Printf(TEXT(" - %s"), *Details);
		
		return FString::Printf(TEXT("[%s] %s: %s%s%s"),
			*Timestamp.ToString(TEXT("%Y-%m-%d %H:%M:%S")),
			bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"),
			*Operation,
			*FileInfo,
			*DetailInfo
		);
	}
};

/**
 * Audit log manager for tracking all documentation and code review operations
 */
class FAuditLog
{
public:
	static FAuditLog& Get()
	{
		static FAuditLog Instance;
		return Instance;
	}

	void LogEntry(const FAuditLogEntry& Entry)
	{
		FScopeLock Lock(&CriticalSection);
		Entries.Add(Entry);
		
		// Keep only last 1000 entries to avoid memory issues
		if (Entries.Num() > 1000)
		{
			Entries.RemoveAt(0, Entries.Num() - 1000);
		}
	}

	void LogOperation(const FString& Operation, const FString& Details, const FString& FilePath = TEXT(""), bool bSuccess = true)
	{
		LogEntry(FAuditLogEntry(Operation, Details, FilePath, bSuccess));
	}

	TArray<FAuditLogEntry> GetEntries() const
	{
		FScopeLock Lock(&CriticalSection);
		return Entries;
	}

	void Clear()
	{
		FScopeLock Lock(&CriticalSection);
		Entries.Empty();
	}

private:
	FAuditLog() {}
	~FAuditLog() {}

	// Non-copyable
	FAuditLog(const FAuditLog&) = delete;
	FAuditLog& operator=(const FAuditLog&) = delete;

	TArray<FAuditLogEntry> Entries;
	mutable FCriticalSection CriticalSection;
};
