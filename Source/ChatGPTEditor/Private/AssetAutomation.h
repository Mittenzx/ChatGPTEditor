// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Represents a parsed asset operation command from ChatGPT
 */
struct FAssetOperation
{
	enum class EType
	{
		CreateMaterial,
		CreateTexture,
		CreateBlueprint,
		RenameAsset,
		MoveAsset,
		DeleteAsset,
		Unknown
	};

	EType Type = EType::Unknown;
	FString AssetName;
	FString TargetPath;
	FString NewName;
	TMap<FString, FString> Properties;
	FString OriginalCommand;

	FAssetOperation() = default;
	
	bool IsValid() const
	{
		return Type != EType::Unknown && !AssetName.IsEmpty();
	}

	FString GetTypeAsString() const
	{
		switch (Type)
		{
		case EType::CreateMaterial: return TEXT("Create Material");
		case EType::CreateTexture: return TEXT("Create Texture");
		case EType::CreateBlueprint: return TEXT("Create Blueprint");
		case EType::RenameAsset: return TEXT("Rename Asset");
		case EType::MoveAsset: return TEXT("Move Asset");
		case EType::DeleteAsset: return TEXT("Delete Asset");
		default: return TEXT("Unknown");
		}
	}

	bool IsDestructive() const
	{
		return Type == EType::DeleteAsset || Type == EType::RenameAsset || Type == EType::MoveAsset;
	}
};

/**
 * Audit log entry for tracking asset operations
 */
struct FAuditLogEntry
{
	FDateTime Timestamp;
	FString Operation;
	FString AssetName;
	FString User;
	bool bSucceeded;
	FString Details;

	FAuditLogEntry()
		: Timestamp(FDateTime::Now())
		, bSucceeded(false)
	{}
};

/**
 * Asset Automation Parser and Executor
 * Parses ChatGPT responses for asset operation commands and executes them with user confirmation
 */
class FAssetAutomation
{
public:
	/**
	 * Parse a ChatGPT response for asset operation commands
	 * @param Response The response text from ChatGPT
	 * @return Array of parsed asset operations
	 */
	static TArray<FAssetOperation> ParseResponse(const FString& Response);

	/**
	 * Execute an asset operation with user confirmation
	 * @param Operation The operation to execute
	 * @param bAllowAssetWrite Whether asset write operations are enabled
	 * @return True if operation was executed successfully
	 */
	static bool ExecuteOperation(const FAssetOperation& Operation, bool bAllowAssetWrite);

	/**
	 * Show confirmation dialog for an asset operation
	 * @param Operation The operation to confirm
	 * @return True if user confirmed
	 */
	static bool ShowConfirmationDialog(const FAssetOperation& Operation);

	/**
	 * Write an entry to the audit log
	 * @param Entry The audit log entry to write
	 */
	static void WriteAuditLog(const FAuditLogEntry& Entry);

	/**
	 * Get the audit log file path
	 * @return Full path to the audit log file
	 */
	static FString GetAuditLogPath();

private:
	/**
	 * Parse a single line for asset operation commands
	 * @param Line The line to parse
	 * @return Parsed operation or invalid operation if not found
	 */
	static FAssetOperation ParseLine(const FString& Line);

	/**
	 * Extract asset name from various command formats
	 * @param Command The command string
	 * @param Keyword The keyword to look for (e.g., "material", "blueprint")
	 * @return Extracted asset name
	 */
	static FString ExtractAssetName(const FString& Command, const FString& Keyword);

	/**
	 * Create a material asset
	 * @param Operation The operation details
	 * @return True if successful
	 */
	static bool CreateMaterial(const FAssetOperation& Operation);

	/**
	 * Create a texture asset
	 * @param Operation The operation details
	 * @return True if successful
	 */
	static bool CreateTexture(const FAssetOperation& Operation);

	/**
	 * Create a blueprint asset
	 * @param Operation The operation details
	 * @return True if successful
	 */
	static bool CreateBlueprint(const FAssetOperation& Operation);

	/**
	 * Rename an asset
	 * @param Operation The operation details
	 * @return True if successful
	 */
	static bool RenameAsset(const FAssetOperation& Operation);

	/**
	 * Move an asset
	 * @param Operation The operation details
	 * @return True if successful
	 */
	static bool MoveAsset(const FAssetOperation& Operation);

	/**
	 * Delete an asset
	 * @param Operation The operation details
	 * @return True if successful
	 */
	static bool DeleteAsset(const FAssetOperation& Operation);
};
