// Copyright Epic Games, Inc. All Rights Reserved.

#include "DocumentationHandler.h"
#include "AuditLogger.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

bool FDocumentationHandler::ParseDocumentationRequest(const FString& UserMessage, const FString& AssistantResponse, FDocumentationChange& OutChange)
{
	// Simple parsing logic - look for common patterns
	// This is a basic implementation that can be enhanced
	
	FString LowerUserMsg = UserMessage.ToLower();
	
	// Check for documentation generation/update requests
	if (LowerUserMsg.Contains(TEXT("generate")) || LowerUserMsg.Contains(TEXT("create")) || 
		LowerUserMsg.Contains(TEXT("update")) || LowerUserMsg.Contains(TEXT("write")))
	{
		if (LowerUserMsg.Contains(TEXT("readme")) || LowerUserMsg.Contains(TEXT("documentation")) || 
			LowerUserMsg.Contains(TEXT("doc")) || LowerUserMsg.Contains(TEXT(".md")))
		{
			// This is a documentation request
			OutChange.Description = TEXT("Documentation generation/update request");
			
			// Try to extract file path from the message
			if (LowerUserMsg.Contains(TEXT("readme")))
			{
				OutChange.FilePath = FPaths::Combine(GetPluginDirectory(), TEXT("README.md"));
				OutChange.bIsNewFile = false;
			}
			
			// The proposed content would come from the assistant response
			// For now, we mark this as needing manual review
			OutChange.ProposedContent = AssistantResponse;
			
			return true;
		}
	}
	
	return false;
}

FString FDocumentationHandler::PreviewChange(const FDocumentationChange& Change)
{
	FString Preview;
	Preview += TEXT("==================== DOCUMENTATION CHANGE PREVIEW ====================\n\n");
	Preview += FString::Printf(TEXT("Description: %s\n"), *Change.Description);
	Preview += FString::Printf(TEXT("File Path: %s\n"), *Change.FilePath);
	Preview += FString::Printf(TEXT("Type: %s\n\n"), Change.bIsNewFile ? TEXT("New File") : TEXT("Update Existing File"));
	
	if (!Change.bIsNewFile && !Change.OriginalContent.IsEmpty())
	{
		Preview += TEXT("--- ORIGINAL CONTENT (First 500 chars) ---\n");
		Preview += Change.OriginalContent.Left(500);
		if (Change.OriginalContent.Len() > 500)
		{
			Preview += TEXT("\n... (truncated)");
		}
		Preview += TEXT("\n\n");
	}
	
	Preview += TEXT("--- PROPOSED CONTENT (First 500 chars) ---\n");
	Preview += Change.ProposedContent.Left(500);
	if (Change.ProposedContent.Len() > 500)
	{
		Preview += TEXT("\n... (truncated)");
	}
	Preview += TEXT("\n\n");
	
	Preview += TEXT("======================================================================\n");
	
	return Preview;
}

bool FDocumentationHandler::ApplyChange(const FDocumentationChange& Change, FString& OutError)
{
	// Validate file path
	if (!IsSafeFilePath(Change.FilePath))
	{
		OutError = TEXT("File path is outside the plugin directory. Operation rejected for security reasons.");
		FAuditLogger::Get().LogError(TEXT("ApplyDocChange"), FString::Printf(TEXT("%s - %s"), *OutError, *Change.FilePath));
		return false;
	}

	// Write the file
	if (!WriteFile(Change.FilePath, Change.ProposedContent, OutError))
	{
		FAuditLogger::Get().LogError(TEXT("ApplyDocChange"), FString::Printf(TEXT("Failed to write file: %s - %s"), *OutError, *Change.FilePath));
		return false;
	}

	FAuditLogger::Get().LogFileWrite(Change.FilePath, Change.Description);
	return true;
}

bool FDocumentationHandler::IsDocumentationRequest(const FString& UserMessage)
{
	FString LowerMsg = UserMessage.ToLower();
	
	TArray<FString> DocKeywords = {
		TEXT("readme"), TEXT("documentation"), TEXT("generate doc"), TEXT("create doc"),
		TEXT("update doc"), TEXT("write doc"), TEXT("api doc"), TEXT("usage guide")
	};
	
	for (const FString& Keyword : DocKeywords)
	{
		if (LowerMsg.Contains(Keyword))
		{
			return true;
		}
	}
	
	return false;
}

bool FDocumentationHandler::IsCodeExplanationRequest(const FString& UserMessage)
{
	FString LowerMsg = UserMessage.ToLower();
	
	TArray<FString> ExplainKeywords = {
		TEXT("explain"), TEXT("what does"), TEXT("how does"), TEXT("describe"),
		TEXT("summarize"), TEXT("what is"), TEXT("review"), TEXT("analyze")
	};
	
	for (const FString& Keyword : ExplainKeywords)
	{
		if (LowerMsg.Contains(Keyword))
		{
			return true;
		}
	}
	
	return false;
}

bool FDocumentationHandler::ReadFile(const FString& FilePath, FString& OutContent, FString& OutError)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	
	if (!PlatformFile.FileExists(*FilePath))
	{
		OutError = TEXT("File does not exist");
		return false;
	}
	
	if (!FFileHelper::LoadFileToString(OutContent, *FilePath))
	{
		OutError = TEXT("Failed to read file");
		return false;
	}
	
	return true;
}

bool FDocumentationHandler::WriteFile(const FString& FilePath, const FString& Content, FString& OutError)
{
	// Ensure directory exists
	FString Directory = FPaths::GetPath(FilePath);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	
	if (!PlatformFile.DirectoryExists(*Directory))
	{
		if (!PlatformFile.CreateDirectoryTree(*Directory))
		{
			OutError = TEXT("Failed to create directory");
			return false;
		}
	}
	
	if (!FFileHelper::SaveStringToFile(Content, *FilePath))
	{
		OutError = TEXT("Failed to write file");
		return false;
	}
	
	return true;
}

FString FDocumentationHandler::GetPluginDirectory()
{
	// Get the plugin's base directory
	FString PluginDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() / TEXT("ChatGPTEditor"));
	
	// If not in project plugins, try engine plugins
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*PluginDir))
	{
		PluginDir = FPaths::ConvertRelativePathToFull(FPaths::EnginePluginsDir() / TEXT("ChatGPTEditor"));
	}
	
	return PluginDir;
}

bool FDocumentationHandler::IsSafeFilePath(const FString& FilePath)
{
	FString FullPath = FPaths::ConvertRelativePathToFull(FilePath);
	FString PluginDir = GetPluginDirectory();
	
	// Path must be within the plugin directory
	if (!FullPath.StartsWith(PluginDir))
	{
		return false;
	}
	
	// Path must not try to escape using ..
	if (FilePath.Contains(TEXT("..")))
	{
		return false;
	}
	
	// Only allow safe file extensions
	FString Extension = FPaths::GetExtension(FilePath).ToLower();
	TArray<FString> AllowedExtensions = { TEXT("md"), TEXT("txt"), TEXT("rst"), TEXT("adoc") };
	
	if (!AllowedExtensions.Contains(Extension))
	{
		return false;
	}
	
	return true;
}
