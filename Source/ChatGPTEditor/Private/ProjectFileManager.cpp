// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectFileManager.h"
#include "AuditLogger.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

FProjectFileManager& FProjectFileManager::Get()
{
	static FProjectFileManager Instance;
	return Instance;
}

FProjectFileManager::FProjectFileManager()
{
}

bool FProjectFileManager::IsPathSafe(const FString& FilePath) const
{
	FString AbsolutePath = GetAbsoluteProjectPath(FilePath);
	if (AbsolutePath.IsEmpty())
	{
		return false;
	}

	// Ensure the path is within the project directory
	FString ProjectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FString NormalizedPath = FPaths::ConvertRelativePathToFull(AbsolutePath);

	// Check if the normalized path starts with the project directory
	return NormalizedPath.StartsWith(ProjectDir);
}

FString FProjectFileManager::GetAbsoluteProjectPath(const FString& FilePath) const
{
	if (FilePath.IsEmpty())
	{
		return FString();
	}

	FString AbsolutePath;
	
	// If already absolute, use it
	if (FPaths::IsRelative(FilePath))
	{
		// Relative to project directory
		AbsolutePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / FilePath);
	}
	else
	{
		AbsolutePath = FPaths::ConvertRelativePathToFull(FilePath);
	}

	// Normalize the path
	FPaths::NormalizeFilename(AbsolutePath);
	
	return AbsolutePath;
}

bool FProjectFileManager::ReadProjectFile(const FString& FilePath, FString& OutContent)
{
	// Validate path safety
	if (!IsPathSafe(FilePath))
	{
		FAuditLogger::Get().LogError("ProjectFileManager", 
			FString::Printf(TEXT("Attempted to read unsafe path: %s"), *FilePath));
		return false;
	}

	FString AbsolutePath = GetAbsoluteProjectPath(FilePath);
	
	// Check if file exists
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.FileExists(*AbsolutePath))
	{
		FAuditLogger::Get().LogError("ProjectFileManager", 
			FString::Printf(TEXT("File not found: %s"), *AbsolutePath));
		return false;
	}

	// Read the file
	if (!FFileHelper::LoadFileToString(OutContent, *AbsolutePath))
	{
		FAuditLogger::Get().LogError("ProjectFileManager", 
			FString::Printf(TEXT("Failed to read file: %s"), *AbsolutePath));
		return false;
	}

	// Log the read operation
	FAuditLogger::Get().LogFileRead(AbsolutePath);
	
	return true;
}

FProjectFileManager::FFileChangePreview FProjectFileManager::PreviewFileChanges(const FString& FilePath, const FString& NewContent)
{
	FFileChangePreview Preview;
	Preview.FilePath = GetAbsoluteProjectPath(FilePath);
	Preview.NewContent = NewContent;
	Preview.bIsValid = false;

	// Validate path safety
	if (!IsPathSafe(FilePath))
	{
		FAuditLogger::Get().LogError("ProjectFileManager", 
			FString::Printf(TEXT("Attempted to preview changes for unsafe path: %s"), *FilePath));
		return Preview;
	}

	// Read original content
	if (!ReadProjectFile(FilePath, Preview.OriginalContent))
	{
		return Preview;
	}

	// Generate diff preview
	Preview.DiffPreview = GenerateDiffPreview(Preview.OriginalContent, Preview.NewContent);
	Preview.bIsValid = true;

	FAuditLogger::Get().LogOperation("ProjectFileManager", 
		FString::Printf(TEXT("Generated preview for: %s"), *Preview.FilePath));

	return Preview;
}

bool FProjectFileManager::CreateBackup(const FString& FilePath)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	
	// Create backup filename with timestamp
	FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
	FString BackupPath = FilePath + TEXT(".backup_") + Timestamp;

	// Copy the file to backup
	if (!PlatformFile.CopyFile(*BackupPath, *FilePath))
	{
		FAuditLogger::Get().LogError("ProjectFileManager", 
			FString::Printf(TEXT("Failed to create backup: %s"), *BackupPath));
		return false;
	}

	FAuditLogger::Get().LogOperation("ProjectFileManager", 
		FString::Printf(TEXT("Created backup: %s"), *BackupPath));

	return true;
}

bool FProjectFileManager::WriteProjectFile(const FFileChangePreview& Preview)
{
	if (!Preview.bIsValid)
	{
		FAuditLogger::Get().LogError("ProjectFileManager", 
			TEXT("Attempted to write with invalid preview"));
		return false;
	}

	// Validate path safety again
	if (!IsPathSafe(Preview.FilePath))
	{
		FAuditLogger::Get().LogError("ProjectFileManager", 
			FString::Printf(TEXT("Attempted to write to unsafe path: %s"), *Preview.FilePath));
		return false;
	}

	// Create backup before writing
	if (!CreateBackup(Preview.FilePath))
	{
		return false;
	}

	// Write the new content
	if (!FFileHelper::SaveStringToFile(Preview.NewContent, *Preview.FilePath))
	{
		FAuditLogger::Get().LogError("ProjectFileManager", 
			FString::Printf(TEXT("Failed to write file: %s"), *Preview.FilePath));
		return false;
	}

	// Log the write operation
	FAuditLogger::Get().LogFileWrite(Preview.FilePath, TEXT("File modified"));

	return true;
}

FString FProjectFileManager::GenerateDiffPreview(const FString& Original, const FString& New) const
{
	TArray<FString> OriginalLines;
	TArray<FString> NewLines;

	Original.ParseIntoArrayLines(OriginalLines);
	New.ParseIntoArrayLines(NewLines);

	FString DiffPreview;
	DiffPreview += TEXT("=== FILE CHANGE PREVIEW ===\n\n");

	int32 MaxLines = FMath::Max(OriginalLines.Num(), NewLines.Num());
	int32 ChangeCount = 0;

	// Simple line-by-line diff
	for (int32 i = 0; i < MaxLines; ++i)
	{
		FString OrigLine = (i < OriginalLines.Num()) ? OriginalLines[i] : TEXT("");
		FString NewLine = (i < NewLines.Num()) ? NewLines[i] : TEXT("");

		if (OrigLine != NewLine)
		{
			ChangeCount++;
			if (ChangeCount <= 20)  // Limit preview to first 20 changes
			{
				if (!OrigLine.IsEmpty())
				{
					DiffPreview += FString::Printf(TEXT("- Line %d: %s\n"), i + 1, *OrigLine);
				}
				if (!NewLine.IsEmpty())
				{
					DiffPreview += FString::Printf(TEXT("+ Line %d: %s\n"), i + 1, *NewLine);
				}
			}
		}
	}

	if (ChangeCount > 20)
	{
		DiffPreview += FString::Printf(TEXT("\n... and %d more changes\n"), ChangeCount - 20);
	}

	DiffPreview += FString::Printf(TEXT("\nTotal changes: %d lines\n"), ChangeCount);
	DiffPreview += TEXT("=== END PREVIEW ===\n");

	return DiffPreview;
}
