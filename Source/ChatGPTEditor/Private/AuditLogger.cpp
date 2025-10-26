// Copyright Epic Games, Inc. All Rights Reserved.

#include "AuditLogger.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

FAuditLogger& FAuditLogger::Get()
{
	static FAuditLogger Instance;
	return Instance;
}

FAuditLogger::FAuditLogger()
{
	EnsureLogDirectoryExists();
	LogOperation("System", "Audit logging initialized");
}

void FAuditLogger::EnsureLogDirectoryExists()
{
	FString LogDir = FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	
	if (!PlatformFile.DirectoryExists(*LogDir))
	{
		PlatformFile.CreateDirectory(*LogDir);
	}
}

FString FAuditLogger::GetAuditLogPath() const
{
	return FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor") / TEXT("audit.log");
}

void FAuditLogger::WriteLogEntry(const FString& Entry)
{
	FScopeLock Lock(&LogMutex);
	
	// Get current timestamp
	FString Timestamp = FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S"));
	FString LogEntry = FString::Printf(TEXT("[%s] %s\n"), *Timestamp, *Entry);
	
	// Append to log file
	FString LogPath = GetAuditLogPath();
	FFileHelper::SaveStringToFile(LogEntry, *LogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}

void FAuditLogger::LogFileRead(const FString& FilePath)
{
	FString Entry = FString::Printf(TEXT("FILE_READ: %s"), *FilePath);
	WriteLogEntry(Entry);
}

void FAuditLogger::LogFileWrite(const FString& FilePath, const FString& Operation)
{
	FString Entry = FString::Printf(TEXT("FILE_WRITE: %s - Operation: %s"), *FilePath, *Operation);
	WriteLogEntry(Entry);
}

void FAuditLogger::LogPermissionChange(const FString& PermissionName, bool bEnabled)
{
	FString Entry = FString::Printf(TEXT("PERMISSION_CHANGE: %s = %s"), *PermissionName, bEnabled ? TEXT("ENABLED") : TEXT("DISABLED"));
	WriteLogEntry(Entry);
}

void FAuditLogger::LogOperation(const FString& Category, const FString& Message)
{
	FString Entry = FString::Printf(TEXT("%s: %s"), *Category, *Message);
	WriteLogEntry(Entry);
}

void FAuditLogger::LogError(const FString& Category, const FString& ErrorMessage)
{
	FString Entry = FString::Printf(TEXT("ERROR [%s]: %s"), *Category, *ErrorMessage);
	WriteLogEntry(Entry);
void FAuditLogger::LogOperation(const FString& UserCommand, const FString& OperationType, 
	const FString& AffectedActors, bool bSuccess, const FString& ErrorMessage)
{
	FScopeLock Lock(&LogMutex);

	FAuditLogEntry Entry;
	Entry.Timestamp = FDateTime::Now();
	Entry.UserCommand = UserCommand;
	Entry.OperationType = OperationType;
	Entry.AffectedActors = AffectedActors;
	Entry.bWasSuccessful = bSuccess;
	Entry.ErrorMessage = ErrorMessage;

	LogEntries.Add(Entry);

	// Log to Unreal's output log as well
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("[SceneEdit] %s: %s - Affected: %s"), 
			*OperationType, *UserCommand, *AffectedActors);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SceneEdit] FAILED - %s: %s - Error: %s"), 
			*OperationType, *UserCommand, *ErrorMessage);
	}
}

TArray<FAuditLogEntry> FAuditLogger::GetRecentEntries(int32 Count) const
{
	FScopeLock Lock(&const_cast<FCriticalSection&>(LogMutex));
	
	TArray<FAuditLogEntry> RecentEntries;
	int32 StartIndex = FMath::Max(0, LogEntries.Num() - Count);
	
	for (int32 i = StartIndex; i < LogEntries.Num(); ++i)
	{
		RecentEntries.Add(LogEntries[i]);
	}
	
	return RecentEntries;
}

void FAuditLogger::ClearLog()
{
	FScopeLock Lock(&LogMutex);
	LogEntries.Empty();
	UE_LOG(LogTemp, Log, TEXT("[SceneEdit] Audit log cleared"));
}

FString FAuditLogger::ExportLogToString() const
{
	FScopeLock Lock(&const_cast<FCriticalSection&>(LogMutex));
	
	FString Result = TEXT("=== Scene Editing Audit Log ===\n\n");
	
	for (const FAuditLogEntry& Entry : LogEntries)
	{
		Result += FString::Printf(TEXT("[%s] %s\n"), 
			*Entry.Timestamp.ToString(), 
			*Entry.OperationType);
		Result += FString::Printf(TEXT("  Command: %s\n"), *Entry.UserCommand);
		Result += FString::Printf(TEXT("  Affected: %s\n"), *Entry.AffectedActors);
		Result += FString::Printf(TEXT("  Status: %s\n"), 
			Entry.bWasSuccessful ? TEXT("SUCCESS") : TEXT("FAILED"));
		
		if (!Entry.ErrorMessage.IsEmpty())
		{
			Result += FString::Printf(TEXT("  Error: %s\n"), *Entry.ErrorMessage);
		}
		
		Result += TEXT("\n");
	}
	
	return Result;
}
