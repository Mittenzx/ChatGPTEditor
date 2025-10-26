// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChatGPTConsoleHandler.h"
#include "Engine/Engine.h"
#include "Misc/MessageDialog.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "ChatGPTConsoleHandler"

FChatGPTConsoleHandler::FChatGPTConsoleHandler()
{
	InitializeCommandLists();
}

FChatGPTConsoleHandler::~FChatGPTConsoleHandler()
{
}

void FChatGPTConsoleHandler::InitializeCommandLists()
{
	// Safe read-only commands
	SafeCommands = {
		TEXT("stat fps"),
		TEXT("stat unit"),
		TEXT("stat game"),
		TEXT("stat scenerendering"),
		TEXT("showflag"),
		TEXT("viewmode"),
		TEXT("r.screenpercentage"),
		TEXT("t.maxfps"),
		TEXT("getall"),
		TEXT("obj list"),
		TEXT("help")
	};
	
	// Blacklisted commands that are never allowed
	BlacklistedCommands = {
		TEXT("exit"),
		TEXT("quit"),
		TEXT("crashdebug"),
		TEXT("debug crash"),
		TEXT("obj delete"),
		TEXT("deleteall"),
		TEXT("destroyall")
	};
	
	// Destructive patterns that always require confirmation
	DestructivePatterns = {
		TEXT("delete"),
		TEXT("remove"),
		TEXT("destroy"),
		TEXT("clear"),
		TEXT("reset"),
		TEXT("save"),
		TEXT("load"),
		TEXT("set "),  // Space after "set" to avoid matching "asset" etc.
		TEXT("changesize")
	};
}

bool FChatGPTConsoleHandler::ExecuteCommand(const FString& Command, bool bRequireConfirmation)
{
	if (Command.IsEmpty())
	{
		return false;
	}
	
	// Check if command is blacklisted
	if (IsCommandBlacklisted(Command))
	{
		FMessageDialog::Open(EAppMsgType::Ok, 
			FText::Format(LOCTEXT("CommandBlacklisted", "Command '{0}' is blacklisted for security reasons and cannot be executed."), 
				FText::FromString(Command)));
		LogCommandExecution(Command, false, TEXT("Command is blacklisted"));
		return false;
	}
	
	// Check if confirmation is required
	if (bRequireConfirmation && !IsCommandSafe(Command))
	{
		EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo,
			FText::Format(LOCTEXT("ConfirmCommand", 
				"Execute console command?\n\nCommand: {0}\n\nThis may modify your editor state. Continue?"),
				FText::FromString(Command)));
		
		if (Result != EAppReturnType::Yes)
		{
			LogCommandExecution(Command, false, TEXT("User declined confirmation"));
			return false;
		}
	}
	
	// Execute the command
	if (GEngine)
	{
		GEngine->Exec(nullptr, *Command);
		LogCommandExecution(Command, true);
		return true;
	}
	
	LogCommandExecution(Command, false, TEXT("GEngine is null"));
	return false;
}

bool FChatGPTConsoleHandler::IsCommandSafe(const FString& Command) const
{
	FString LowerCommand = Command.ToLower();
	
	// Check if command is in safe list
	for (const FString& SafeCmd : SafeCommands)
	{
		if (LowerCommand.StartsWith(SafeCmd.ToLower()))
		{
			return true;
		}
	}
	
	// Check for destructive patterns
	for (const FString& Pattern : DestructivePatterns)
	{
		FString LowerPattern = Pattern.ToLower();
		
		// Special handling for "set " to ensure it's actually followed by whitespace
		if (LowerPattern == TEXT("set "))
		{
			// Match "set " followed by any whitespace character
			if (LowerCommand.Contains(TEXT("set ")) || 
				LowerCommand.Contains(TEXT("set\t")) || 
				LowerCommand.Contains(TEXT("set\n")) ||
				LowerCommand.Contains(TEXT("set\r")))
			{
				return false;
			}
		}
		else if (LowerCommand.Contains(LowerPattern))
		{
			return false;
		}
	}
	
	// By default, treat unknown commands as potentially unsafe
	return false;
}

bool FChatGPTConsoleHandler::IsCommandBlacklisted(const FString& Command) const
{
	FString LowerCommand = Command.ToLower();
	
	for (const FString& BlacklistedCmd : BlacklistedCommands)
	{
		if (LowerCommand.StartsWith(BlacklistedCmd.ToLower()))
		{
			return true;
		}
	}
	
	return false;
}

bool FChatGPTConsoleHandler::ParseNaturalLanguageCommand(const FString& NaturalLanguageInput, FString& OutCommand) const
{
	FString LowerInput = NaturalLanguageInput.ToLower();
	
	// Look for patterns indicating a console command request
	if (LowerInput.Contains(TEXT("execute command")) || 
		LowerInput.Contains(TEXT("run command")) ||
		LowerInput.Contains(TEXT("console command")))
	{
		// Try to extract command from quotes
		int32 FirstQuote, SecondQuote;
		if (NaturalLanguageInput.FindChar('\"', FirstQuote))
		{
			if (NaturalLanguageInput.FindChar('\"', SecondQuote) && SecondQuote > FirstQuote + 1)
			{
				OutCommand = NaturalLanguageInput.Mid(FirstQuote + 1, SecondQuote - FirstQuote - 1);
				return true;
			}
		}
		
		// Try to extract command after "command:" or similar
		int32 ColonPos = NaturalLanguageInput.Find(TEXT(":"));
		if (ColonPos != INDEX_NONE)
		{
			OutCommand = NaturalLanguageInput.Mid(ColonPos + 1).TrimStartAndEnd();
			return !OutCommand.IsEmpty();
		}
	}
	
	return false;
}

void FChatGPTConsoleHandler::LogCommandExecution(const FString& Command, bool bSuccess, const FString& ErrorMessage)
{
	// Get the audit log path
	FString LogPath = FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor") / TEXT("audit.log");
	
	// Ensure directory exists
	FString LogDir = FPaths::GetPath(LogPath);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*LogDir))
	{
		PlatformFile.CreateDirectoryTree(*LogDir);
	}
	
	// Format log entry
	FDateTime Now = FDateTime::Now();
	FString LogEntry = FString::Printf(TEXT("[%s] CONSOLE_COMMAND: %s | Success: %s%s\n"),
		*Now.ToString(),
		*Command,
		bSuccess ? TEXT("YES") : TEXT("NO"),
		ErrorMessage.IsEmpty() ? TEXT("") : *FString::Printf(TEXT(" | Error: %s"), *ErrorMessage));
	
	// Append to log file
	FFileHelper::SaveStringToFile(LogEntry, *LogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}

#undef LOCTEXT_NAMESPACE
