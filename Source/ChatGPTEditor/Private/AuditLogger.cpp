// Copyright Epic Games, Inc. All Rights Reserved.

#include "AuditLogger.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"

FAuditLogger& FAuditLogger::Get()
{
	static FAuditLogger Instance;
	return Instance;
}

void FAuditLogger::Initialize()
{
	FScopeLock Lock(&LogMutex);
	
	if (bInitialized)
	{
		return;
	}
	
	EnsureLogDirectoryExists();
	
	FString InitMessage = FString::Printf(
		TEXT("=== Audit Log Initialized ===\nTimestamp: %s\n"),
		*GetTimestamp()
	);
	
	WriteLog(InitMessage);
	bInitialized = true;
}

void FAuditLogger::Shutdown()
{
	FScopeLock Lock(&LogMutex);
	
	if (!bInitialized)
	{
		return;
	}
	
	FString ShutdownMessage = FString::Printf(
		TEXT("=== Audit Log Shutdown ===\nTimestamp: %s\n\n"),
		*GetTimestamp()
	);
	
	WriteLog(ShutdownMessage);
	bInitialized = false;
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

FString FAuditLogger::GetAuditLogPath() const
{
	// Log to Saved/ChatGPTEditor/audit.log
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
		PlatformFile.CreateDirectoryTree(*LogDir);
	}
}
