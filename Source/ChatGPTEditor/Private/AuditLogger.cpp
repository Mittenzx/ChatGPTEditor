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
}
