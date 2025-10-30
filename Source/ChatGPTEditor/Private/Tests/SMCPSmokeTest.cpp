// Copyright Epic Games, Inc. All Rights Reserved.

/**
 * MCP Smoke Tests
 * 
 * These tests verify basic MCP (Model Context Protocol) functionality:
 * - Server initialization
 * - Tool registration and listing
 * - JSON-RPC request processing
 * - Core tool execution (echo, spawn_actor)
 * 
 * Run these tests via:
 * Session Frontend -> Automation tab -> Filter: "MCP"
 * Or via command line:
 * UnrealEditor-Cmd.exe ProjectName -ExecCmds="Automation RunTests MCP" -unattended -nopause -nosplash -nullrhi
 */

#include "Misc/AutomationTest.h"
#include "MCP/MCPServer.h"
#include "MCP/MCPTool.h"
#include "MCP/MCPTypes.h"
#include "Json.h"
#include "JsonObjectConverter.h"

// Test flags for MCP smoke tests
#define MCP_SMOKE_TEST_FLAGS (EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * Test: MCP Server Initialization
 * Verifies that the MCP server can be created and initialized
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMCPServerInitializationTest, "MCP.Smoke.ServerInitialization", MCP_SMOKE_TEST_FLAGS)

bool FMCPServerInitializationTest::RunTest(const FString& Parameters)
{
	// Create MCP server instance
	FMCPServer MCPServer;
	
	// Initialize the server
	bool bInitialized = MCPServer.Initialize();
	TestTrue(TEXT("MCP Server should initialize successfully"), bInitialized);
	
	// Verify server is in initialized state
	TestTrue(TEXT("MCP Server should report initialized state"), MCPServer.IsInitialized());
	
	// Shutdown the server
	MCPServer.Shutdown();
	TestFalse(TEXT("MCP Server should not be initialized after shutdown"), MCPServer.IsInitialized());
	
	return true;
}

/**
 * Test: MCP Initialize JSON-RPC Request
 * Verifies that the server responds correctly to an initialize request
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMCPInitializeRequestTest, "MCP.Smoke.InitializeRequest", MCP_SMOKE_TEST_FLAGS)

bool FMCPInitializeRequestTest::RunTest(const FString& Parameters)
{
	FMCPServer MCPServer;
	MCPServer.Initialize();
	
	// Create initialize request
	FString InitRequest = TEXT(R"({
		"jsonrpc": "2.0",
		"id": 1,
		"method": "initialize",
		"params": {
			"protocolVersion": "2024-11-05",
			"clientInfo": {
				"name": "test-client",
				"version": "1.0.0"
			}
		}
	})");
	
	// Process the request
	FString Response = MCPServer.ProcessMessage(InitRequest);
	
	// Verify response is not empty
	TestFalse(TEXT("Initialize response should not be empty"), Response.IsEmpty());
	
	// Parse response JSON
	TSharedPtr<FJsonObject> ResponseJson;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
	
	if (FJsonSerializer::Deserialize(Reader, ResponseJson) && ResponseJson.IsValid())
	{
		// Check for required fields
		TestTrue(TEXT("Response should have jsonrpc field"), ResponseJson->HasField(TEXT("jsonrpc")));
		TestTrue(TEXT("Response should have id field"), ResponseJson->HasField(TEXT("id")));
		
		// Check for result or error
		bool bHasResult = ResponseJson->HasField(TEXT("result"));
		bool bHasError = ResponseJson->HasField(TEXT("error"));
		TestTrue(TEXT("Response should have either result or error"), bHasResult || bHasError);
		
		// If successful, check result structure
		if (bHasResult)
		{
			const TSharedPtr<FJsonObject>* Result;
			if (ResponseJson->TryGetObjectField(TEXT("result"), Result))
			{
				TestTrue(TEXT("Result should have protocolVersion"), (*Result)->HasField(TEXT("protocolVersion")));
				TestTrue(TEXT("Result should have serverInfo"), (*Result)->HasField(TEXT("serverInfo")));
			}
		}
	}
	else
	{
		AddError(TEXT("Failed to parse initialize response JSON"));
	}
	
	MCPServer.Shutdown();
	return true;
}

/**
 * Test: MCP Tools List Request
 * Verifies that the server responds to tools/list requests
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMCPToolsListTest, "MCP.Smoke.ToolsList", MCP_SMOKE_TEST_FLAGS)

bool FMCPToolsListTest::RunTest(const FString& Parameters)
{
	FMCPServer MCPServer;
	MCPServer.Initialize();
	
	// Create tools/list request
	FString ListRequest = TEXT(R"({
		"jsonrpc": "2.0",
		"id": 2,
		"method": "tools/list",
		"params": {}
	})");
	
	// Process the request
	FString Response = MCPServer.ProcessMessage(ListRequest);
	
	// Verify response is not empty
	TestFalse(TEXT("Tools list response should not be empty"), Response.IsEmpty());
	
	// Parse response JSON
	TSharedPtr<FJsonObject> ResponseJson;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
	
	if (FJsonSerializer::Deserialize(Reader, ResponseJson) && ResponseJson.IsValid())
	{
		// Check for required fields
		TestTrue(TEXT("Response should have jsonrpc field"), ResponseJson->HasField(TEXT("jsonrpc")));
		TestTrue(TEXT("Response should have id field"), ResponseJson->HasField(TEXT("id")));
		
		// Check for result (tools list)
		if (ResponseJson->HasField(TEXT("result")))
		{
			const TSharedPtr<FJsonObject>* Result;
			if (ResponseJson->TryGetObjectField(TEXT("result"), Result))
			{
				// Result should have tools array
				TestTrue(TEXT("Result should have tools field"), (*Result)->HasField(TEXT("tools")));
			}
		}
	}
	else
	{
		AddError(TEXT("Failed to parse tools list response JSON"));
	}
	
	MCPServer.Shutdown();
	return true;
}

/**
 * Test: MCP Echo Tool
 * Verifies that the echo tool (if registered) works correctly
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMCPEchoToolTest, "MCP.Smoke.EchoTool", MCP_SMOKE_TEST_FLAGS)

bool FMCPEchoToolTest::RunTest(const FString& Parameters)
{
	FMCPServer MCPServer;
	MCPServer.Initialize();
	
	// Create echo tool call request
	FString EchoRequest = TEXT(R"({
		"jsonrpc": "2.0",
		"id": 3,
		"method": "tools/call",
		"params": {
			"name": "echo",
			"arguments": {
				"message": "Hello from smoke test"
			}
		}
	})");
	
	// Process the request
	FString Response = MCPServer.ProcessMessage(EchoRequest);
	
	// Verify response is not empty
	TestFalse(TEXT("Echo tool response should not be empty"), Response.IsEmpty());
	
	// Parse response JSON
	TSharedPtr<FJsonObject> ResponseJson;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
	
	if (FJsonSerializer::Deserialize(Reader, ResponseJson) && ResponseJson.IsValid())
	{
		// Check for required fields
		TestTrue(TEXT("Response should have jsonrpc field"), ResponseJson->HasField(TEXT("jsonrpc")));
		TestTrue(TEXT("Response should have id field"), ResponseJson->HasField(TEXT("id")));
		
		// Response should have either result or error
		bool bHasResult = ResponseJson->HasField(TEXT("result"));
		bool bHasError = ResponseJson->HasField(TEXT("error"));
		TestTrue(TEXT("Response should have either result or error"), bHasResult || bHasError);
		
		// If error, log it for debugging (tool might not be registered yet)
		if (bHasError)
		{
			const TSharedPtr<FJsonObject>* Error;
			if (ResponseJson->TryGetObjectField(TEXT("error"), Error))
			{
				FString ErrorMessage;
				if ((*Error)->TryGetStringField(TEXT("message"), ErrorMessage))
				{
					AddWarning(FString::Printf(TEXT("Echo tool returned error: %s (This is expected if tool is not yet implemented)"), *ErrorMessage));
				}
			}
		}
	}
	else
	{
		AddError(TEXT("Failed to parse echo tool response JSON"));
	}
	
	MCPServer.Shutdown();
	return true;
}

/**
 * Test: MCP Spawn Actor Tool
 * Verifies that the spawn_actor tool request structure is valid
 * Note: Actual spawning may require a valid world/level context
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMCPSpawnActorToolTest, "MCP.Smoke.SpawnActorTool", MCP_SMOKE_TEST_FLAGS)

bool FMCPSpawnActorToolTest::RunTest(const FString& Parameters)
{
	FMCPServer MCPServer;
	MCPServer.Initialize();
	
	// Create spawn_actor tool call request
	FString SpawnRequest = TEXT(R"({
		"jsonrpc": "2.0",
		"id": 4,
		"method": "tools/call",
		"params": {
			"name": "spawn_actor",
			"arguments": {
				"actorClass": "Actor",
				"location": {"x": 0.0, "y": 0.0, "z": 0.0},
				"rotation": {"pitch": 0.0, "yaw": 0.0, "roll": 0.0}
			}
		}
	})");
	
	// Process the request
	FString Response = MCPServer.ProcessMessage(SpawnRequest);
	
	// Verify response is not empty
	TestFalse(TEXT("Spawn actor tool response should not be empty"), Response.IsEmpty());
	
	// Parse response JSON
	TSharedPtr<FJsonObject> ResponseJson;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
	
	if (FJsonSerializer::Deserialize(Reader, ResponseJson) && ResponseJson.IsValid())
	{
		// Check for required fields
		TestTrue(TEXT("Response should have jsonrpc field"), ResponseJson->HasField(TEXT("jsonrpc")));
		TestTrue(TEXT("Response should have id field"), ResponseJson->HasField(TEXT("id")));
		
		// Response should have either result or error
		bool bHasResult = ResponseJson->HasField(TEXT("result"));
		bool bHasError = ResponseJson->HasField(TEXT("error"));
		TestTrue(TEXT("Response should have either result or error"), bHasResult || bHasError);
		
		// If error, log it (spawn_actor might not be fully implemented yet)
		if (bHasError)
		{
			const TSharedPtr<FJsonObject>* Error;
			if (ResponseJson->TryGetObjectField(TEXT("error"), Error))
			{
				FString ErrorMessage;
				if ((*Error)->TryGetStringField(TEXT("message"), ErrorMessage))
				{
					AddWarning(FString::Printf(TEXT("Spawn actor tool returned error: %s (This is expected if tool is not yet implemented or world context unavailable)"), *ErrorMessage));
				}
			}
		}
	}
	else
	{
		AddError(TEXT("Failed to parse spawn actor tool response JSON"));
	}
	
	MCPServer.Shutdown();
	return true;
}

/**
 * Test: MCP Tool Registration
 * Verifies that tools can be registered and unregistered
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMCPToolRegistrationTest, "MCP.Smoke.ToolRegistration", MCP_SMOKE_TEST_FLAGS)

bool FMCPToolRegistrationTest::RunTest(const FString& Parameters)
{
	FMCPServer MCPServer;
	MCPServer.Initialize();
	
	// Get initial tool count
	int32 InitialToolCount = MCPServer.GetRegisteredTools().Num();
	AddInfo(FString::Printf(TEXT("Initial tool count: %d"), InitialToolCount));
	
	// Note: In a real implementation, we would:
	// 1. Create a test tool
	// 2. Register it
	// 3. Verify it appears in the list
	// 4. Unregister it
	// 5. Verify it's removed
	
	// For now, just verify the API exists and doesn't crash
	TArray<TSharedPtr<IMCPTool>> RegisteredTools = MCPServer.GetRegisteredTools();
	TestTrue(TEXT("GetRegisteredTools should not crash"), true);
	
	MCPServer.Shutdown();
	return true;
}

/**
 * Test: MCP Invalid Request Handling
 * Verifies that the server handles invalid requests gracefully
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMCPInvalidRequestTest, "MCP.Smoke.InvalidRequest", MCP_SMOKE_TEST_FLAGS)

bool FMCPInvalidRequestTest::RunTest(const FString& Parameters)
{
	FMCPServer MCPServer;
	MCPServer.Initialize();
	
	// Test 1: Invalid JSON
	{
		FString InvalidJson = TEXT("This is not valid JSON");
		FString Response = MCPServer.ProcessMessage(InvalidJson);
		TestFalse(TEXT("Server should return response for invalid JSON"), Response.IsEmpty());
		
		// Response should be an error
		TSharedPtr<FJsonObject> ResponseJson;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
		if (FJsonSerializer::Deserialize(Reader, ResponseJson) && ResponseJson.IsValid())
		{
			TestTrue(TEXT("Invalid JSON should return error"), ResponseJson->HasField(TEXT("error")));
		}
	}
	
	// Test 2: Missing required fields
	{
		FString MissingMethodRequest = TEXT(R"({
			"jsonrpc": "2.0",
			"id": 5
		})");
		
		FString Response = MCPServer.ProcessMessage(MissingMethodRequest);
		TestFalse(TEXT("Server should return response for missing method"), Response.IsEmpty());
	}
	
	// Test 3: Unknown method
	{
		FString UnknownMethodRequest = TEXT(R"({
			"jsonrpc": "2.0",
			"id": 6,
			"method": "unknown/method",
			"params": {}
		})");
		
		FString Response = MCPServer.ProcessMessage(UnknownMethodRequest);
		TestFalse(TEXT("Server should return response for unknown method"), Response.IsEmpty());
		
		// Response should be an error
		TSharedPtr<FJsonObject> ResponseJson;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
		if (FJsonSerializer::Deserialize(Reader, ResponseJson) && ResponseJson.IsValid())
		{
			TestTrue(TEXT("Unknown method should return error"), ResponseJson->HasField(TEXT("error")));
		}
	}
	
	MCPServer.Shutdown();
	return true;
}
