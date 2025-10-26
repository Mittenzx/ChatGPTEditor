// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Entry type for Blueprint operations in the audit log
 */
enum class EBlueprintAuditType : uint8
{
	Generation,
	Explanation,
	PreviewShown,
	UserApproved,
	UserRejected
};

/**
 * Single audit log entry for Blueprint operations
 */
struct FBlueprintAuditEntry
{
	FDateTime Timestamp;
	EBlueprintAuditType Type;
	FString Description;
	FString UserPrompt;
	FString GeneratedContent;
	bool bWasApproved;

	FBlueprintAuditEntry()
		: Timestamp(FDateTime::Now())
		, Type(EBlueprintAuditType::Generation)
		, bWasApproved(false)
	{
	}
};

/**
 * Audit logging system for Blueprint Scripting Assistant
 * Tracks all Blueprint generation and explanation requests for security and compliance
 */
class FBlueprintAuditLog
{
public:
	/** Get singleton instance */
	static FBlueprintAuditLog& Get();

	/** Log a Blueprint generation request */
	void LogGeneration(const FString& UserPrompt, const FString& GeneratedContent);

	/** Log a Blueprint explanation request */
	void LogExplanation(const FString& BlueprintName, const FString& Explanation);

	/** Log preview shown to user */
	void LogPreviewShown(const FString& PreviewContent);

	/** Log user approval of generated Blueprint */
	void LogUserApproval(const FString& BlueprintName);

	/** Log user rejection of generated Blueprint */
	void LogUserRejection(const FString& Reason);

	/** Get all audit entries */
	const TArray<FBlueprintAuditEntry>& GetEntries() const { return Entries; }

	/** Export audit log to file */
	bool ExportToFile(const FString& FilePath) const;

	/** Clear all entries (use with caution) */
	void Clear();

private:
	FBlueprintAuditLog() = default;

	TArray<FBlueprintAuditEntry> Entries;

	void AddEntry(EBlueprintAuditType Type, const FString& Description, const FString& UserPrompt = TEXT(""), const FString& GeneratedContent = TEXT(""));
};
