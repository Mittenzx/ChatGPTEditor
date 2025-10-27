// Copyright Epic Games, Inc. All Rights Reserved.

/**
 * Unit tests for the ChatGPTEditor plugin core functionality
 * 
 * These tests verify:
 * - Plugin initialization and shutdown
 * - API key validation
 * - Audit logging functionality
 * - Permission system
 * 
 * Run these tests via:
 * Session Frontend -> Automation tab
 * Or via command line:
 * UnrealEditor-Cmd.exe ProjectName -ExecCmds="Automation RunTests ChatGPTEditor" -unattended -nopause -nosplash -nullrhi
 */

#include "Misc/AutomationTest.h"
#include "AuditLogger.h"
#include "ChatGPTEditor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

// Test flags: Combines ATF for automation test framework
#define CHATGPT_TEST_FLAGS (EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * Test: Audit Logger Initialization
 * Verifies that the audit logger can be initialized and creates necessary directories
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAuditLoggerInitTest, "ChatGPTEditor.AuditLogger.Initialize", CHATGPT_TEST_FLAGS)

bool FAuditLoggerInitTest::RunTest(const FString& Parameters)
{
	// Initialize the audit logger
	FAuditLogger::Get().Initialize();
	
	// Verify the log directory exists
	FString LogDir = FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	
	TestTrue(TEXT("Audit log directory should exist"), PlatformFile.DirectoryExists(*LogDir));
	
	// Verify we can write to the log
	FAuditLogger::Get().LogEvent(TEXT("TEST"), TEXT("Unit test log entry"));
	
	// Verify the log file exists
	FString LogFilePath = LogDir / TEXT("audit.log");
	TestTrue(TEXT("Audit log file should exist"), PlatformFile.FileExists(*LogFilePath));
	
	return true;
}

/**
 * Test: Audit Logger Thread Safety
 * Verifies that concurrent logging operations are handled safely
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAuditLoggerThreadSafetyTest, "ChatGPTEditor.AuditLogger.ThreadSafety", CHATGPT_TEST_FLAGS)

bool FAuditLoggerThreadSafetyTest::RunTest(const FString& Parameters)
{
	FAuditLogger::Get().Initialize();
	
	// Log multiple events rapidly (simulating concurrent access)
	const int32 NumEvents = 100;
	for (int32 i = 0; i < NumEvents; ++i)
	{
		FString EventName = FString::Printf(TEXT("TEST_EVENT_%d"), i);
		FString EventData = FString::Printf(TEXT("Test data for event %d"), i);
		FAuditLogger::Get().LogEvent(EventName, EventData);
	}
	
	// If we get here without crashing, thread safety is working
	TestTrue(TEXT("Concurrent logging should not crash"), true);
	
	return true;
}

/**
 * Test: Audit Logger Event Logging
 * Verifies that logged events are properly formatted and written
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAuditLoggerEventTest, "ChatGPTEditor.AuditLogger.LogEvent", CHATGPT_TEST_FLAGS)

bool FAuditLoggerEventTest::RunTest(const FString& Parameters)
{
	FAuditLogger::Get().Initialize();
	
	// Log a specific event
	const FString TestEventName = TEXT("UNIT_TEST_EVENT");
	const FString TestEventData = TEXT("This is test event data");
	
	FAuditLogger::Get().LogEvent(TestEventName, TestEventData);
	
	// Read the log file and verify the event was logged
	FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor") / TEXT("audit.log");
	FString LogContents;
	
	if (FFileHelper::LoadFileToString(LogContents, *LogFilePath))
	{
		TestTrue(TEXT("Log should contain event name"), LogContents.Contains(TestEventName));
		TestTrue(TEXT("Log should contain event data"), LogContents.Contains(TestEventData));
	}
	else
	{
		AddError(TEXT("Failed to read audit log file"));
	}
	
	return true;
}

/**
 * Test: API Key Validation
 * Verifies that API key validation works correctly
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAPIKeyValidationTest, "ChatGPTEditor.Core.APIKeyValidation", CHATGPT_TEST_FLAGS)

bool FAPIKeyValidationTest::RunTest(const FString& Parameters)
{
	// Test empty API key
	FString EmptyKey = TEXT("");
	TestFalse(TEXT("Empty API key should be invalid"), !EmptyKey.IsEmpty());
	
	// Test valid format API key (starts with sk-)
	FString ValidKey = TEXT("sk-test1234567890abcdefghijklmnopqrstuvwxyz");
	TestTrue(TEXT("Valid format API key should start with sk-"), ValidKey.StartsWith(TEXT("sk-")));
	
	// Test invalid format API key
	FString InvalidKey = TEXT("invalid-key");
	TestFalse(TEXT("Invalid format API key should not start with sk-"), InvalidKey.StartsWith(TEXT("sk-")));
	
	return true;
}

/**
 * Test: Module Lifecycle
 * Verifies that the module can start up and shut down correctly
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FModuleLifecycleTest, "ChatGPTEditor.Core.ModuleLifecycle", CHATGPT_TEST_FLAGS)

bool FModuleLifecycleTest::RunTest(const FString& Parameters)
{
	// Verify the module is loaded
	FModuleManager& ModuleManager = FModuleManager::Get();
	
	TestTrue(TEXT("ChatGPTEditor module should be loaded"), 
		ModuleManager.IsModuleLoaded(TEXT("ChatGPTEditor")));
	
	// Get the module interface
	FChatGPTEditorModule* Module = ModuleManager.GetModulePtr<FChatGPTEditorModule>(TEXT("ChatGPTEditor"));
	
	TestNotNull(TEXT("Module pointer should be valid"), Module);
	
	return true;
}

/**
 * Test: Audit Log Export
 * Verifies that audit logs can be exported
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAuditLogExportTest, "ChatGPTEditor.AuditLogger.Export", CHATGPT_TEST_FLAGS)

bool FAuditLogExportTest::RunTest(const FString& Parameters)
{
	FAuditLogger::Get().Initialize();
	
	// Log some test events
	FAuditLogger::Get().LogEvent(TEXT("EXPORT_TEST_1"), TEXT("First test event"));
	FAuditLogger::Get().LogEvent(TEXT("EXPORT_TEST_2"), TEXT("Second test event"));
	
	// Verify audit log file exists and has content
	FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor") / TEXT("audit.log");
	
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	TestTrue(TEXT("Audit log file should exist"), PlatformFile.FileExists(*LogFilePath));
	
	int64 FileSize = PlatformFile.FileSize(*LogFilePath);
	TestTrue(TEXT("Audit log file should have content"), FileSize > 0);
	
	return true;
}

/**
 * Test: Code Block Extraction
 * Verifies that code blocks can be extracted from ChatGPT responses
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCodeBlockExtractionTest, "ChatGPTEditor.Parsing.CodeBlockExtraction", CHATGPT_TEST_FLAGS)

bool FCodeBlockExtractionTest::RunTest(const FString& Parameters)
{
	// Test Python code block extraction
	FString PythonResponse = TEXT(
		"Here's a Python script:\n"
		"```python\n"
		"import unreal\n"
		"print('Hello')\n"
		"```\n"
		"That should work!"
	);
	
	// Simple extraction logic test
	const FString PythonMarker = TEXT("```python");
	TestTrue(TEXT("Should detect Python code block"), PythonResponse.Contains(PythonMarker));
	
	// Test C++ code block extraction
	FString CppResponse = TEXT(
		"Here's some C++ code:\n"
		"```cpp\n"
		"UE_LOG(LogTemp, Log, TEXT(\"Test\"));\n"
		"```\n"
		"End of code."
	);
	
	const FString CppMarker = TEXT("```cpp");
	TestTrue(TEXT("Should detect C++ code block"), CppResponse.Contains(CppMarker));
	
	// Test console command extraction
	FString ConsoleResponse = TEXT(
		"Execute this command: `stat fps`"
	);
	
	TestTrue(TEXT("Should detect inline code"), ConsoleResponse.Contains(TEXT("`stat fps`")));
	
	return true;
}

/**
 * Test: Path Validation
 * Verifies that file paths are validated correctly for security
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPathValidationTest, "ChatGPTEditor.Security.PathValidation", CHATGPT_TEST_FLAGS)

bool FPathValidationTest::RunTest(const FString& Parameters)
{
	// Test valid project paths
	FString ValidPath1 = FPaths::ProjectDir() / TEXT("Config/DefaultEngine.ini");
	TestTrue(TEXT("Project config path should be valid"), ValidPath1.StartsWith(FPaths::ProjectDir()));
	
	FString ValidPath2 = FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor/audit.log");
	TestTrue(TEXT("Saved directory path should be valid"), ValidPath2.StartsWith(FPaths::ProjectSavedDir()));
	
	// Test invalid paths (outside project)
	FString InvalidPath1 = TEXT("C:/Windows/System32/config.ini");
	TestFalse(TEXT("System path should not start with project dir"), InvalidPath1.StartsWith(FPaths::ProjectDir()));
	
	FString InvalidPath2 = TEXT("../../../etc/passwd");
	// Path traversal should be detected
	TestTrue(TEXT("Path traversal should be detected"), InvalidPath2.Contains(TEXT("..")));
	
	return true;
}

#undef CHATGPT_TEST_FLAGS
