// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Manages project file operations with security controls
 * Handles reading, previewing, and writing project configuration files
 * Requires explicit confirmation before any file modifications
 */
class FProjectFileManager
{
public:
	/** Structure to hold file change preview information */
	struct FFileChangePreview
	{
		FString FilePath;
		FString OriginalContent;
		FString NewContent;
		FString DiffPreview;
		bool bIsValid;

		FFileChangePreview()
			: bIsValid(false)
		{}
	};

	/** Get singleton instance */
	static FProjectFileManager& Get();

	/**
	 * Read a project file (e.g., DefaultEngine.ini, .uproject)
	 * @param FilePath - Relative or absolute path to the file
	 * @param OutContent - The file content if successful
	 * @return true if read was successful
	 */
	bool ReadProjectFile(const FString& FilePath, FString& OutContent);

	/**
	 * Preview changes to a project file before applying
	 * @param FilePath - Path to the file to modify
	 * @param NewContent - The new content to write
	 * @return Preview structure with diff information
	 */
	FFileChangePreview PreviewFileChanges(const FString& FilePath, const FString& NewContent);

	/**
	 * Write changes to a project file after confirmation
	 * Creates a backup before writing
	 * @param Preview - The preview object from PreviewFileChanges
	 * @return true if write was successful
	 */
	bool WriteProjectFile(const FFileChangePreview& Preview);

	/**
	 * Check if a file path is safe to access (within project directory)
	 * @param FilePath - Path to validate
	 * @return true if path is safe
	 */
	bool IsPathSafe(const FString& FilePath) const;

	/**
	 * Get the absolute path for a project file
	 * Handles both relative and absolute paths
	 * @param FilePath - Input file path
	 * @return Absolute path or empty string if invalid
	 */
	FString GetAbsoluteProjectPath(const FString& FilePath) const;

private:
	/** Private constructor for singleton */
	FProjectFileManager();

	/** Create a backup of a file before modifying it */
	bool CreateBackup(const FString& FilePath);

	/** Generate a simple diff preview between two strings */
	FString GenerateDiffPreview(const FString& Original, const FString& New) const;
};
