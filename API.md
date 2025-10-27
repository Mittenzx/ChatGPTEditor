# API Documentation

This document provides detailed API documentation for the ChatGPTEditor plugin's public interfaces.

## Table of Contents

- [Core Interfaces](#core-interfaces)
- [Audit Logging](#audit-logging)
- [Scene Editing](#scene-editing)
- [Asset Automation](#asset-automation)
- [Blueprint Assistant](#blueprint-assistant)
- [Console and Scripting](#console-and-scripting)
- [Usage Examples](#usage-examples)

## Core Interfaces

### FChatGPTEditorModule

Main module interface for the plugin.

```cpp
class FChatGPTEditorModule : public IModuleInterface
{
public:
    /** Initialize plugin on module load */
    virtual void StartupModule() override;
    
    /** Clean up on module unload */
    virtual void ShutdownModule() override;
    
    /** Open the ChatGPT window */
    void PluginButtonClicked();
};
```

**Usage:**
```cpp
// Access the module
FChatGPTEditorModule& Module = FModuleManager::LoadModuleChecked<FChatGPTEditorModule>("ChatGPTEditor");

// Open ChatGPT window programmatically
Module.PluginButtonClicked();
```

## Audit Logging

### FAuditLogger

Singleton class for comprehensive audit logging of all operations.

```cpp
class FAuditLogger
{
public:
    /** Get singleton instance */
    static FAuditLogger& Get();
    
    /** Initialize audit logging system */
    void Initialize();
    
    /** Shutdown and cleanup */
    void Shutdown();
    
    /** Log a general event */
    void LogEvent(const FString& EventType, const FString& Message);
    
    /** Log API connection attempt */
    void LogAPIConnection(const FString& Endpoint, const FString& Method, bool bApproved);
    
    /** Log code changes */
    void LogCodeChange(const FString& Description, const FString& CodePreview, bool bApproved);
    
    /** Log file operations */
    void LogFileRead(const FString& FilePath);
    void LogFileWrite(const FString& FilePath, const FString& Operation);
    
    /** Log permission changes */
    void LogPermissionChange(const FString& PermissionName, bool bEnabled);
    
    /** Log errors */
    void LogError(const FString& Category, const FString& ErrorMessage);
    
    /** Log scene editing operations */
    void LogOperation(const FString& UserCommand, const FString& OperationType, 
                     const FString& AffectedActors, bool bSuccess, const FString& ErrorMessage);
    
    /** Get recent log entries */
    TArray<FAuditLogEntry> GetRecentEntries(int32 Count) const;
    
    /** Export entire log as string */
    FString ExportLogToString() const;
    
    /** Get path to audit log file */
    FString GetAuditLogPath() const;
    
    /** Clear the log */
    void ClearLog();
};
```

**Audit Log Entry Structure:**
```cpp
struct FAuditLogEntry
{
    FDateTime Timestamp;
    FString UserCommand;
    FString OperationType;
    FString AffectedActors;
    bool bWasSuccessful;
    FString ErrorMessage;
};
```

**Usage Examples:**

```cpp
// Initialize on plugin startup
FAuditLogger::Get().Initialize();

// Log a custom event
FAuditLogger::Get().LogEvent("CUSTOM_OPERATION", "User performed action X");

// Log with success/failure
FAuditLogger::Get().LogOperation(
    "Delete all cubes",
    "DELETE_ACTORS",
    "Cube_1, Cube_2, Cube_3",
    true,
    ""
);

// Log an error
FAuditLogger::Get().LogError("ASSET_CREATION", "Failed to create material - invalid name");

// Get recent entries for display
TArray<FAuditLogEntry> RecentEntries = FAuditLogger::Get().GetRecentEntries(10);

// Export full log
FString LogContent = FAuditLogger::Get().ExportLogToString();

// Shutdown on plugin exit
FAuditLogger::Get().Shutdown();
```

**Log File Location:**
```
YourProject/Saved/ChatGPTEditor/audit.log
```

**Log Format:**
```
[2024-10-27 10:30:45] EVENT_TYPE | Message details
[2024-10-27 10:31:12] ASSET_CREATION | Asset: MyMaterial | Status: SUCCESS
[2024-10-27 10:32:03] SCENE_EDIT | Operation: SPAWN | Actors: Cube_1 | Status: SUCCESS
```

## Scene Editing

### FSceneEditingManager

Manages scene editing operations with natural language commands.

```cpp
class FSceneEditingManager
{
public:
    /** Get singleton instance */
    static FSceneEditingManager& Get();
    
    /** Parse natural language command into actions */
    TArray<FSceneEditAction> ParseCommand(const FString& Command);
    
    /** Execute parsed actions */
    bool ExecuteActions(const TArray<FSceneEditAction>& Actions, UWorld* World, bool bPreviewOnly = false);
    
    /** Spawn actors */
    TArray<AActor*> SpawnActors(const FSceneEditAction& Action, UWorld* World);
    
    /** Delete actors */
    TArray<FString> DeleteActors(const FSceneEditAction& Action, UWorld* World);
    
    /** Move actors */
    TArray<FString> MoveActors(const FSceneEditAction& Action, UWorld* World);
    
    /** Modify actor properties */
    TArray<FString> ModifyActorProperties(const FSceneEditAction& Action, UWorld* World);
    
    /** Find actors by pattern */
    TArray<AActor*> FindActorsByPattern(const FString& Pattern, UWorld* World);
};
```

**Scene Edit Action Structure:**
```cpp
struct FSceneEditAction
{
    ESceneEditOperation Operation;  // Spawn, Delete, Move, Modify
    FString ActorClass;             // Actor type (e.g., "Cube", "Light")
    FString SearchPattern;          // Pattern for finding actors
    FVector Location;               // Target location
    int32 Count;                    // Number of actors (for spawn)
    FString PropertyName;           // Property to modify
    FString PropertyValue;          // New property value
    FString Description;            // Human-readable description
};
```

**Usage Examples:**

```cpp
// Parse a command
FString Command = "Spawn 3 cubes at 100,200,300";
TArray<FSceneEditAction> Actions = FSceneEditingManager::Get().ParseCommand(Command);

// Preview actions (dry run)
UWorld* World = GEditor->GetEditorWorldContext().World();
bool bSuccess = FSceneEditingManager::Get().ExecuteActions(Actions, World, true);

// Execute actions (actual changes)
bSuccess = FSceneEditingManager::Get().ExecuteActions(Actions, World, false);

// Spawn actors programmatically
FSceneEditAction SpawnAction;
SpawnAction.Operation = ESceneEditOperation::SpawnActor;
SpawnAction.ActorClass = "StaticMeshActor";
SpawnAction.Location = FVector(0, 0, 100);
SpawnAction.Count = 1;

TArray<AActor*> SpawnedActors = FSceneEditingManager::Get().SpawnActors(SpawnAction, World);

// Find actors
TArray<AActor*> Cubes = FSceneEditingManager::Get().FindActorsByPattern("Cube", World);
```

**Supported Operations:**

| Operation | Command Keywords | Example |
|-----------|-----------------|---------|
| Spawn | spawn, add, place | "Spawn a cube at 0,0,100" |
| Delete | delete, remove | "Delete all lights" |
| Move | move | "Move PlayerStart to 100,200,300" |
| Modify | change, set, modify | "Change light color to red" |

## Asset Automation

### Asset Creation and Management

Asset automation is handled through natural language parsing in the main window.

**Supported Asset Types:**
- Materials
- Textures  
- Blueprints
- Static Meshes
- Particle Systems
- Sound Cues
- And more...

**Command Format:**
```
Create [AssetType] [AssetName]
Rename [OldName] to [NewName]
Delete [AssetName]
```

**Examples:**

```cpp
// Via ChatGPT interface (natural language):
"Create material MyMaterial"
"Create texture MyTexture"  
"Create blueprint MyBlueprint"
"Rename OldMaterial to NewMaterial"
"Delete MyUnusedAsset"
```

**Security:**
- Requires "Allow Asset Write" permission enabled
- Shows confirmation dialog before execution
- Creates automatic backups
- Logs all operations to audit log

## Blueprint Assistant

### Blueprint Generation and Explanation

```cpp
// Request blueprint generation
"Generate a blueprint that spawns an actor when E is pressed"
"Create a blueprint for a sliding door"

// Request blueprint explanation
"Explain the logic in MyBlueprint"
"What does the PlayerController blueprint do?"
```

**Workflow:**
1. User submits natural language request
2. ChatGPT generates blueprint structure
3. Preview shown to user
4. User approves or denies
5. If approved, blueprint is created
6. Operation logged to audit

**Audit Logging:**
```cpp
// Automatic logging for all blueprint operations
// Location: YourProject/Saved/ChatGPTEditor/audit.log
```

## Console and Scripting

### Console Command Handler

```cpp
class FChatGPTConsoleHandler
{
public:
    /** Try to execute a console command from natural language */
    bool TryExecuteCommand(const FString& Response, bool bPermissionEnabled);
};
```

**Command Types:**

**Whitelisted (no confirmation):**
- `stat fps`
- `stat unit`
- `stat game`
- `help`

**Requires Confirmation:**
- Commands with "set", "r.", "t."
- Rendering and performance commands

**Blacklisted (never allowed):**
- `exit`
- `quit`
- `crash`

**Usage:**
```cpp
TSharedPtr<FChatGPTConsoleHandler> Handler = MakeShared<FChatGPTConsoleHandler>();
bool bExecuted = Handler->TryExecuteCommand(ChatGPTResponse, bAllowConsoleCommands);
```

### Python Script Handler

```cpp
class FChatGPTPythonHandler
{
public:
    /** Try to execute Python script from response */
    bool TryExecuteScript(const FString& Response, bool bPermissionEnabled);
    
    /** Validate script for security issues */
    bool ValidateScript(const FString& Script, TArray<FString>& OutWarnings);
};
```

**Forbidden Operations:**
```python
import subprocess  # System command execution
import os.system   # System access
eval()            # Dynamic code execution
exec()            # Dynamic code execution
```

**Allowed Operations:**
```python
import unreal                              # Unreal Engine API
editor_subsystem = unreal.get_editor_subsystem(...)
actors = editor_subsystem.get_all_level_actors()
```

## Usage Examples

### Complete Workflow Example

```cpp
// 1. Initialize plugin
void MyClass::InitializeChatGPT()
{
    // Audit logger is automatically initialized by the module
    FAuditLogger::Get().LogEvent("CUSTOM_INIT", "MyClass initialized ChatGPT integration");
}

// 2. Parse and execute scene command
void MyClass::ExecuteSceneCommand(const FString& Command)
{
    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World)
    {
        UE_LOG(LogChatGPTEditor, Error, TEXT("No valid world found"));
        return;
    }
    
    // Parse command
    TArray<FSceneEditAction> Actions = FSceneEditingManager::Get().ParseCommand(Command);
    
    // Preview first
    bool bPreviewSuccess = FSceneEditingManager::Get().ExecuteActions(Actions, World, true);
    
    if (bPreviewSuccess)
    {
        // Execute actual changes
        bool bSuccess = FSceneEditingManager::Get().ExecuteActions(Actions, World, false);
        
        if (bSuccess)
        {
            FAuditLogger::Get().LogEvent("SCENE_EDIT", FString::Printf(TEXT("Executed: %s"), *Command));
        }
    }
}

// 3. Custom audit logging
void MyClass::LogCustomOperation()
{
    FAuditLogger::Get().LogOperation(
        "Custom user action",
        "CUSTOM_OPERATION",
        "Affected resources",
        true,
        ""
    );
}

// 4. Export audit log
void MyClass::ExportAuditLog()
{
    FString LogContent = FAuditLogger::Get().ExportLogToString();
    
    // Save to file
    FString ExportPath = FPaths::ProjectSavedDir() / TEXT("ChatGPTEditor") / TEXT("audit_export.txt");
    FFileHelper::SaveStringToFile(LogContent, *ExportPath);
    
    UE_LOG(LogChatGPTEditor, Log, TEXT("Audit log exported to: %s"), *ExportPath);
}
```

### Thread Safety

All audit logging operations are thread-safe:

```cpp
// Safe to call from any thread
FAuditLogger::Get().LogEvent("ASYNC_OPERATION", "Background task completed");

// Internal implementation uses FScopeLock
void FAuditLogger::LogEvent(const FString& EventType, const FString& Message)
{
    FScopeLock Lock(&LogMutex);  // Thread-safe
    // ... logging implementation ...
}
```

### Error Handling Pattern

```cpp
void MyClass::SafeOperation()
{
    // Always check for null/validity
    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World)
    {
        UE_LOG(LogChatGPTEditor, Error, TEXT("Invalid world context"));
        FAuditLogger::Get().LogError("OPERATION", "Failed - no world context");
        return;
    }
    
    // Validate permissions
    if (!bAllowDestructiveOperations)
    {
        UE_LOG(LogChatGPTEditor, Warning, TEXT("Operation denied - permission not granted"));
        FAuditLogger::Get().LogEvent("PERMISSION_DENIED", "User attempted operation without permission");
        return;
    }
    
    // Perform operation with error handling
    bool bSuccess = PerformOperation();
    
    // Log result
    if (bSuccess)
    {
        FAuditLogger::Get().LogEvent("OPERATION", "Completed successfully");
    }
    else
    {
        FAuditLogger::Get().LogError("OPERATION", "Operation failed");
    }
}
```

## Best Practices

### 1. Always Use Audit Logging

```cpp
// Good
FAuditLogger::Get().LogEvent("MY_FEATURE", "Operation started");
bool bResult = DoOperation();
FAuditLogger::Get().LogEvent("MY_FEATURE", bResult ? "Success" : "Failed");

// Bad - no logging
DoOperation();  // No audit trail
```

### 2. Check Permissions Before Operations

```cpp
// Good
if (bAllowAssetWrite)
{
    CreateAsset();
}
else
{
    UE_LOG(LogChatGPTEditor, Warning, TEXT("Permission denied"));
}

// Bad - bypassing permission check
CreateAsset();  // Could be dangerous
```

### 3. Validate Input

```cpp
// Good
if (!ActorName.IsEmpty() && ActorName.Len() < 64)
{
    SpawnActor(ActorName);
}

// Bad - no validation
SpawnActor(ActorName);  // Could crash or cause issues
```

### 4. Use Descriptive Log Messages

```cpp
// Good
FAuditLogger::Get().LogError("ASSET_CREATION", 
    FString::Printf(TEXT("Failed to create material '%s': Invalid name"), *MaterialName));

// Bad - vague message
FAuditLogger::Get().LogError("ERROR", "Something failed");
```

## Log Categories

Use appropriate log categories for `UE_LOG`:

```cpp
// Main plugin category
UE_LOG(LogChatGPTEditor, Log, TEXT("General plugin message"));
UE_LOG(LogChatGPTEditor, Warning, TEXT("Warning message"));
UE_LOG(LogChatGPTEditor, Error, TEXT("Error message"));
UE_LOG(LogChatGPTEditor, Verbose, TEXT("Detailed debug info"));
```

Enable verbose logging in `DefaultEngine.ini`:
```ini
[Core.Log]
LogChatGPTEditor=Verbose
```

---

For more information, see:
- [README.md](README.md) - User documentation
- [CONTRIBUTING.md](CONTRIBUTING.md) - Development guidelines
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Common issues and solutions
