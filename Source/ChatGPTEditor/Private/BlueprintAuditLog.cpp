// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintAuditLog.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

FBlueprintAuditLog& FBlueprintAuditLog::Get()
{
	static FBlueprintAuditLog Instance;
	return Instance;
}

void FBlueprintAuditLog::LogGeneration(const FString& UserPrompt, const FString& GeneratedContent)
{
	AddEntry(EBlueprintAuditType::Generation, TEXT("Blueprint generation requested"), UserPrompt, GeneratedContent);
}

void FBlueprintAuditLog::LogExplanation(const FString& BlueprintName, const FString& Explanation)
{
	AddEntry(EBlueprintAuditType::Explanation, FString::Printf(TEXT("Blueprint explanation for: %s"), *BlueprintName), TEXT(""), Explanation);
}

void FBlueprintAuditLog::LogPreviewShown(const FString& PreviewContent)
{
	AddEntry(EBlueprintAuditType::PreviewShown, TEXT("Preview shown to user"), TEXT(""), PreviewContent);
}

void FBlueprintAuditLog::LogUserApproval(const FString& BlueprintName)
{
	if (Entries.Num() > 0)
	{
		Entries.Last().bWasApproved = true;
	}
	AddEntry(EBlueprintAuditType::UserApproved, FString::Printf(TEXT("User approved Blueprint: %s"), *BlueprintName));
}

void FBlueprintAuditLog::LogUserRejection(const FString& Reason)
{
	AddEntry(EBlueprintAuditType::UserRejected, FString::Printf(TEXT("User rejected: %s"), *Reason));
}

void FBlueprintAuditLog::AddEntry(EBlueprintAuditType Type, const FString& Description, const FString& UserPrompt, const FString& GeneratedContent)
{
	FBlueprintAuditEntry Entry;
	Entry.Timestamp = FDateTime::Now();
	Entry.Type = Type;
	Entry.Description = Description;
	Entry.UserPrompt = UserPrompt;
	Entry.GeneratedContent = GeneratedContent;
	Entry.bWasApproved = false;

	Entries.Add(Entry);

	// Log to output for debugging
	UE_LOG(LogTemp, Log, TEXT("[BlueprintAudit] %s: %s"), *Entry.Timestamp.ToString(), *Description);
}

bool FBlueprintAuditLog::ExportToFile(const FString& FilePath) const
{
	FString LogContent = TEXT("Blueprint Scripting Assistant - Audit Log\n");
	LogContent += TEXT("========================================\n\n");

	for (const FBlueprintAuditEntry& Entry : Entries)
	{
		FString TypeString;
		switch (Entry.Type)
		{
		case EBlueprintAuditType::Generation:
			TypeString = TEXT("GENERATION");
			break;
		case EBlueprintAuditType::Explanation:
			TypeString = TEXT("EXPLANATION");
			break;
		case EBlueprintAuditType::PreviewShown:
			TypeString = TEXT("PREVIEW");
			break;
		case EBlueprintAuditType::UserApproved:
			TypeString = TEXT("APPROVED");
			break;
		case EBlueprintAuditType::UserRejected:
			TypeString = TEXT("REJECTED");
			break;
		default:
			TypeString = TEXT("UNKNOWN");
		}

		LogContent += FString::Printf(TEXT("[%s] %s\n"), *Entry.Timestamp.ToString(), *TypeString);
		LogContent += FString::Printf(TEXT("Description: %s\n"), *Entry.Description);
		
		if (!Entry.UserPrompt.IsEmpty())
		{
			LogContent += FString::Printf(TEXT("User Prompt: %s\n"), *Entry.UserPrompt);
		}
		
		if (!Entry.GeneratedContent.IsEmpty())
		{
			LogContent += FString::Printf(TEXT("Generated Content: %s\n"), *Entry.GeneratedContent);
		}
		
		LogContent += FString::Printf(TEXT("Approved: %s\n"), Entry.bWasApproved ? TEXT("Yes") : TEXT("No"));
		LogContent += TEXT("\n---\n\n");
	}

	return FFileHelper::SaveStringToFile(LogContent, *FilePath);
}

void FBlueprintAuditLog::Clear()
{
	Entries.Empty();
	UE_LOG(LogTemp, Warning, TEXT("[BlueprintAudit] Audit log cleared"));
}
