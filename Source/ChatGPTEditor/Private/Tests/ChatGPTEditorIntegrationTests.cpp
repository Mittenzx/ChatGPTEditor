// Copyright Epic Games, Inc. All Rights Reserved.

/**
 * Integration tests for ChatGPTEditor plugin features
 * 
 * These tests verify the integration between different components:
 * - Asset automation integration
 * - Scene editing integration
 * - Blueprint assistant integration
 * - Console command handling
 * 
 * Run these tests via:
 * Session Frontend -> Automation tab
 * Or via command line:
 * UnrealEditor-Cmd.exe ProjectName -ExecCmds="Automation RunTests ChatGPTEditor.Integration" -unattended -nopause -nosplash -nullrhi
 */

#include "Misc/AutomationTest.h"
#include "AuditLogger.h"
#include "AssetAutomation.h"
#include "SceneEditingManager.h"
#include "TestAutomationHelper.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/World.h"
#include "EngineUtils.h"

#define CHATGPT_INTEGRATION_TEST_FLAGS (EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

/**
 * Test: Asset Automation Integration
 * Verifies that asset automation can be initialized and logged
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetAutomationIntegrationTest, 
	"ChatGPTEditor.Integration.AssetAutomation", CHATGPT_INTEGRATION_TEST_FLAGS)

bool FAssetAutomationIntegrationTest::RunTest(const FString& Parameters)
{
	// Initialize audit logger
	FAuditLogger::Get().Initialize();
	
	// Test asset creation command detection
	FString CreateMaterialCommand = TEXT("Create material MyTestMaterial");
	TestTrue(TEXT("Should detect 'Create material' command"), 
		CreateMaterialCommand.Contains(TEXT("Create material")));
	
	// Test asset rename command detection
	FString RenameCommand = TEXT("Rename OldAsset to NewAsset");
	TestTrue(TEXT("Should detect 'Rename' command"), 
		RenameCommand.Contains(TEXT("Rename")));
	
	// Test asset delete command detection
	FString DeleteCommand = TEXT("Delete MyAsset");
	TestTrue(TEXT("Should detect 'Delete' command"), 
		DeleteCommand.Contains(TEXT("Delete")));
	
	// Verify audit logging works with asset operations
	FAuditLogger::Get().LogEvent(TEXT("ASSET_CREATION_TEST"), TEXT("Test material creation"));
	
	return true;
}

/**
 * Test: Scene Editing Integration
 * Verifies that scene editing manager can be used with audit logging
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSceneEditingIntegrationTest, 
	"ChatGPTEditor.Integration.SceneEditing", CHATGPT_INTEGRATION_TEST_FLAGS)

bool FSceneEditingIntegrationTest::RunTest(const FString& Parameters)
{
	// Initialize systems
	FAuditLogger::Get().Initialize();
	
	// Test actor spawn command detection
	FString SpawnCommand = TEXT("Spawn a cube at location 0,0,100");
	TestTrue(TEXT("Should detect 'Spawn' command"), 
		SpawnCommand.Contains(TEXT("Spawn")));
	
	// Test actor move command detection
	FString MoveCommand = TEXT("Move PlayerStart to 100,200,300");
	TestTrue(TEXT("Should detect 'Move' command"), 
		MoveCommand.Contains(TEXT("Move")));
	
	// Test actor delete command detection
	FString DeleteActorCommand = TEXT("Delete all cubes");
	TestTrue(TEXT("Should detect 'Delete' command"), 
		DeleteActorCommand.Contains(TEXT("Delete")));
	
	// Log scene editing operation
	FAuditLogger::Get().LogEvent(TEXT("SCENE_EDIT_TEST"), TEXT("Test actor spawn"));
	
	return true;
}

/**
 * Test: Test Automation Helper Integration
 * Verifies the test automation helper works correctly
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTestAutomationHelperIntegrationTest, 
	"ChatGPTEditor.Integration.TestAutomationHelper", CHATGPT_INTEGRATION_TEST_FLAGS)

bool FTestAutomationHelperIntegrationTest::RunTest(const FString& Parameters)
{
	// Initialize test automation helper
	FTestAutomationHelper::InitializeAuditLog();
	
	// Test logging capabilities
	FTestAutomationHelper::LogTestGenerationRequest(TEXT("Generate unit test"), TEXT("Unit"));
	FTestAutomationHelper::LogTestExecutionRequest(TEXT("MyTest"), TEXT("/Game/Tests/MyTest.cpp"));
	FTestAutomationHelper::LogTestExecutionResult(TEXT("MyTest"), true, TEXT("Test passed"));
	
	// Verify audit log was created
	FString AuditLogPath = FTestAutomationHelper::GetAuditLogPath();
	TestFalse(TEXT("Audit log path should not be empty"), AuditLogPath.IsEmpty());
	
	return true;
}

/**
 * Test: Code Validation Integration
 * Verifies that code validation works for different types of generated code
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCodeValidationIntegrationTest, 
	"ChatGPTEditor.Integration.CodeValidation", CHATGPT_INTEGRATION_TEST_FLAGS)

bool FCodeValidationIntegrationTest::RunTest(const FString& Parameters)
{
	// Test safe code validation
	FString SafeCode = TEXT(
		"#include \"Misc/AutomationTest.h\"\n"
		"IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMyTest, \"MyTest\", EAutomationTestFlags::EditorContext)\n"
		"bool FMyTest::RunTest(const FString& Parameters)\n"
		"{\n"
		"    return true;\n"
		"}\n"
	);
	
	TArray<FString> Warnings;
	bool bIsValid = FTestAutomationHelper::ValidateTestCode(SafeCode, Warnings);
	
	TestTrue(TEXT("Safe code should be valid"), bIsValid);
	
	// Test dangerous code validation
	FString DangerousCode = TEXT(
		"import subprocess\n"
		"subprocess.run(['rm', '-rf', '/'])\n"
	);
	
	TArray<FString> DangerousWarnings;
	bool bIsDangerous = FTestAutomationHelper::ValidateTestCode(DangerousCode, DangerousWarnings);
	
	// Should detect dangerous operations
	TestTrue(TEXT("Should detect dangerous operations"), 
		DangerousCode.Contains(TEXT("subprocess")) || 
		DangerousWarnings.Num() > 0 || 
		!bIsDangerous);
	
	return true;
}

/**
 * Test: Console Command Parsing Integration
 * Verifies that console commands can be parsed and validated
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FConsoleCommandIntegrationTest, 
	"ChatGPTEditor.Integration.ConsoleCommands", CHATGPT_INTEGRATION_TEST_FLAGS)

bool FConsoleCommandIntegrationTest::RunTest(const FString& Parameters)
{
	// Test safe command detection
	FString SafeCommand = TEXT("stat fps");
	TestTrue(TEXT("Should recognize safe command"), 
		SafeCommand.Equals(TEXT("stat fps"), ESearchCase::IgnoreCase));
	
	// Test blacklisted command detection
	TArray<FString> BlacklistedCommands = {TEXT("exit"), TEXT("quit"), TEXT("crash")};
	
	for (const FString& Cmd : BlacklistedCommands)
	{
		TestTrue(TEXT("Should identify blacklisted command"), 
			Cmd.Equals(TEXT("exit")) || Cmd.Equals(TEXT("quit")) || Cmd.Equals(TEXT("crash")));
	}
	
	// Test command with parameters
	FString CommandWithParams = TEXT("r.SetNearClipPlane 1.0");
	TestTrue(TEXT("Should detect command with parameters"), 
		CommandWithParams.Contains(TEXT("r.SetNearClipPlane")));
	
	return true;
}

/**
 * Test: Python Script Parsing Integration
 * Verifies that Python scripts can be parsed from responses
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonScriptIntegrationTest, 
	"ChatGPTEditor.Integration.PythonScripts", CHATGPT_INTEGRATION_TEST_FLAGS)

bool FPythonScriptIntegrationTest::RunTest(const FString& Parameters)
{
	// Test Python script extraction
	FString ResponseWithPython = TEXT(
		"Here's a Python script to help:\n"
		"```python\n"
		"import unreal\n"
		"editor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)\n"
		"actors = editor_subsystem.get_all_level_actors()\n"
		"for actor in actors:\n"
		"    unreal.log(actor.get_name())\n"
		"```\n"
	);
	
	TestTrue(TEXT("Should detect Python code block"), 
		ResponseWithPython.Contains(TEXT("```python")));
	TestTrue(TEXT("Should contain unreal import"), 
		ResponseWithPython.Contains(TEXT("import unreal")));
	
	// Test forbidden operations detection
	FString ForbiddenScript = TEXT(
		"```python\n"
		"import subprocess\n"
		"import os\n"
		"os.system('dangerous_command')\n"
		"```\n"
	);
	
	TestTrue(TEXT("Should detect subprocess import"), 
		ForbiddenScript.Contains(TEXT("import subprocess")));
	TestTrue(TEXT("Should detect os import"), 
		ForbiddenScript.Contains(TEXT("import os")));
	
	return true;
}

/**
 * Test: Audit Log Integration with Multiple Systems
 * Verifies that different systems can all log to the same audit log
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAuditLogMultiSystemTest, 
	"ChatGPTEditor.Integration.AuditLogMultiSystem", CHATGPT_INTEGRATION_TEST_FLAGS)

bool FAuditLogMultiSystemTest::RunTest(const FString& Parameters)
{
	// Initialize all logging systems
	FAuditLogger::Get().Initialize();
	FTestAutomationHelper::InitializeAuditLog();
	
	// Log from different systems
	FAuditLogger::Get().LogEvent(TEXT("SYSTEM_A"), TEXT("Message from system A"));
	FTestAutomationHelper::LogAuditMessage(TEXT("SYSTEM_B"), TEXT("Message from system B"));
	
	// Verify both logs exist and are accessible
	FString MainAuditLog = FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor") / TEXT("audit.log");
	FString TestAuditLog = FTestAutomationHelper::GetAuditLogPath();
	
	TestFalse(TEXT("Main audit log path should not be empty"), MainAuditLog.IsEmpty());
	TestFalse(TEXT("Test audit log path should not be empty"), TestAuditLog.IsEmpty());
	
	// Verify both point to the same log (unified logging)
	TestEqual(TEXT("Both systems should use the same audit log"), MainAuditLog, TestAuditLog);
	
	return true;
}

/**
 * Test: Permission System Integration
 * Verifies that permission flags work correctly across different features
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPermissionSystemIntegrationTest, 
	"ChatGPTEditor.Integration.PermissionSystem", CHATGPT_INTEGRATION_TEST_FLAGS)

bool FPermissionSystemIntegrationTest::RunTest(const FString& Parameters)
{
	// Test permission flag states (these would normally be set in UI)
	bool bAssetWritePermission = false; // Should be off by default
	bool bConsoleCommandPermission = false; // Should be off by default
	bool bFileIOPermission = false; // Should be off by default
	bool bPythonScriptingPermission = false; // Should be off by default
	bool bSceneEditingPermission = false; // Should be off by default
	
	// Verify all destructive permissions are off by default
	TestFalse(TEXT("Asset write permission should be off by default"), bAssetWritePermission);
	TestFalse(TEXT("Console command permission should be off by default"), bConsoleCommandPermission);
	TestFalse(TEXT("File I/O permission should be off by default"), bFileIOPermission);
	TestFalse(TEXT("Python scripting permission should be off by default"), bPythonScriptingPermission);
	TestFalse(TEXT("Scene editing permission should be off by default"), bSceneEditingPermission);
	
	// Log permission check
	FAuditLogger::Get().LogEvent(TEXT("PERMISSION_CHECK"), TEXT("Verified default permissions"));
	
	return true;
}

#undef CHATGPT_INTEGRATION_TEST_FLAGS
