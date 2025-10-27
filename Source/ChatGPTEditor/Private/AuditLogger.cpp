// Copyright Epic Games, Inc. All Rights Reserved.

#include "AuditLogger.h"
#include "ChatGPTEditor.h"
#include "SceneEditingTypes.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"

FAuditLogger& FAuditLogger::Get()
{
	static FAuditLogger Instance;
	return Instance;
}

FAuditLogger::FAuditLogger()
	: bInitialized(false)
{
}

void FAuditLogger::Initialize()
{
	FScopeLock Lock(&LogMutex);
	
	if (bInitialized)
	{
		UE_LOG(LogChatGPTEditor, Verbose, TEXT("AuditLogger already initialized"));
		return;
	}
	
	UE_LOG(LogChatGPTEditor, Log, TEXT("Initializing AuditLogger..."));
	
	EnsureLogDirectoryExists();
	
	FString InitMessage = FString::Printf(
		TEXT("=== Audit Log Initialized ===\nTimestamp: %s\n"),
		*GetTimestamp()
	);
	
	WriteLog(InitMessage);
	bInitialized = true;
	
	UE_LOG(LogChatGPTEditor, Log, TEXT("AuditLogger initialized successfully. Log path: %s"), *GetAuditLogPath());
}

void FAuditLogger::Shutdown()
{
	FScopeLock Lock(&LogMutex);
	
	if (!bInitialized)
	{
		UE_LOG(LogChatGPTEditor, Verbose, TEXT("AuditLogger shutdown called but not initialized"));
		return;
	}
	
	UE_LOG(LogChatGPTEditor, Log, TEXT("Shutting down AuditLogger..."));
	
	FString ShutdownMessage = FString::Printf(
		TEXT("=== Audit Log Shutdown ===\nTimestamp: %s\n\n"),
		*GetTimestamp()
	);
	
	WriteLog(ShutdownMessage);
	bInitialized = false;
	
	UE_LOG(LogChatGPTEditor, Log, TEXT("AuditLogger shutdown complete"));
}

void FAuditLogger::LogAPIConnection(const FString& Endpoint, const FString& Method, bool bApproved)
{
	FString Status = bApproved ? TEXT("APPROVED") : TEXT("DENIED");
	FString LogEntry = FString::Printf(
		TEXT("[%s] API_CONNECTION | Status: %s | Method: %s | Endpoint: %s\n"),
		*GetTimestamp(),
		*Status,
		*Method,
		*Endpoint
	);
	
	WriteLog(LogEntry);
}

void FAuditLogger::LogCodeChange(const FString& Description, const FString& CodePreview, bool bApproved)
{
	FString Status = bApproved ? TEXT("APPROVED") : TEXT("DENIED");
	FString LogEntry = FString::Printf(
		TEXT("[%s] CODE_CHANGE | Status: %s | Description: %s\nCode Preview:\n%s\n---\n"),
		*GetTimestamp(),
		*Status,
		*Description,
		*CodePreview
	);
	
	WriteLog(LogEntry);
}

void FAuditLogger::LogEvent(const FString& EventType, const FString& Message)
{
	FString LogEntry = FString::Printf(
		TEXT("[%s] %s | %s\n"),
		*GetTimestamp(),
		*EventType,
		*Message
	);
	
	WriteLog(LogEntry);
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
}

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

	// Also write to the audit log file
	FString LogText = FString::Printf(
		TEXT("[SceneEdit] %s: %s - Affected: %s - Status: %s"),
		*OperationType, *UserCommand, *AffectedActors, bSuccess ? TEXT("SUCCESS") : TEXT("FAILED")
	);
	if (!ErrorMessage.IsEmpty())
	{
		LogText += FString::Printf(TEXT(" - Error: %s"), *ErrorMessage);
	}
	WriteLogEntry(LogText);
}

TArray<FAuditLogEntry> FAuditLogger::GetLogEntries() const
{
	FScopeLock Lock(&const_cast<FCriticalSection&>(LogMutex));
	return LogEntries;
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
	LogEvent(TEXT("AUDIT_LOG"), TEXT("Log cleared by user"));
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

FString FAuditLogger::GetAuditLogPath() const
{
	FString SavedDir = FPaths::ProjectSavedDir();
	return FPaths::Combine(SavedDir, TEXT("ChatGPTEditor"), TEXT("audit.log"));
}

void FAuditLogger::WriteLog(const FString& Entry)
{
	FScopeLock Lock(&LogMutex);
	
	FString LogPath = GetAuditLogPath();
	
	// Append to log file
	IFileManager& FileManager = IFileManager::Get();
	FArchive* LogFile = FileManager.CreateFileWriter(*LogPath, FILEWRITE_Append);
	
	if (LogFile)
	{
		FString EntryWithNewline = Entry;
		if (!Entry.EndsWith(TEXT("\n")))
		{
			EntryWithNewline += TEXT("\n");
		}
		
		// Convert to UTF-8
		FTCHARToUTF8 UTF8String(*EntryWithNewline);
		LogFile->Serialize((void*)UTF8String.Get(), UTF8String.Length());
		LogFile->Close();
		delete LogFile;
	}
	else
	{
		UE_LOG(LogChatGPTEditor, Error, TEXT("Failed to open audit log file for writing: %s"), *LogPath);
	}
}

void FAuditLogger::WriteLogEntry(const FString& Entry)
{
	FString Timestamp = GetTimestamp();
	FString LogEntry = FString::Printf(TEXT("[%s] %s"), *Timestamp, *Entry);
	WriteLog(LogEntry);
}

FString FAuditLogger::GetTimestamp() const
{
	FDateTime Now = FDateTime::Now();
	return Now.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
}

void FAuditLogger::EnsureLogDirectoryExists()
{
	FString LogPath = GetAuditLogPath();
	FString LogDir = FPaths::GetPath(LogPath);
	
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*LogDir))
	{
		UE_LOG(LogChatGPTEditor, Log, TEXT("Creating audit log directory: %s"), *LogDir);
		if (!PlatformFile.CreateDirectoryTree(*LogDir))
		{
			UE_LOG(LogChatGPTEditor, Error, TEXT("Failed to create audit log directory: %s"), *LogDir);
		}
	}
}
