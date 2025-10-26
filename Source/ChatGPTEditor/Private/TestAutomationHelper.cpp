// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestAutomationHelper.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Misc/DateTime.h"

FString FTestAutomationHelper::AuditLogPath;
FCriticalSection FTestAutomationHelper::LogMutex;

void FTestAutomationHelper::InitializeAuditLog()
{
	// Get the Saved/ChatGPTEditor directory path
	FString PluginSavedDir = FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor");
	
	// Create directory if it doesn't exist
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*PluginSavedDir))
	{
		PlatformFile.CreateDirectory(*PluginSavedDir);
	}
	
	// Set audit log path
	AuditLogPath = PluginSavedDir / TEXT("audit.log");
	
	// Write initialization message
	LogAuditMessage(TEXT("SYSTEM"), TEXT("Test Automation audit log initialized"));
}

void FTestAutomationHelper::LogTestGenerationRequest(const FString& UserPrompt, const FString& TestType)
{
	FString Message = FString::Printf(
		TEXT("TEST_GENERATION_REQUEST | Type: %s | Prompt: %s"),
		*TestType,
		*UserPrompt
	);
	WriteToAuditLog(Message);
}

void FTestAutomationHelper::LogTestExecutionRequest(const FString& TestName, const FString& TestPath)
{
	FString Message = FString::Printf(
		TEXT("TEST_EXECUTION_REQUEST | Test: %s | Path: %s"),
		*TestName,
		*TestPath
	);
	WriteToAuditLog(Message);
}

void FTestAutomationHelper::LogTestExecutionResult(const FString& TestName, bool bSuccess, const FString& ResultMessage)
{
	FString Message = FString::Printf(
		TEXT("TEST_EXECUTION_RESULT | Test: %s | Success: %s | Result: %s"),
		*TestName,
		bSuccess ? TEXT("TRUE") : TEXT("FALSE"),
		*ResultMessage
	);
	WriteToAuditLog(Message);
}

void FTestAutomationHelper::LogAuditMessage(const FString& Category, const FString& Message)
{
	FString FormattedMessage = FString::Printf(
		TEXT("%s | %s"),
		*Category,
		*Message
	);
	WriteToAuditLog(FormattedMessage);
}

FString FTestAutomationHelper::GetAuditLogPath()
{
	if (AuditLogPath.IsEmpty())
	{
		InitializeAuditLog();
	}
	return AuditLogPath;
}

bool FTestAutomationHelper::ParseTestCodeFromResponse(const FString& Response, FString& OutTestCode, FString& OutTestName)
{
	// Look for code blocks marked with ```cpp or ```c++
	const FString CppMarkerStart = TEXT("```cpp");
	const FString CppAltMarkerStart = TEXT("```c++");
	const FString MarkerEnd = TEXT("```");
	
	int32 StartIdx = Response.Find(CppMarkerStart, ESearchCase::IgnoreCase);
	if (StartIdx == INDEX_NONE)
	{
		StartIdx = Response.Find(CppAltMarkerStart, ESearchCase::IgnoreCase);
		if (StartIdx == INDEX_NONE)
		{
			return false;
		}
		StartIdx += CppAltMarkerStart.Len();
	}
	else
	{
		StartIdx += CppMarkerStart.Len();
	}
	
	// Skip newline after marker
	if (StartIdx < Response.Len() && (Response[StartIdx] == '\n' || Response[StartIdx] == '\r'))
	{
		StartIdx++;
		if (StartIdx < Response.Len() && Response[StartIdx] == '\n')
		{
			StartIdx++;
		}
	}
	
	// Find end marker
	int32 EndIdx = Response.Find(MarkerEnd, ESearchCase::IgnoreCase, ESearchDir::FromStart, StartIdx);
	if (EndIdx == INDEX_NONE)
	{
		return false;
	}
	
	// Extract code
	OutTestCode = Response.Mid(StartIdx, EndIdx - StartIdx).TrimStartAndEnd();
	
	// Try to extract test name from code (look for class name or IMPLEMENT_SIMPLE_AUTOMATION_TEST)
	int32 TestNameIdx = OutTestCode.Find(TEXT("IMPLEMENT_SIMPLE_AUTOMATION_TEST"), ESearchCase::IgnoreCase);
	if (TestNameIdx != INDEX_NONE)
	{
		// Extract test name from macro
		int32 OpenParen = OutTestCode.Find(TEXT("("), ESearchCase::IgnoreCase, ESearchDir::FromStart, TestNameIdx);
		int32 Comma = OutTestCode.Find(TEXT(","), ESearchCase::IgnoreCase, ESearchDir::FromStart, OpenParen);
		if (OpenParen != INDEX_NONE && Comma != INDEX_NONE)
		{
			OutTestName = OutTestCode.Mid(OpenParen + 1, Comma - OpenParen - 1).TrimStartAndEnd();
		}
	}
	
	return !OutTestCode.IsEmpty();
}

bool FTestAutomationHelper::ValidateTestCode(const FString& TestCode, TArray<FString>& OutWarnings)
{
	OutWarnings.Empty();
	
	// List of potentially dangerous operations to check for
	TArray<FString> DangerousOperations = {
		TEXT("system("),
		TEXT("exec("),
		TEXT("ShellExecute"),
		TEXT("CreateProcess"),
		TEXT("FPlatformProcess::CreateProc"),
		TEXT("DeleteFile"),
		TEXT("RemoveDirectory"),
		TEXT("IFileManager::Delete"),
		TEXT("#include <windows.h>"),
		TEXT("#include <stdlib.h>")
	};
	
	for (const FString& Operation : DangerousOperations)
	{
		if (TestCode.Contains(Operation, ESearchCase::IgnoreCase))
		{
			OutWarnings.Add(FString::Printf(TEXT("Warning: Potentially dangerous operation found: %s"), *Operation));
		}
	}
	
	// Check for excessive file I/O operations
	int32 FileWriteCount = 0;
	TArray<FString> FileOperations = {
		TEXT("FFileHelper::SaveStringToFile"),
		TEXT("FFileHelper::SaveArrayToFile"),
		TEXT("IFileHandle::Write")
	};
	
	for (const FString& FileOp : FileOperations)
	{
		int32 SearchStart = 0;
		while (TestCode.Find(FileOp, ESearchCase::IgnoreCase, ESearchDir::FromStart, SearchStart) != INDEX_NONE)
		{
			FileWriteCount++;
			SearchStart = TestCode.Find(FileOp, ESearchCase::IgnoreCase, ESearchDir::FromStart, SearchStart) + FileOp.Len();
		}
	}
	
	if (FileWriteCount > 3)
	{
		OutWarnings.Add(FString::Printf(TEXT("Warning: Excessive file write operations detected (%d instances)"), FileWriteCount));
	}
	
	// Check if test code has proper structure
	bool bHasAutomationTest = TestCode.Contains(TEXT("IMPLEMENT_SIMPLE_AUTOMATION_TEST"), ESearchCase::IgnoreCase) ||
	                          TestCode.Contains(TEXT("IMPLEMENT_COMPLEX_AUTOMATION_TEST"), ESearchCase::IgnoreCase) ||
	                          TestCode.Contains(TEXT("IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST"), ESearchCase::IgnoreCase);
	
	if (!bHasAutomationTest)
	{
		OutWarnings.Add(TEXT("Warning: Code does not appear to contain standard Unreal Engine automation test macros"));
	}
	
	// Return false only if there are critical security issues
	bool bHasCriticalIssues = false;
	for (const FString& Warning : OutWarnings)
	{
		if (Warning.Contains(TEXT("system(")) || Warning.Contains(TEXT("exec(")) || 
		    Warning.Contains(TEXT("ShellExecute")) || Warning.Contains(TEXT("CreateProcess")))
		{
			bHasCriticalIssues = true;
			break;
		}
	}
	
	return !bHasCriticalIssues;
}

void FTestAutomationHelper::WriteToAuditLog(const FString& Message)
{
	FScopeLock Lock(&LogMutex);
	
	if (AuditLogPath.IsEmpty())
	{
		InitializeAuditLog();
	}
	
	FString TimestampedMessage = FString::Printf(TEXT("[%s] %s\n"), *GetTimestamp(), *Message);
	
	// Append to log file
	FFileHelper::SaveStringToFile(
		TimestampedMessage,
		*AuditLogPath,
		FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(),
		FILEWRITE_Append
	);
}

FString FTestAutomationHelper::GetTimestamp()
{
	FDateTime Now = FDateTime::Now();
	return Now.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
}
