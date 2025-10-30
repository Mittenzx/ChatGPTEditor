# MCP Implementation Guide for ChatGPTEditor

This guide provides practical implementation details for adopting the Model Context Protocol (MCP) in the ChatGPTEditor Unreal Engine plugin.

## Table of Contents

1. [Quick Start](#quick-start)
2. [MCP Server Implementation](#mcp-server-implementation)
3. [Tool Implementations](#tool-implementations)
4. [Resource Implementations](#resource-implementations)
5. [Client Integration](#client-integration)
6. [Testing Strategy](#testing-strategy)
7. [Migration Path](#migration-path)

---

## Quick Start

### Minimal MCP Server Example

Here's the simplest possible MCP server for Unreal Engine:

```cpp
// MinimalMCPServer.h
#pragma once

#include "CoreMinimal.h"
#include "Json.h"

class FMinimalMCPServer
{
public:
    FMinimalMCPServer();
    
    // Process a JSON-RPC message and return response
    FString ProcessMessage(const FString& JsonMessage);
    
private:
    TSharedPtr<FJsonObject> HandleInitialize(const TSharedPtr<FJsonObject>& Params);
    TSharedPtr<FJsonObject> HandleToolsList();
    TSharedPtr<FJsonObject> HandleToolsCall(const TSharedPtr<FJsonObject>& Params);
    
    FString ProtocolVersion;
    bool bIsInitialized;
};
```

```cpp
// MinimalMCPServer.cpp
#include "MinimalMCPServer.h"
#include "JsonUtilities.h"

FMinimalMCPServer::FMinimalMCPServer()
    : ProtocolVersion(TEXT("2025-03-26"))
    , bIsInitialized(false)
{
}

FString FMinimalMCPServer::ProcessMessage(const FString& JsonMessage)
{
    // Parse JSON-RPC request
    TSharedPtr<FJsonObject> Request;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonMessage);
    
    if (!FJsonSerializer::Deserialize(Reader, Request) || !Request.IsValid())
    {
        // Return error response
        TSharedPtr<FJsonObject> ErrorResponse = MakeShared<FJsonObject>();
        ErrorResponse->SetStringField(TEXT("jsonrpc"), TEXT("2.0"));
        ErrorResponse->SetNumberField(TEXT("id"), 0);
        
        TSharedPtr<FJsonObject> Error = MakeShared<FJsonObject>();
        Error->SetNumberField(TEXT("code"), -32700);
        Error->SetStringField(TEXT("message"), TEXT("Parse error"));
        ErrorResponse->SetObjectField(TEXT("error"), Error);
        
        FString ResponseString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
        FJsonSerializer::Serialize(ErrorResponse.ToSharedRef(), Writer);
        return ResponseString;
    }
    
    // Extract method and id
    FString Method = Request->GetStringField(TEXT("method"));
    int32 Id = static_cast<int32>(Request->GetNumberField(TEXT("id")));
    TSharedPtr<FJsonObject> Params = Request->HasField(TEXT("params")) 
        ? Request->GetObjectField(TEXT("params")) 
        : nullptr;
    
    // Handle method
    TSharedPtr<FJsonObject> Result;
    
    if (Method == TEXT("initialize"))
    {
        Result = HandleInitialize(Params);
    }
    else if (Method == TEXT("tools/list"))
    {
        Result = HandleToolsList();
    }
    else if (Method == TEXT("tools/call"))
    {
        Result = HandleToolsCall(Params);
    }
    else
    {
        // Method not found error
        TSharedPtr<FJsonObject> ErrorResponse = MakeShared<FJsonObject>();
        ErrorResponse->SetStringField(TEXT("jsonrpc"), TEXT("2.0"));
        ErrorResponse->SetNumberField(TEXT("id"), Id);
        
        TSharedPtr<FJsonObject> Error = MakeShared<FJsonObject>();
        Error->SetNumberField(TEXT("code"), -32601);
        Error->SetStringField(TEXT("message"), TEXT("Method not found"));
        ErrorResponse->SetObjectField(TEXT("error"), Error);
        
        FString ResponseString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
        FJsonSerializer::Serialize(ErrorResponse.ToSharedRef(), Writer);
        return ResponseString;
    }
    
    // Build success response
    TSharedPtr<FJsonObject> Response = MakeShared<FJsonObject>();
    Response->SetStringField(TEXT("jsonrpc"), TEXT("2.0"));
    Response->SetNumberField(TEXT("id"), Id);
    Response->SetObjectField(TEXT("result"), Result);
    
    // Serialize response
    FString ResponseString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
    FJsonSerializer::Serialize(Response.ToSharedRef(), Writer);
    
    return ResponseString;
}

TSharedPtr<FJsonObject> FMinimalMCPServer::HandleInitialize(const TSharedPtr<FJsonObject>& Params)
{
    bIsInitialized = true;
    
    TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("protocolVersion"), ProtocolVersion);
    
    // Capabilities
    TSharedPtr<FJsonObject> Capabilities = MakeShared<FJsonObject>();
    TSharedPtr<FJsonObject> Tools = MakeShared<FJsonObject>();
    Tools->SetBoolField(TEXT("listChanged"), true);
    Capabilities->SetObjectField(TEXT("tools"), Tools);
    Result->SetObjectField(TEXT("capabilities"), Capabilities);
    
    // Server info
    TSharedPtr<FJsonObject> ServerInfo = MakeShared<FJsonObject>();
    ServerInfo->SetStringField(TEXT("name"), TEXT("ChatGPTEditor-MCP"));
    ServerInfo->SetStringField(TEXT("version"), TEXT("1.0.0"));
    Result->SetObjectField(TEXT("serverInfo"), ServerInfo);
    
    return Result;
}

TSharedPtr<FJsonObject> FMinimalMCPServer::HandleToolsList()
{
    TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
    
    TArray<TSharedPtr<FJsonValue>> ToolsArray;
    
    // Define a simple "echo" tool
    TSharedPtr<FJsonObject> EchoTool = MakeShared<FJsonObject>();
    EchoTool->SetStringField(TEXT("name"), TEXT("echo"));
    EchoTool->SetStringField(TEXT("description"), TEXT("Echo back the input message"));
    
    TSharedPtr<FJsonObject> InputSchema = MakeShared<FJsonObject>();
    InputSchema->SetStringField(TEXT("type"), TEXT("object"));
    
    TSharedPtr<FJsonObject> Properties = MakeShared<FJsonObject>();
    TSharedPtr<FJsonObject> MessageProp = MakeShared<FJsonObject>();
    MessageProp->SetStringField(TEXT("type"), TEXT("string"));
    Properties->SetObjectField(TEXT("message"), MessageProp);
    InputSchema->SetObjectField(TEXT("properties"), Properties);
    
    EchoTool->SetObjectField(TEXT("inputSchema"), InputSchema);
    
    ToolsArray.Add(MakeShared<FJsonValueObject>(EchoTool));
    
    Result->SetArrayField(TEXT("tools"), ToolsArray);
    
    return Result;
}

TSharedPtr<FJsonObject> FMinimalMCPServer::HandleToolsCall(const TSharedPtr<FJsonObject>& Params)
{
    FString ToolName = Params->GetStringField(TEXT("name"));
    TSharedPtr<FJsonObject> Arguments = Params->GetObjectField(TEXT("arguments"));
    
    TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
    
    if (ToolName == TEXT("echo"))
    {
        FString Message = Arguments->GetStringField(TEXT("message"));
        
        TArray<TSharedPtr<FJsonValue>> ContentArray;
        TSharedPtr<FJsonObject> Content = MakeShared<FJsonObject>();
        Content->SetStringField(TEXT("type"), TEXT("text"));
        Content->SetStringField(TEXT("text"), FString::Printf(TEXT("Echo: %s"), *Message));
        ContentArray.Add(MakeShared<FJsonValueObject>(Content));
        
        Result->SetArrayField(TEXT("content"), ContentArray);
    }
    
    return Result;
}
```

### Testing the Minimal Server

```cpp
// In your test code or console command
void TestMCPServer()
{
    FMinimalMCPServer Server;
    
    // Test initialize
    FString InitRequest = TEXT(R"({
        "jsonrpc": "2.0",
        "id": 1,
        "method": "initialize",
        "params": {
            "protocolVersion": "2025-03-26",
            "capabilities": {},
            "clientInfo": {"name": "test-client", "version": "1.0.0"}
        }
    })");
    
    FString InitResponse = Server.ProcessMessage(InitRequest);
    UE_LOG(LogTemp, Log, TEXT("Initialize Response: %s"), *InitResponse);
    
    // Test tools/list
    FString ListRequest = TEXT(R"({
        "jsonrpc": "2.0",
        "id": 2,
        "method": "tools/list"
    })");
    
    FString ListResponse = Server.ProcessMessage(ListRequest);
    UE_LOG(LogTemp, Log, TEXT("Tools List Response: %s"), *ListResponse);
    
    // Test tools/call
    FString CallRequest = TEXT(R"({
        "jsonrpc": "2.0",
        "id": 3,
        "method": "tools/call",
        "params": {
            "name": "echo",
            "arguments": {
                "message": "Hello MCP!"
            }
        }
    })");
    
    FString CallResponse = Server.ProcessMessage(CallRequest);
    UE_LOG(LogTemp, Log, TEXT("Tool Call Response: %s"), *CallResponse);
}
```

---

## MCP Server Implementation

### Full MCP Server Architecture

```cpp
// MCPServer.h
#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

// Forward declarations
class IMCPTool;
class IMCPResource;

/**
 * Main MCP Server for Unreal Engine
 * Implements JSON-RPC 2.0 over stdio transport
 */
class CHATGPTEDITOR_API FMCPServer : public FRunnable
{
public:
    FMCPServer();
    virtual ~FMCPServer();
    
    // Server lifecycle
    bool Start();
    void Stop();
    bool IsRunning() const { return bIsRunning; }
    
    // Tool registration
    void RegisterTool(TSharedPtr<IMCPTool> Tool);
    void UnregisterTool(const FString& ToolName);
    TArray<TSharedPtr<IMCPTool>> GetRegisteredTools() const;
    
    // Resource registration
    void RegisterResource(TSharedPtr<IMCPResource> Resource);
    void UnregisterResource(const FString& Uri);
    TArray<TSharedPtr<IMCPResource>> GetRegisteredResources() const;
    
    // FRunnable interface
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;
    
protected:
    // Message processing
    FString ProcessMessage(const FString& JsonMessage);
    
    // Protocol handlers
    TSharedPtr<FJsonObject> HandleInitialize(int32 Id, const TSharedPtr<FJsonObject>& Params);
    TSharedPtr<FJsonObject> HandleToolsList(int32 Id);
    TSharedPtr<FJsonObject> HandleToolsCall(int32 Id, const TSharedPtr<FJsonObject>& Params);
    TSharedPtr<FJsonObject> HandleResourcesList(int32 Id);
    TSharedPtr<FJsonObject> HandleResourcesRead(int32 Id, const TSharedPtr<FJsonObject>& Params);
    TSharedPtr<FJsonObject> HandlePromptsList(int32 Id);
    
    // Error responses
    TSharedPtr<FJsonObject> CreateErrorResponse(int32 Id, int32 Code, const FString& Message);
    
    // Notification handlers
    void HandleNotification(const FString& Method, const TSharedPtr<FJsonObject>& Params);
    
private:
    // Registered tools and resources
    TMap<FString, TSharedPtr<IMCPTool>> RegisteredTools;
    TMap<FString, TSharedPtr<IMCPResource>> RegisteredResources;
    
    // Server state
    FString ProtocolVersion;
    bool bIsInitialized;
    bool bIsRunning;
    
    // Threading
    FRunnableThread* Thread;
    FCriticalSection RegistrationLock;
    
    // Configuration
    TSharedPtr<FJsonObject> ServerCapabilities;
    
    // Statistics
    int32 RequestsProcessed;
    int32 ErrorsEncountered;
};
```

### Tool Interface

```cpp
// MCPTool.h
#pragma once

#include "CoreMinimal.h"
#include "Json.h"

/**
 * Interface for MCP tools
 * Each tool represents an action that can be performed in Unreal Engine
 */
class IMCPTool
{
public:
    virtual ~IMCPTool() = default;
    
    // Tool metadata
    virtual FString GetName() const = 0;
    virtual FString GetDescription() const = 0;
    virtual TSharedPtr<FJsonObject> GetInputSchema() const = 0;
    
    // Execution
    virtual TSharedPtr<FJsonObject> Execute(const TSharedPtr<FJsonObject>& Arguments) = 0;
    
    // Security
    virtual bool RequiresConfirmation() const { return false; }
    virtual bool IsDangerous() const { return false; }
    virtual TArray<FString> GetRequiredPermissions() const { return TArray<FString>(); }
    
    // Optional: output schema
    virtual TSharedPtr<FJsonObject> GetOutputSchema() const { return nullptr; }
};

/**
 * Base class for MCP tools with common functionality
 */
class CHATGPTEDITOR_API FMCPToolBase : public IMCPTool
{
public:
    FMCPToolBase(const FString& InName, const FString& InDescription);
    
    virtual FString GetName() const override { return Name; }
    virtual FString GetDescription() const override { return Description; }
    
protected:
    // Helper functions
    TSharedPtr<FJsonObject> CreateSuccessResponse(const FString& Message) const;
    TSharedPtr<FJsonObject> CreateErrorResponse(const FString& ErrorMessage) const;
    TSharedPtr<FJsonObject> CreateTextContent(const FString& Text) const;
    
    FString Name;
    FString Description;
};
```

---

## Tool Implementations

### Example 1: Spawn Actor Tool

```cpp
// SpawnActorTool.h
#pragma once

#include "MCP/MCPTool.h"

class FSpawnActorTool : public FMCPToolBase
{
public:
    FSpawnActorTool();
    
    virtual TSharedPtr<FJsonObject> GetInputSchema() const override;
    virtual TSharedPtr<FJsonObject> Execute(const TSharedPtr<FJsonObject>& Arguments) override;
    
    virtual bool RequiresConfirmation() const override { return true; }
    virtual bool IsDangerous() const override { return false; }
    virtual TArray<FString> GetRequiredPermissions() const override;
};
```

```cpp
// SpawnActorTool.cpp
#include "SpawnActorTool.h"
#include "Editor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

FSpawnActorTool::FSpawnActorTool()
    : FMCPToolBase(
        TEXT("spawn_actor"),
        TEXT("Spawn one or more actors in the active Unreal Engine level")
    )
{
}

TSharedPtr<FJsonObject> FSpawnActorTool::GetInputSchema() const
{
    TSharedPtr<FJsonObject> Schema = MakeShared<FJsonObject>();
    Schema->SetStringField(TEXT("type"), TEXT("object"));
    
    TSharedPtr<FJsonObject> Properties = MakeShared<FJsonObject>();
    
    // actorClass property
    TSharedPtr<FJsonObject> ActorClassProp = MakeShared<FJsonObject>();
    ActorClassProp->SetStringField(TEXT("type"), TEXT("string"));
    ActorClassProp->SetStringField(TEXT("description"), 
        TEXT("Type of actor to spawn (PointLight, SpotLight, Camera, etc.)"));
    Properties->SetObjectField(TEXT("actorClass"), ActorClassProp);
    
    // count property
    TSharedPtr<FJsonObject> CountProp = MakeShared<FJsonObject>();
    CountProp->SetStringField(TEXT("type"), TEXT("integer"));
    CountProp->SetNumberField(TEXT("minimum"), 1);
    CountProp->SetNumberField(TEXT("maximum"), 100);
    CountProp->SetNumberField(TEXT("default"), 1);
    CountProp->SetStringField(TEXT("description"), TEXT("Number of actors to spawn"));
    Properties->SetObjectField(TEXT("count"), CountProp);
    
    // location property (optional)
    TSharedPtr<FJsonObject> LocationProp = MakeShared<FJsonObject>();
    LocationProp->SetStringField(TEXT("type"), TEXT("object"));
    LocationProp->SetStringField(TEXT("description"), TEXT("Spawn location (optional)"));
    
    TSharedPtr<FJsonObject> LocationProperties = MakeShared<FJsonObject>();
    
    TSharedPtr<FJsonObject> XProp = MakeShared<FJsonObject>();
    XProp->SetStringField(TEXT("type"), TEXT("number"));
    LocationProperties->SetObjectField(TEXT("x"), XProp);
    
    TSharedPtr<FJsonObject> YProp = MakeShared<FJsonObject>();
    YProp->SetStringField(TEXT("type"), TEXT("number"));
    LocationProperties->SetObjectField(TEXT("y"), YProp);
    
    TSharedPtr<FJsonObject> ZProp = MakeShared<FJsonObject>();
    ZProp->SetStringField(TEXT("type"), TEXT("number"));
    LocationProperties->SetObjectField(TEXT("z"), ZProp);
    
    LocationProp->SetObjectField(TEXT("properties"), LocationProperties);
    Properties->SetObjectField(TEXT("location"), LocationProp);
    
    Schema->SetObjectField(TEXT("properties"), Properties);
    
    // Required fields
    TArray<TSharedPtr<FJsonValue>> Required;
    Required.Add(MakeShared<FJsonValueString>(TEXT("actorClass")));
    Required.Add(MakeShared<FJsonValueString>(TEXT("count")));
    Schema->SetArrayField(TEXT("required"), Required);
    
    return Schema;
}

TSharedPtr<FJsonObject> FSpawnActorTool::Execute(const TSharedPtr<FJsonObject>& Arguments)
{
    // Extract parameters
    if (!Arguments->HasField(TEXT("actorClass")) || !Arguments->HasField(TEXT("count")))
    {
        return CreateErrorResponse(TEXT("Missing required parameters: actorClass and count"));
    }
    
    FString ActorClass = Arguments->GetStringField(TEXT("actorClass"));
    int32 Count = static_cast<int32>(Arguments->GetNumberField(TEXT("count")));
    
    // Validate count
    if (Count < 1 || Count > 100)
    {
        return CreateErrorResponse(TEXT("Count must be between 1 and 100"));
    }
    
    // Get location if provided
    FVector SpawnLocation = FVector::ZeroVector;
    if (Arguments->HasField(TEXT("location")))
    {
        const TSharedPtr<FJsonObject>& LocObj = Arguments->GetObjectField(TEXT("location"));
        SpawnLocation.X = LocObj->GetNumberField(TEXT("x"));
        SpawnLocation.Y = LocObj->GetNumberField(TEXT("y"));
        SpawnLocation.Z = LocObj->GetNumberField(TEXT("z"));
    }
    
    // Get world
    UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
    if (!World)
    {
        return CreateErrorResponse(TEXT("No active world found. Please open a level."));
    }
    
    // Determine actor class to spawn
    UClass* ClassToSpawn = nullptr;
    
    if (ActorClass.Contains(TEXT("PointLight"), ESearchCase::IgnoreCase))
    {
        ClassToSpawn = APointLight::StaticClass();
    }
    else if (ActorClass.Contains(TEXT("SpotLight"), ESearchCase::IgnoreCase))
    {
        ClassToSpawn = ASpotLight::StaticClass();
    }
    else if (ActorClass.Contains(TEXT("Camera"), ESearchCase::IgnoreCase))
    {
        ClassToSpawn = ACameraActor::StaticClass();
    }
    else
    {
        return CreateErrorResponse(FString::Printf(
            TEXT("Unsupported actor class: %s"), *ActorClass));
    }
    
    // Spawn actors
    TArray<FString> SpawnedActorNames;
    for (int32 i = 0; i < Count; ++i)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Name = MakeUniqueObjectName(World, ClassToSpawn);
        
        FVector ActualLocation = SpawnLocation + FVector(i * 100.0f, 0, 0);
        AActor* NewActor = World->SpawnActor<AActor>(ClassToSpawn, ActualLocation, FRotator::ZeroRotator, SpawnParams);
        
        if (NewActor)
        {
            SpawnedActorNames.Add(NewActor->GetName());
        }
    }
    
    // Create response
    TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
    
    TArray<TSharedPtr<FJsonValue>> ContentArray;
    TSharedPtr<FJsonObject> TextContent = MakeShared<FJsonObject>();
    TextContent->SetStringField(TEXT("type"), TEXT("text"));
    TextContent->SetStringField(TEXT("text"), 
        FString::Printf(TEXT("Successfully spawned %d %s actor(s)"), 
            SpawnedActorNames.Num(), *ActorClass));
    ContentArray.Add(MakeShared<FJsonValueObject>(TextContent));
    
    Result->SetArrayField(TEXT("content"), ContentArray);
    
    // Add metadata
    TSharedPtr<FJsonObject> Metadata = MakeShared<FJsonObject>();
    Metadata->SetNumberField(TEXT("spawnedCount"), SpawnedActorNames.Num());
    
    TArray<TSharedPtr<FJsonValue>> NamesArray;
    for (const FString& Name : SpawnedActorNames)
    {
        NamesArray.Add(MakeShared<FJsonValueString>(Name));
    }
    Metadata->SetArrayField(TEXT("spawnedActors"), NamesArray);
    
    Result->SetObjectField(TEXT("metadata"), Metadata);
    
    return Result;
}

TArray<FString> FSpawnActorTool::GetRequiredPermissions() const
{
    TArray<FString> Permissions;
    Permissions.Add(TEXT("scene_editing"));
    return Permissions;
}
```

### Example 2: List Actors Resource

```cpp
// ListActorsResource.h
#pragma once

#include "MCP/MCPResource.h"

class FListActorsResource : public FMCPResourceBase
{
public:
    FListActorsResource();
    
    virtual FString GetUri() const override;
    virtual FString GetName() const override;
    virtual FString GetDescription() const override;
    virtual FString GetMimeType() const override;
    
    virtual TSharedPtr<FJsonObject> Read() override;
};
```

```cpp
// ListActorsResource.cpp
#include "ListActorsResource.h"
#include "Editor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"

FListActorsResource::FListActorsResource()
    : FMCPResourceBase(
        TEXT("unreal://level/actors"),
        TEXT("Current Level Actors"),
        TEXT("List of all actors in the active level")
    )
{
}

FString FListActorsResource::GetUri() const
{
    return TEXT("unreal://level/actors");
}

FString FListActorsResource::GetName() const
{
    return TEXT("Current Level Actors");
}

FString FListActorsResource::GetDescription() const
{
    return TEXT("List of all actors in the active level with their types and locations");
}

FString FListActorsResource::GetMimeType() const
{
    return TEXT("application/json");
}

TSharedPtr<FJsonObject> FListActorsResource::Read()
{
    TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
    
    UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
    if (!World)
    {
        Result->SetBoolField(TEXT("success"), false);
        Result->SetStringField(TEXT("error"), TEXT("No active world"));
        return Result;
    }
    
    // Get all actors
    TArray<TSharedPtr<FJsonValue>> ActorsArray;
    
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Actor = *It;
        if (!Actor || Actor->IsPendingKill())
        {
            continue;
        }
        
        TSharedPtr<FJsonObject> ActorObj = MakeShared<FJsonObject>();
        ActorObj->SetStringField(TEXT("name"), Actor->GetName());
        ActorObj->SetStringField(TEXT("class"), Actor->GetClass()->GetName());
        
        FVector Location = Actor->GetActorLocation();
        TSharedPtr<FJsonObject> LocObj = MakeShared<FJsonObject>();
        LocObj->SetNumberField(TEXT("x"), Location.X);
        LocObj->SetNumberField(TEXT("y"), Location.Y);
        LocObj->SetNumberField(TEXT("z"), Location.Z);
        ActorObj->SetObjectField(TEXT("location"), LocObj);
        
        ActorsArray.Add(MakeShared<FJsonValueObject>(ActorObj));
    }
    
    Result->SetBoolField(TEXT("success"), true);
    Result->SetNumberField(TEXT("count"), ActorsArray.Num());
    Result->SetArrayField(TEXT("actors"), ActorsArray);
    
    // Create content
    TArray<TSharedPtr<FJsonValue>> ContentArray;
    TSharedPtr<FJsonObject> TextContent = MakeShared<FJsonObject>();
    TextContent->SetStringField(TEXT("type"), TEXT("text"));
    
    FString ResponseText;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseText);
    FJsonSerializer::Serialize(Result.ToSharedRef(), Writer);
    
    TextContent->SetStringField(TEXT("text"), ResponseText);
    ContentArray.Add(MakeShared<FJsonValueObject>(TextContent));
    
    TSharedPtr<FJsonObject> FinalResult = MakeShared<FJsonObject>();
    FinalResult->SetArrayField(TEXT("contents"), ContentArray);
    
    return FinalResult;
}
```

---

## Client Integration

### Connecting to External MCP Clients

For users who want to use Claude Desktop or other MCP clients with Unreal Engine:

```json
// Example: Claude Desktop configuration
// File: ~/Library/Application Support/Claude/claude_desktop_config.json (Mac)
// Or: %APPDATA%\Claude\claude_desktop_config.json (Windows)
{
  "mcpServers": {
    "unreal-engine": {
      "command": "UnrealEditor",
      "args": [
        "YourProject.uproject",
        "-ExecCmds=ChatGPT.StartMCPServer"
      ],
      "env": {
        "UNREAL_MCP_PORT": "8080"
      }
    }
  }
}
```

---

## Testing Strategy

### Unit Tests for MCP Protocol

```cpp
// MCPServerTests.cpp
#include "Misc/AutomationTest.h"
#include "MCP/MCPServer.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMCPInitializeTest, "ChatGPTEditor.MCP.Initialize", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMCPInitializeTest::RunTest(const FString& Parameters)
{
    FMCPServer Server;
    
    FString Request = TEXT(R"({
        "jsonrpc": "2.0",
        "id": 1,
        "method": "initialize",
        "params": {
            "protocolVersion": "2025-03-26"
        }
    })");
    
    FString Response = Server.ProcessMessage(Request);
    
    TestTrue(TEXT("Response should contain 'result'"), Response.Contains(TEXT("result")));
    TestTrue(TEXT("Response should contain protocol version"), 
        Response.Contains(TEXT("protocolVersion")));
    
    return true;
}
```

---

## Migration Path

### Step-by-Step Migration

**Week 1-2: POC**
1. Create `FMinimalMCPServer`
2. Implement one simple tool (echo or list actors)
3. Test with manual JSON-RPC messages
4. Validate JSON-RPC 2.0 compliance

**Week 3-4: Core Server**
1. Expand to full `FMCPServer` with all protocol methods
2. Implement tool registration system
3. Add resource support
4. Create base classes for tools and resources

**Week 5-8: Tool Migration**
1. Week 5: Spawn/Delete/Move actor tools
2. Week 6: Asset automation tools
3. Week 7: Blueprint tools
4. Week 8: Console/Python/File tools

**Week 9-10: Resources**
1. Implement level data resources
2. Implement asset information resources
3. Implement project configuration resources

**Week 11-12: UI & Integration**
1. Update `SChatGPTWindow` to use MCP
2. Add capability negotiation UI
3. Update audit logging

**Week 13-14: Testing & Documentation**
1. Comprehensive testing
2. Performance benchmarking
3. Documentation
4. Examples

**Week 15-16: Cleanup & Release**
1. Remove old code
2. Final polish
3. Release

---

## Summary

This implementation guide provides:

1. **Minimal working example** to get started quickly
2. **Full architecture** for production implementation
3. **Practical tool examples** showing real Unreal Engine operations
4. **Testing strategies** for quality assurance
5. **Clear migration path** with timeline

The MCP adoption will transform the ChatGPTEditor from a custom integration into a standards-based, extensible platform that can work with any AI model and integrate with the broader MCP ecosystem.

---

**Next Steps:**
1. Review this guide with the team
2. Build the minimal POC
3. Validate approach
4. Proceed with full implementation

**Questions or feedback?** Open an issue on GitHub or contact the maintainers.
