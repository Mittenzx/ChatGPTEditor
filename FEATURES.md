# ChatGPTEditor Features Showcase

This document provides detailed examples and use cases for all ChatGPTEditor plugin features.

## Table of Contents

- [Core Chat Functionality](#core-chat-functionality)
- [Blueprint Scripting Assistant](#blueprint-scripting-assistant)
- [Asset Automation](#asset-automation)
- [Scene Editing & Level Design](#scene-editing--level-design)
- [Console Commands](#console-commands)
- [Python Scripting](#python-scripting)
- [Documentation Generation](#documentation-generation)
- [External API Integration](#external-api-integration)
- [Test Automation](#test-automation)
- [Accessibility Features](#accessibility-features)

## Core Chat Functionality

### Basic Conversation

Ask ChatGPT for help with Unreal Engine development:

**Examples:**
```
You: "How do I create a blueprint that rotates an actor?"
ChatGPT: [Provides step-by-step instructions]

You: "What's the difference between Tick and Timer in Unreal Engine?"
ChatGPT: [Explains the concepts with examples]

You: "How can I optimize my level for better performance?"
ChatGPT: [Offers optimization strategies]
```

### Code Explanation

Request explanations for Unreal Engine concepts:

```
You: "Explain how the GameMode class works in Unreal Engine"
You: "What does the UFUNCTION macro do?"
You: "How does the garbage collector work in UE?"
```

## Blueprint Scripting Assistant

### Generating Blueprints

Create Blueprint logic from natural language descriptions.

#### Example 1: Simple Input Handler

**User Request:**
```
"Generate a blueprint that spawns an actor when the E key is pressed"
```

**ChatGPT Response:**
```
Here's a blueprint for spawning an actor on E key press:

1. Event: InputAction E (Pressed)
2. Node: Spawn Actor from Class
   - Class: [Select your actor class]
   - Transform: Get Actor Transform
3. Node: Print String (optional)
   - String: "Actor spawned!"
```

**Workflow:**
1. Request generates blueprint structure
2. Preview dialog shows proposed nodes
3. User approves
4. Blueprint file created in Content Browser
5. Operation logged to audit.log

#### Example 2: Timer-Based Behavior

**User Request:**
```
"Create a blueprint that damages the player every 2 seconds when they enter a trigger"
```

**ChatGPT Response:**
```
Blueprint logic:
1. Event: Actor Begin Overlap
2. Set Timer by Event (2.0 seconds, looping)
3. Custom Event: DamagePlayer
   - Get Player Character
   - Apply Damage (10.0)
```

### Explaining Blueprints

Understand existing Blueprint logic.

**User Request:**
```
"Explain the logic in my PlayerController blueprint"
```

**Workflow:**
1. Select blueprint in Content Browser
2. Submit request
3. ChatGPT analyzes structure
4. Provides detailed explanation

**Example Output:**
```
Your PlayerController blueprint contains:
1. Input handling for WASD movement
2. Mouse look with sensitivity settings
3. Sprint functionality when Shift is held
4. Interaction system on E key press
5. Health regeneration timer
```

## Asset Automation

### Creating Assets

Create Unreal Engine assets using natural language.

#### Materials

```
You: "Create material MyMetalMaterial"
System: Shows preview → You approve
Result: Material created in /Game/Materials/
```

#### Textures

```
You: "Create texture MyTexture"
Result: Texture2D asset created
```

#### Blueprints

```
You: "Create blueprint MyDoorBlueprint"
Result: Blueprint asset created
```

### Managing Assets

#### Renaming

```
You: "Rename OldMaterial to NewMaterial"
System: Shows confirmation dialog
Result: Asset renamed, references updated
```

#### Deleting

```
You: "Delete UnusedAsset"
System: Shows warning (destructive operation)
You: Confirm
Result: Asset deleted, logged to audit
```

### Security Features

- **Permission Required**: "Allow Asset Write" must be enabled
- **Confirmation Dialog**: All operations require approval
- **Automatic Backups**: Created before any modifications
- **Audit Logging**: All operations logged with timestamp

**Audit Log Example:**
```
[2024-10-27 10:30:45] ASSET_CREATION | Asset: MyMaterial | Status: SUCCESS
[2024-10-27 10:31:20] ASSET_RENAME | From: OldName To: NewName | Status: SUCCESS
[2024-10-27 10:32:15] ASSET_DELETE | Asset: UnusedAsset | Status: SUCCESS
```

## Scene Editing & Level Design

### Spawning Actors

Create actors in your level with natural language.

#### Basic Spawning

```
You: "Spawn a cube at location 0,0,100"
System: Shows preview with actor details
You: Approve
Result: StaticMeshActor (Cube) spawned at (0,0,100)
```

#### Multiple Actors

```
You: "Add 5 point lights at height 200"
Result: 5 point lights spawned at various locations, Z=200
```

#### Complex Placement

```
You: "Place a camera at the player start position"
System: Finds PlayerStart location
Result: Camera actor spawned at PlayerStart
```

### Moving Actors

Reposition actors in your level.

```
You: "Move PlayerStart to 100,200,300"
System: Shows preview
Result: PlayerStart moved to new location

You: "Move all lights 50 units up"
Result: All light actors moved +50 on Z axis
```

### Deleting Actors

Remove actors from your level.

```
You: "Delete all cubes"
System: Lists cubes to be deleted
You: Confirm
Result: All cube actors deleted

You: "Remove the DirectionalLight"
Result: Directional light removed
```

### Modifying Properties

Change actor properties.

```
You: "Change light color to red"
System: Shows property change preview
Result: Light color updated to red

You: "Set cube scale to 2"
Result: Cube scaled to 2x size
```

### Protected Actors

Some actors cannot be modified for safety:

- **PlayerStart**: Level must have spawn point
- **WorldSettings**: Critical level settings
- **Level Blueprint**: Core level logic

**Example:**
```
You: "Delete PlayerStart"
System: ERROR - PlayerStart is protected and cannot be deleted
```

### Preview System

All scene edits show a preview before applying:

```
Preview Dialog:
-----------------
Operation: SPAWN_ACTORS
Actor Type: StaticMeshActor (Cube)
Count: 3
Locations: (0,0,100), (100,0,100), (200,0,100)

[Approve] [Cancel]
```

## Console Commands

Execute Unreal Editor console commands via natural language.

### Whitelisted Commands (No Confirmation)

Safe commands that execute immediately:

```
You: "Execute console command `stat fps`"
Result: FPS stats displayed

You: "Run `stat unit`"
Result: Unit time stats displayed

You: "Show `stat memory`"
Result: Memory stats displayed
```

### Commands Requiring Confirmation

Potentially impactful commands:

```
You: "Execute `r.SetNearClipPlane 1.0`"
System: Shows confirmation dialog
You: Approve
Result: Near clip plane updated

You: "Run `t.MaxFPS 60`"
System: Confirmation required (sets value)
Result: Max FPS capped at 60
```

### Blacklisted Commands (Never Allowed)

Dangerous commands that are always blocked:

```
You: "Execute `exit`"
System: ERROR - Command is blacklisted

Blacklisted commands:
- exit
- quit
- crash
```

### Natural Language Parsing

The system understands various phrasings:

```
✓ "Execute the console command `stat fps`"
✓ "Run command: stat fps"
✓ "Show me stat unit"
✓ "Can you run `stat game`?"
```

## Python Scripting

Generate and execute Python scripts for Editor automation.

### Safe Script Example

```
You: "Write a Python script to print all actor names in the current level"

ChatGPT: 
```python
import unreal

# Get the editor subsystem
editor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
all_actors = editor_subsystem.get_all_level_actors()

# Print actor names
for actor in all_actors:
    unreal.log(f"Actor: {actor.get_name()}")
```

System: Shows preview
You: Approve
Result: Script executes, actor names logged
```

### Script with Warnings

```
You: "Create a Python script to delete all empty folders in Content"

ChatGPT:
```python
import os
import unreal

# Code to delete folders
```

System: WARNING - Script contains dangerous operations:
  - import os (file system access)
  - delete operations
  
You: Review carefully, then approve/deny
```

### Forbidden Operations

These operations are always blocked:

```python
# BLOCKED
import subprocess  # System command execution
import os.system   # Direct system access
eval()            # Dynamic code execution
exec()            # Dynamic code execution

# ALLOWED
import unreal     # Unreal Engine API
import math       # Standard library
```

## Documentation Generation

Generate or update documentation from natural language.

### Creating Documentation

```
You: "Generate a README for this plugin explaining the Blueprint Assistant feature"

ChatGPT: [Generates comprehensive README content]

System: Shows preview
You: Approve
Result: README.md created/updated
```

### Updating Documentation

```
You: "Update the troubleshooting guide with information about Python scripting errors"

System: Shows diff of changes
You: Approve
Result: TROUBLESHOOTING.md updated
```

### Code Review

```
You: "Review the SceneEditingManager class and suggest improvements"

ChatGPT: [Provides detailed code review with suggestions]
```

## External API Integration

Connect to external REST APIs and generate integration code.

### Making API Requests

```
You: "Call the weather API at https://api.weather.com/current?city=Seattle"

System: Preview Dialog
  Method: GET
  URL: https://api.weather.com/current?city=Seattle
  Headers: [None]
  
You: Approve
Result: Request sent, response logged
```

### Generated Integration Code

The system can generate C++ code for API integration:

```
You: "Generate code to integrate with the OpenWeather API"

ChatGPT: [Generates UE5 HTTP request code]

Result:
- C++ class with HTTP handling
- Request/response structures
- Error handling
- Example usage
```

### Supported HTTP Methods

- GET
- POST
- PUT
- DELETE
- PATCH

## Test Automation

Generate automated tests from natural language descriptions.

### Unit Test Example

```
You: "Generate a unit test for the AuditLogger initialization"

ChatGPT:
```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAuditLoggerInitTest,
    "ChatGPTEditor.AuditLogger.Initialize",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAuditLoggerInitTest::RunTest(const FString& Parameters)
{
    FAuditLogger::Get().Initialize();
    // Test implementation
    return true;
}
```

System: Shows preview
You: Approve
Result: Test file created
```

### Integration Test Example

```
You: "Create an integration test for scene editing"

Result: Complete test file with:
- Test setup
- Action execution
- Result validation
- Cleanup
```

### Running Tests

```
You: "Run the ChatGPTEditor tests"

System: Executes automation tests
Result: Test results displayed:
  ✓ FAuditLoggerInitTest - PASSED
  ✓ FSceneEditingTest - PASSED
  ✓ FAssetCreationTest - PASSED
```

## Accessibility Features

### Keyboard Shortcuts

Navigate and control the plugin efficiently:

| Shortcut | Action |
|----------|--------|
| Ctrl+Enter | Send message |
| Ctrl+L | Clear conversation history |
| Ctrl+Plus | Increase font size |
| Ctrl+Minus | Decrease font size |
| Ctrl+0 | Reset font size to default |

**Example Usage:**
1. Type your message
2. Press Ctrl+Enter (no need to click Send)
3. View response
4. Press Ctrl+L to start fresh conversation

### Adjustable Font Size

**Via Buttons:**
- Click [A+] to increase
- Click [A-] to decrease
- Click [Reset] for default

**Via Keyboard:**
- Ctrl+Plus to increase
- Ctrl+Minus to decrease
- Ctrl+0 to reset

**Range:** 8pt - 24pt (default: 10pt)

### Tooltips

Hover over any UI element for helpful tooltips:

- Permission checkboxes explain what they control
- Buttons describe their actions
- Input fields explain expected format

### Visual Indicators

- **Loading Spinner**: Shows when waiting for API response
- **Success Icon**: Green checkmark for successful operations
- **Error Icon**: Red X for failed operations
- **Warning Icon**: Yellow triangle for warnings

## Complete Workflow Example

### Scene Design with ChatGPT

```
1. You: "I want to create a simple test level"
   ChatGPT: "I'll help you create a test level. What elements do you need?"

2. You: "Add a floor plane, some lights, and a player start"
   System: Scene Edit Permission → Enable
   
3. You: "Spawn a floor plane at 0,0,0 with scale 10"
   System: Preview → Approve
   Result: Floor created

4. You: "Add 4 point lights at height 300 in a square pattern"
   System: Preview → Approve  
   Result: 4 lights spawned

5. You: "Change all light colors to warm white"
   System: Preview → Approve
   Result: Lights updated

6. You: "Create a material for the floor called FloorMaterial"
   System: Asset Write Permission → Enable
   System: Preview → Approve
   Result: Material created

7. You: "Generate documentation for this level setup"
   ChatGPT: [Creates level documentation]
   System: Preview → Save
   Result: Level_Documentation.md created

All operations logged to:
YourProject/Saved/ChatGPTEditor/audit.log
```

## Best Practices

### Permission Management

1. **Enable permissions only when needed**
2. **Disable after completing tasks**
3. **Review audit log regularly**

### Safe Workflows

1. **Always preview before approving**
2. **Backup your project before major changes**
3. **Test in non-production environments first**

### Efficient Usage

1. **Use keyboard shortcuts**
2. **Clear history for fresh context**
3. **Be specific in requests**
4. **Review generated code before using**

---

For more information:
- [README.md](README.md) - Installation and basic usage
- [API.md](API.md) - API documentation for developers
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Common issues and solutions
- [CONTRIBUTING.md](CONTRIBUTING.md) - How to contribute
