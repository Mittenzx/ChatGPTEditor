// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChatGPTPythonHandler.h"
#include "Misc/MessageDialog.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Python execution support
#if defined(WITH_PYTHON) && WITH_PYTHON
#include "IPythonScriptPlugin.h"
#endif

#define LOCTEXT_NAMESPACE "ChatGPTPythonHandler"

FChatGPTPythonHandler::FChatGPTPythonHandler()
{
	InitializeDangerousOperations();
}

FChatGPTPythonHandler::~FChatGPTPythonHandler()
{
}

void FChatGPTPythonHandler::InitializeDangerousOperations()
{
	// Operations that should trigger additional warnings
	DangerousOperations = {
		TEXT("import os"),
		TEXT("import sys"),
		TEXT("delete"),
		TEXT("remove"),
		TEXT("destroy"),
		TEXT("unregister"),
		TEXT("save")
	};
	
	// Operations that are forbidden and will cause validation to fail
	ForbiddenOperations = {
		TEXT("import subprocess"),
		TEXT("exec("),
		TEXT("eval("),
		TEXT("__import__"),
		TEXT("compile(")
	};
}

bool FChatGPTPythonHandler::ExecuteScript(const FString& Script, bool bRequireConfirmation)
{
	if (Script.IsEmpty())
	{
		return false;
	}
	
	// Check if Python is available
	if (!IsPythonAvailable())
	{
		FMessageDialog::Open(EAppMsgType::Ok,
			LOCTEXT("PythonNotAvailable", "Python scripting is not available in this editor. Please enable the Python Editor Script Plugin."));
		LogScriptExecution(Script, false, TEXT("Python not available"));
		return false;
	}
	
	// Validate the script
	TArray<FString> Warnings;
	if (!ValidateScript(Script, Warnings))
	{
		FMessageDialog::Open(EAppMsgType::Ok,
			LOCTEXT("ScriptValidationFailed", "Script validation failed. Please review the script for security concerns."));
		LogScriptExecution(Script, false, TEXT("Validation failed"));
		return false;
	}
	
	// Show warnings if any
	if (Warnings.Num() > 0 && bRequireConfirmation)
	{
		FString WarningMessage = TEXT("Security Warnings:\n\n");
		for (const FString& Warning : Warnings)
		{
			WarningMessage += TEXT("• ") + Warning + TEXT("\n");
		}
		WarningMessage += TEXT("\nContinue with execution?");
		
		EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo,
			FText::FromString(WarningMessage));
		
		if (Result != EAppReturnType::Yes)
		{
			LogScriptExecution(Script, false, TEXT("User declined after warnings"));
			return false;
		}
	}
	
	// Preview and confirm execution
	if (bRequireConfirmation)
	{
		FString Preview;
		PreviewScript(Script, Preview);
		
		EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo,
			FText::Format(LOCTEXT("ConfirmScriptExecution",
				"Execute Python Script?\n\n{0}\n\nThis script will be executed in the Unreal Editor. Continue?"),
				FText::FromString(Preview)));
		
		if (Result != EAppReturnType::Yes)
		{
			LogScriptExecution(Script, false, TEXT("User declined confirmation"));
			return false;
		}
	}
	
	// Execute the script
#if defined(WITH_PYTHON) && WITH_PYTHON
	IPythonScriptPlugin* PythonPlugin = IPythonScriptPlugin::Get();
	if (PythonPlugin)
	{
		// Execute the script
		FPythonCommandEx PythonCommand;
		PythonCommand.Command = Script;
		PythonCommand.ExecutionMode = EPythonCommandExecutionMode::ExecuteStatement;
		
		bool bSuccess = PythonPlugin->ExecPythonCommandEx(PythonCommand);
		
		if (bSuccess)
		{
			LogScriptExecution(Script, true);
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok,
				LOCTEXT("ScriptExecutionFailed", "Script execution failed. Check the output log for details."));
			LogScriptExecution(Script, false, TEXT("Execution failed"));
		}
		
		return bSuccess;
	}
#endif
	
	LogScriptExecution(Script, false, TEXT("Python plugin not available"));
	return false;
}

void FChatGPTPythonHandler::PreviewScript(const FString& Script, FString& OutPreview) const
{
	// Sanitize and format script for preview
	FString SanitizedScript = SanitizeScriptForPreview(Script);
	
	// Limit preview length
	const int32 MaxPreviewLength = 500;
	if (SanitizedScript.Len() > MaxPreviewLength)
	{
		OutPreview = SanitizedScript.Left(MaxPreviewLength) + TEXT("\n... (truncated)");
	}
	else
	{
		OutPreview = SanitizedScript;
	}
}

bool FChatGPTPythonHandler::IsPythonAvailable() const
{
#if defined(WITH_PYTHON) && WITH_PYTHON
	IPythonScriptPlugin* PythonPlugin = IPythonScriptPlugin::Get();
	return PythonPlugin != nullptr;
#else
	return false;
#endif
}

bool FChatGPTPythonHandler::ValidateScript(const FString& Script, TArray<FString>& OutWarnings) const
{
	OutWarnings.Empty();
	
	FString LowerScript = Script.ToLower();
	
	// Check for forbidden operations first
	for (const FString& ForbiddenOp : ForbiddenOperations)
	{
		if (LowerScript.Contains(ForbiddenOp.ToLower()))
		{
			OutWarnings.Add(FString::Printf(TEXT("FORBIDDEN: Script contains prohibited operation: %s"), *ForbiddenOp));
			return false; // Reject the script
		}
	}
	
	// Check for dangerous operations
	for (const FString& DangerousOp : DangerousOperations)
	{
		if (LowerScript.Contains(DangerousOp.ToLower()))
		{
			OutWarnings.Add(FString::Printf(TEXT("Script contains potentially dangerous operation: %s"), *DangerousOp));
		}
	}
	
	// Script passes validation but may have warnings
	return true;
}

bool FChatGPTPythonHandler::IsPythonScriptRequest(const FString& NaturalLanguageInput) const
{
	FString LowerInput = NaturalLanguageInput.ToLower();
	
	// Check for Python-related keywords
	return LowerInput.Contains(TEXT("python script")) ||
		   LowerInput.Contains(TEXT("write a script")) ||
		   LowerInput.Contains(TEXT("generate script")) ||
		   LowerInput.Contains(TEXT("automate")) ||
		   (LowerInput.Contains(TEXT("script")) && LowerInput.Contains(TEXT("editor")));
}

void FChatGPTPythonHandler::LogScriptExecution(const FString& Script, bool bSuccess, const FString& ErrorMessage)
{
	// Get the audit log path
	FString AuditLogPath = FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor") / TEXT("audit.log");
	
	// Ensure directory exists
	FString LogDir = FPaths::GetPath(AuditLogPath);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*LogDir))
	{
		PlatformFile.CreateDirectoryTree(*LogDir);
	}
	
	// Format log entry - sanitize script for logging (truncate if too long)
	FString ScriptPreview = Script.Len() > 200 ? Script.Left(200) + TEXT("...") : Script;
	ScriptPreview.ReplaceInline(TEXT("\n"), TEXT(" "));
	
	FDateTime Now = FDateTime::Now();
	FString LogEntry = FString::Printf(TEXT("[%s] PYTHON_SCRIPT: %s | Success: %s%s\n"),
		*Now.ToString(),
		*ScriptPreview,
		bSuccess ? TEXT("YES") : TEXT("NO"),
		ErrorMessage.IsEmpty() ? TEXT("") : *FString::Printf(TEXT(" | Error: %s"), *ErrorMessage));
	
	// Append to log file
	FFileHelper::SaveStringToFile(LogEntry, *AuditLogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}

FString FChatGPTPythonHandler::SanitizeScriptForPreview(const FString& Script) const
{
	// Remove excessive whitespace while preserving structure
	FString Sanitized = Script;
	Sanitized.TrimStartAndEndInline();
	
	// Replace multiple newlines with double newlines
	Sanitized.ReplaceInline(TEXT("\n\n\n"), TEXT("\n\n"));
	
	return Sanitized;
}

#undef LOCTEXT_NAMESPACE
