# ChatGPTEditor

A secure Unreal Engine 5.5 Editor plugin that integrates ChatGPT functionality directly into the Unreal Editor. This plugin adds a Slate-based Editor tab that allows you to send messages to the OpenAI Chat Completions API and receive responses in real-time.

## Features

- **Slate-based UI**: Clean, integrated Editor tab that fits naturally into the Unreal Editor workflow
- **OpenAI Integration**: Direct communication with OpenAI's Chat Completions API (GPT-3.5-turbo)
- **Test Automation & QA**: Generate, preview, and manage automated tests with natural language prompts
- **Project File Management**: Edit project configuration files (DefaultEngine.ini, .uproject, etc.) using natural language
- **Security-First Design**: Permission toggles for potentially destructive operations
- **Preview & Confirm**: All file changes must be previewed and explicitly confirmed before applying
- **Automatic Backups**: Creates timestamped backups before any file modifications
- **Audit Logging**: Complete operation log in `Saved/ChatGPTEditor/audit.log`
- **Level Design & Scene Editing**: Natural language-based actor spawning, movement, deletion, and modification
- **Security-First Design**: Permission toggles for potentially destructive operations
- **Preview & Confirmation**: All scene changes require explicit preview and confirmation
- **Audit Logging**: Complete tracking of all scene editing operations
- **Conversation History**: Maintains context throughout your ChatGPT conversation
- **Audit Logging**: All test generation and execution requests are logged for security and compliance
- **Environment-based API Keys**: Secure API key storage using environment variables
- **📝 Documentation Generation**: Generate or update documentation files (README, guides) from natural language prompts
- **🔍 Code Review & Explanation**: Request explanations for code, assets, or project structure
- **📋 Audit Logging**: Complete audit trail of all documentation and code review operations
- **✅ Preview & Confirm**: All documentation changes are previewed and require explicit confirmation
- **Editor Console Commands**: Execute Unreal Editor console commands via natural language (with permission gating)
- **Python Scripting**: Generate and execute Python scripts to automate Editor tasks (with preview and confirmation)
- **Audit Logging**: All console and scripting actions are logged to Saved/ChatGPTEditor/audit.log
- **Keyboard Shortcuts**: Efficient navigation and control with keyboard shortcuts
- **Accessibility Features**: Adjustable font sizes and tooltips for all interactive elements
- **Visual Feedback**: Icons and loading indicators for better user experience
- **Editor Asset Automation**: Create and modify Unreal Engine assets using natural language prompts
- **Audit Logging**: All asset operations are logged with timestamps and user information

## Installation

### Prerequisites

- Unreal Engine 5.5 or later
- Valid OpenAI API key ([Get one here](https://platform.openai.com/api-keys))

### Steps

1. **Clone or download this repository** to your Unreal Engine project's `Plugins` folder:
   ```
   YourProject/
   └── Plugins/
       └── ChatGPTEditor/
   ```

2. **Set your OpenAI API key as an environment variable**:

   **Windows (PowerShell)**:
   ```powershell
   [System.Environment]::SetEnvironmentVariable('OPENAI_API_KEY', 'your-api-key-here', 'User')
   ```
   
   **Windows (Command Prompt)**:
   ```cmd
   setx OPENAI_API_KEY "your-api-key-here"
   ```
   
   **macOS/Linux**:
   ```bash
   export OPENAI_API_KEY="your-api-key-here"
   # Add to ~/.bashrc or ~/.zshrc for persistence
   echo 'export OPENAI_API_KEY="your-api-key-here"' >> ~/.bashrc
   ```

3. **Restart Unreal Editor** (required for environment variable to take effect)

4. **Enable the plugin**:
   - Open your project in Unreal Editor
   - Go to Edit → Plugins
   - Search for "ChatGPT Editor"
   - Check the box to enable it
   - Restart the editor when prompted

5. **Access the ChatGPT window**:
   - Go to Window → ChatGPT (or find it in the Level Editor toolbar)
   - The ChatGPT tab will open as a dockable window

## Usage

### Basic Conversation

1. Type your message in the input box at the bottom
2. Click "Send" or press **Ctrl+Enter** to submit your message to ChatGPT
3. Responses will appear in the conversation history area
4. Click "Clear" or press **Ctrl+L** to start a new conversation

### Editor Asset Automation

The plugin can automatically detect and execute asset operations from ChatGPT responses. When enabled, you can use natural language to create and manage assets:

#### Supported Commands

**Creating Assets:**
- `Create material MyMaterial` - Creates a new material asset
- `Create texture MyTexture` - Creates a new texture asset
- `Create blueprint MyBlueprint` - Creates a new blueprint asset

**Managing Assets:**
- `Rename OldAssetName to NewAssetName` - Renames an existing asset
- `Delete MyAsset` - Deletes an asset (requires confirmation)

#### How It Works

1. **Enable Asset Write Permission**: Check the "Allow Asset Write Operations" checkbox
2. **Confirm the Warning**: Accept the security warning dialog
3. **Send Natural Language Commands**: Ask ChatGPT to create or modify assets
   - Example: "Create a material called M_GlowingMetal"
4. **Review the Preview**: A confirmation dialog shows what will be created/modified
5. **Confirm or Cancel**: Choose whether to proceed with the operation
6. **Check Results**: The operation is logged and feedback appears in the conversation

#### Security Features

- **Confirmation Required**: Every asset operation requires explicit user confirmation
- **Operation Preview**: See exactly what will happen before it executes
- **Audit Logging**: All operations are logged to `Saved/ChatGPTEditor/audit.log`
- **Permission Control**: Asset write must be explicitly enabled
- **Destructive Operation Warnings**: Extra warnings for delete and rename operations

#### Example Workflow

```
You: "Create a material called M_Metal and a texture called T_MetalBase"

ChatGPT: "I'll help you create those assets. Create material M_Metal. Create texture T_MetalBase."

System: Detected 2 asset operation(s) in response.
System: Processing: Create Material - M_Metal
[Confirmation Dialog Appears]
System: ✓ Successfully executed: Create Material
System: Processing: Create Texture - T_MetalBase
[Confirmation Dialog Appears]
System: ✓ Successfully executed: Create Texture
```

#### Audit Log

All asset operations are logged to `Saved/ChatGPTEditor/audit.log` with:
- Timestamp
- User name
- Operation type
- Asset name
- Success/failure status
- Additional details

Example log entry:
```
[2024-10-26 15:30:45] User: JohnDoe | Operation: Create Material | Asset: M_Metal | Success: YES | Details: Operation completed successfully
```

### Keyboard Shortcuts

The plugin supports several keyboard shortcuts for efficient workflow:

| Shortcut | Action |
|----------|--------|
| **Ctrl+Enter** | Send message to ChatGPT |
| **Ctrl+L** | Clear conversation history |
| **Ctrl+Plus** (+) | Increase font size |
| **Ctrl+Minus** (-) | Decrease font size |
| **Ctrl+0** | Reset font size to default |

### Accessibility Features

#### Adjustable Font Size
- Use the **A-** button or **Ctrl+Minus** to decrease font size
- Use the **A+** button or **Ctrl+Plus** to increase font size
- Click the font size display or press **Ctrl+0** to reset to default
- Font sizes range from 8pt to 24pt for comfortable reading

#### Tooltips
All interactive elements include helpful tooltips:
- Hover over any button, checkbox, or input field to see its description
- Security permission checkboxes show detailed warning information
- Keyboard shortcuts are displayed in tooltips for quick reference

#### Visual Indicators
- 🔒 Icons indicate security-sensitive permissions
- ⏳ Loading indicator shows when a request is being processed
- 💡 Tips and helpful information are highlighted
- ⌨️ Keyboard shortcuts reference is always visible at the bottom

### Blueprint Scripting Assistant

The plugin includes a Blueprint Scripting Assistant feature that allows you to generate and explain Blueprint logic using natural language:

#### Generating Blueprints

1. **Enable Asset Write Permission**: The "Allow Asset Write Operations" toggle must be enabled to generate Blueprints
2. **Enter your prompt**: In the "Blueprint Scripting Assistant" section, describe the Blueprint logic you want to create
   - Example: "Create a health pickup that restores 25 health points when the player overlaps it"
3. **Click "Generate Blueprint"**: The system will send your request to ChatGPT
4. **Review the preview**: A preview dialog will show you:
   - Your original request
   - A description of the generated Blueprint
   - List of nodes that will be created
   - Connections between nodes
   - A security warning
5. **Approve or Reject**: 
   - Click "Approve & Create" to proceed with Blueprint creation
   - Click "Reject" to cancel the operation
6. **All actions are logged**: Every generation request, preview, and approval/rejection is recorded in the audit log

**Security Notes:**
- All Blueprint generation requires manual preview and approval
- No code is executed without your explicit confirmation
- Requires "Allow Asset Write Operations" permission to be enabled
- All operations are logged in the audit log

#### Explaining Blueprints

1. **Enter Blueprint name**: In the Blueprint name field, type the name of a Blueprint you want explained
2. **Click "Explain Blueprint"**: The system will request an explanation from ChatGPT
3. **View explanation**: The explanation will appear in the conversation history with:
   - A summary of the Blueprint's purpose
   - Step-by-step breakdown of its logic
4. **All explanations are logged**: Every explanation request is recorded in the audit log

**Note**: This feature provides AI-generated explanations based on the Blueprint name. For actual Blueprint analysis, the Blueprint would need to be selected in the editor (future enhancement).

#### Audit Log

The plugin maintains a comprehensive audit log of all Blueprint Scripting Assistant activities:

- **What's logged**:
  - All Blueprint generation requests
  - All Blueprint explanation requests
  - All preview dialogs shown
  - All user approvals and rejections
  - Timestamps for all operations

- **Exporting the log**:
  1. Click "Export Audit Log" in the Blueprint Scripting Assistant section
  2. Choose a location to save the log file
  3. The log will be exported as a text file with all recorded activities

- **Use cases**:
  - Security compliance and auditing
  - Tracking AI-assisted modifications
  - Debugging and troubleshooting
  - Team collaboration and review

### Level Design & Scene Editing

The plugin supports natural language commands for level design and scene editing. **This feature requires the "Allow Scene Editing" permission to be enabled.**

**For detailed examples and advanced usage, see [SCENE_EDITING_EXAMPLES.md](SCENE_EDITING_EXAMPLES.md)**

#### Supported Commands

**Spawning Actors:**
- `Add 5 lights to this room` - Spawns 5 point lights
- `Add 3 point lights` - Spawns 3 point lights
- `Place a camera at PlayerStart` - Spawns a camera at the PlayerStart location
- `Add 10 spot lights` - Spawns 10 spot lights
- `Spawn a directional light` - Spawns a directional light

**Moving Actors:**
- `Move all props up by 100 units` - Moves all static mesh actors up by 100 units
- `Move all lights up by 200` - Moves all lights up by 200 units
- `Move all cameras down by 50` - Moves all cameras down by 50 units

**Deleting Actors:**
- `Delete all trigger volumes` - Deletes all trigger volumes in the level
- `Remove all lights` - Deletes all light actors
- `Delete all cameras` - Deletes all camera actors

**Modifying Properties:**
- `Change light color to red` - Changes all lights to red
- `Set light color to blue` - Changes all lights to blue
- `Change light color to green` - Changes all lights to green

#### How It Works

1. **Type a scene editing command** in the input box
2. **The plugin detects the command** automatically (no special prefix needed)
3. **A preview dialog appears** showing exactly what changes will be made
4. **Review the changes carefully** - the preview shows the operation type, affected actors, and command details
5. **Click "Confirm & Apply"** to execute the changes, or **"Cancel"** to abort
6. **Changes are applied** and logged in the audit log
7. **Use Ctrl+Z** to undo changes if needed (standard Unreal undo system)

#### Viewing the Audit Log

- Click the **"View Audit Log"** button to see all scene editing operations
- The audit log shows:
  - Timestamp of each operation
  - Command that was executed
  - Operation type (Spawn, Delete, Move, Modify)
  - Affected actors
  - Success/failure status
  - Error messages (if any)

#### Safety Features

✅ **All operations require preview and confirmation** - No changes are applied without your explicit approval

✅ **Comprehensive audit logging** - Every operation is tracked with full details

✅ **Undo support** - All changes can be undone using Unreal's standard Undo system (Ctrl+Z)

✅ **Permission-based access** - Scene editing is disabled by default and requires explicit permission

✅ **Non-destructive by default** - Critical actors (like PlayerStart, Brushes) are protected from deletion

### Editor Console Commands

The plugin can execute Unreal Editor console commands through natural language requests. This feature requires the **Allow Console Commands** permission to be enabled.

#### How to Use Console Commands

1. **Enable the permission**: Check "Allow Console Commands" in the plugin window
2. **Request a command**: Ask ChatGPT to execute a console command
   - Example: "Execute the console command `stat fps`"
   - Example: "Run the command to show FPS statistics"
3. **Review and confirm**: You'll be prompted to confirm the command before execution
4. **Check the log**: All command executions are logged to `Saved/ChatGPTEditor/audit.log`

#### Example Console Command Requests

```
User: "Execute the console command `stat fps`"
Assistant: [Detects command and prompts for confirmation]

User: "Show me the current screen percentage"
Assistant: [May suggest the command `r.screenpercentage`]

User: "Run `viewmode unlit`"
Assistant: [Extracts and executes the command with confirmation]
```

#### Safe Commands

These commands are considered safe and execute without additional warnings:
- `stat fps`, `stat unit`, `stat game`, `stat scenerendering`
- `showflag`, `viewmode`
- `r.screenpercentage`, `t.maxfps`
- `help`, `ke *` (key bindings)

#### Blacklisted Commands

The following commands are never allowed for security:
- `exit`, `quit`
- `crashdebug`, `debug crash`
- `obj delete`, `deleteall`, `destroyall`

### Python Scripting

The plugin can generate and execute Python scripts to automate Editor tasks. This feature requires the **Allow Console Commands** permission and the Python Editor Script Plugin to be enabled.

#### Prerequisites for Python Scripting

1. **Enable Python Editor Script Plugin**:
   - Go to Edit → Plugins
   - Search for "Python Editor Script Plugin"
   - Enable it and restart the editor

2. **Enable the permission**: Check "Allow Python Scripting" in the ChatGPT window

#### How to Use Python Scripting

1. **Request a script**: Ask ChatGPT to write a Python script for your task
   - Example: "Write a Python script to list all actors in the current level"
   - Example: "Generate a script to automate asset organization"
2. **Preview the script**: The script will be shown in a preview dialog
3. **Review warnings**: Any potentially dangerous operations will be flagged
4. **Confirm execution**: Approve the script to execute it
5. **Check results**: Script output appears in the Output Log
6. **Check the audit log**: All executions are logged to `Saved/ChatGPTEditor/audit.log`

#### Example Python Script Requests

```
User: "Write a Python script to list all actors in the level"
Assistant: [Generates Python script with unreal module imports]
System: [Shows preview and security warnings]
User: [Confirms execution]
System: [Executes script and shows results]
```

```
User: "Generate a script to automate creating folders in the Content Browser"
Assistant: [Generates script using unreal.EditorAssetLibrary]
System: [Warns about asset modification operations]
User: [Reviews and confirms]
```

#### Script Security

The plugin validates scripts for potentially dangerous operations:
- File system access (`os`, `sys`, `subprocess`)
- Code execution functions (`exec`, `eval`, `compile`)
- Destructive operations (`delete`, `remove`, `destroy`)

You'll be warned about these operations before execution.

### Audit Logging

All console commands and Python scripts are logged for security and debugging purposes.

**Log Location**: `[ProjectRoot]/Saved/ChatGPTEditor/audit.log`

**Log Format**:
```
[2025-10-26 15:30:45] CONSOLE_COMMAND: stat fps | Success: YES
[2025-10-26 15:31:12] PYTHON_SCRIPT: import unreal; unreal.log("Hello") | Success: YES
[2025-10-26 15:32:05] CONSOLE_COMMAND: dangerous_command | Success: NO | Error: User declined confirmation
```

**What's Logged**:
- Timestamp of execution
- Type of operation (CONSOLE_COMMAND or PYTHON_SCRIPT)
- The command or script content (truncated for long scripts)
- Success/failure status
- Error messages (if applicable)

### Project File and Config Management

With the **File I/O Operations** permission enabled, you can use natural language to read and modify project configuration files.

#### Reading Files

Simply ask ChatGPT to read a file:
```
"Read the DefaultEngine.ini file"
"Show me the contents of Config/DefaultGame.ini"
"What's in my .uproject file?"
```

ChatGPT will use the `READ_FILE:` command internally to fetch the file contents and display them in the conversation.

#### Modifying Files

Ask ChatGPT to make specific changes to configuration files:
```
"Set the default game mode to MyGameMode in DefaultEngine.ini"
"Add a new plugin dependency to the .uproject file"
"Change the max FPS to 120 in DefaultEngine.ini"
```

When ChatGPT generates a file modification:
1. A **preview dialog** will appear showing the exact changes
2. You must **explicitly confirm** before any changes are applied
3. A **backup** is automatically created (with timestamp) before writing
4. All operations are **logged** to `Saved/ChatGPTEditor/audit.log`

#### Safety Features

- **Path Validation**: Only files within your project directory can be accessed
- **Preview Required**: All changes must be previewed and confirmed
- **Automatic Backups**: Original files are backed up before modification
- **Audit Logging**: Every file operation is logged with timestamp
- **Permission Required**: File I/O permission must be explicitly enabled

#### Supported File Types

- Configuration files (`.ini`): DefaultEngine.ini, DefaultGame.ini, etc.
- Project files (`.uproject`)
- Any text-based project configuration files

#### Example Workflow

1. **Enable File I/O Permission**: Check the "Allow File I/O Operations" checkbox and confirm the warning
2. **Ask ChatGPT**: "I need to enable ray tracing in DefaultEngine.ini"
3. **Review Changes**: ChatGPT will show you the proposed changes in a preview dialog
4. **Confirm**: Click "Yes" to apply the changes or "No" to cancel
5. **Verify**: Check the audit log at `Saved/ChatGPTEditor/audit.log` for the operation record

### Documentation and Code Review Features

The plugin now supports advanced documentation and code review capabilities:

#### Code Explanation

Ask ChatGPT to explain code, classes, or project structure using natural language:

**Examples:**
- "Explain this class" (when discussing a specific class in conversation)
- "Summarize the plugin architecture"
- "What does the SChatGPTWindow class do?"
- "Review the security implementation"
- "How does the audit logging work?"

These requests are automatically logged in the audit trail but don't modify any files.

#### Documentation Generation

Request ChatGPT to generate or update documentation files:

**Examples:**
- "Generate a README for this feature"
- "Update the documentation with the new features"
- "Create API documentation"
- "Write a usage guide"

**Important:** 
- Documentation operations require the **File I/O Operations** permission to be enabled
- All proposed changes are **previewed** before being applied
- You must **explicitly confirm** each file modification
- Only documentation files (.md, .txt, .rst, .adoc) can be modified
- All operations are restricted to the plugin directory for security

#### Audit Log

Click the **"View Audit Log"** button to see a complete history of:
- All ChatGPT requests (general, code review, documentation)
- Documentation changes (proposed and applied)
- User confirmations and cancellations
- Any errors or security warnings

The audit log helps you track what operations have been performed and maintain accountability.

### Test Automation & QA

The plugin includes a powerful test automation feature that helps you generate, review, and manage automated tests:

#### Generating Tests

1. **Select Test Type**: Choose from Unit Test, Integration Test, or Functional Test in the dropdown
2. **Enter Prompt**: Describe what you want to test (e.g., "Write tests for Asset Automation")
3. **Click "Generate Test"**: The plugin will send your request to ChatGPT
4. **Review Generated Code**: A preview window will appear with the generated test code

#### Reviewing Test Code

- All generated test code is displayed in a preview window before being accepted
- The code goes through automatic security validation to check for dangerous operations
- Security warnings are displayed if any potential issues are found
- You must explicitly confirm or reject the generated code

#### Using Generated Tests

1. **Confirm the Code**: Click "Confirm" in the preview window if the code looks good
2. **Copy the Code**: Copy the test code from the conversation history
3. **Create Test File**: Create a new `.cpp` file in your project's `Tests` folder
4. **Add to Project**: Include the test file in your project's build
5. **Compile**: Rebuild your project to include the new test
6. **Run Tests**: Use Unreal Engine's Test Automation window (Window → Test Automation)

#### Audit Logging

All test automation activities are logged to maintain a security audit trail:

- **Location**: `Saved/ChatGPTEditor/audit.log` in your project directory
- **What's Logged**:
  - Test generation requests with prompts and test types
  - Test code confirmations and rejections
  - Security validation results
  - Test execution requests (when implemented)
  
The audit log helps you:
- Track what tests were generated and when
- Review security decisions
- Maintain compliance with testing policies
- Debug issues with test generation

### Security Permissions

The plugin includes four permission toggles that are **disabled by default** for your safety:

#### 🔒 Allow Asset Write Operations (DANGEROUS)
- **Default**: OFF
- **Risk**: Can modify or delete project assets
- **When to enable**: Required for Blueprint generation feature
- **Warning**: Always backup your project before enabling
- **Blueprint Assistant**: Must be enabled to generate Blueprints

#### 🔒 Allow Console Commands (DANGEROUS)
- **Default**: OFF
- **Risk**: Can execute arbitrary console commands in the editor
- **When to enable**: Only when you need ChatGPT to execute console commands
- **Warning**: Can affect editor state and project settings

#### 🔒 Allow File I/O Operations (DANGEROUS)
- **Default**: OFF
- **Risk**: Can read and write files on your system
- **When to enable**: When you need ChatGPT to generate or update documentation files
- **Warning**: Can lead to data loss or file corruption
- **Protection**: Limited to documentation files within the plugin directory
- **Risk**: Can read and write project configuration files
- **When to enable**: Only when you need ChatGPT to modify project files (e.g., DefaultEngine.ini, .uproject)
- **Warning**: Can lead to data loss or file corruption
- **Features**:
  - Read project configuration files
  - Preview all changes before applying
  - Automatic backups created before any file modifications
  - All operations logged to `Saved/ChatGPTEditor/audit.log`

#### 🔒 Allow Python Scripting (DANGEROUS)
- **Default**: OFF
- **Risk**: Can execute arbitrary Python code in the editor
- **When to enable**: Only when you need ChatGPT to generate and execute Python scripts
- **Warning**: Can modify assets, execute system commands, and affect project integrity
- **Note**: Requires the Python Editor Script Plugin to be enabled
#### 🔒 Allow Scene Editing (DANGEROUS)
- **Default**: OFF
- **Risk**: Can spawn, move, modify, or delete actors in your level
- **When to enable**: Only when you want to use natural language level design features
- **Warning**: All changes require preview and confirmation, but can still affect your level
- **Safety**: All operations are logged in the audit log and can be undone using Unreal's Undo system

**Important**: Each permission requires explicit confirmation when enabled. You will see a warning dialog explaining the risks.

**Note on Documentation**: Even with File I/O enabled, all documentation changes are previewed and require your explicit approval before being written to disk.

## Security Best Practices

### API Key Security

✅ **DO:**
- Store your API key in environment variables
- Keep your API key confidential and never commit it to version control
- Regenerate your API key if you suspect it has been compromised
- Use separate API keys for development and production

❌ **DON'T:**
- Hard-code API keys in source files
- Share API keys in public repositories
- Use the same API key across multiple projects without access controls

### Permission Management

✅ **DO:**
- Keep all permissions disabled unless actively needed
- Review ChatGPT's proposed actions before enabling permissions
- Disable permissions immediately after use
- Maintain regular backups before enabling destructive permissions
- Test in a separate project first
- Review the audit log regularly to track asset operations

❌ **DON'T:**
- Enable all permissions by default
- Leave permissions enabled when not in use
- Use on production projects without thorough testing
- Trust AI-generated code without review
- Ignore confirmation dialogs - always review what will be created/modified

### Asset Automation Security

✅ **DO:**
- Always review the confirmation dialog before proceeding with asset operations
- Check the audit log after operations to verify what was changed
- Keep backups of your project before using asset automation
- Test asset commands in a test project first
- Understand what each command will do before confirming

❌ **DON'T:**
- Blindly confirm asset operations without reading the preview
- Use asset automation on production assets without testing
- Delete the audit log - it's your record of what changed
- Ignore failed operations - check the audit log for details

### Test Automation Best Practices

✅ **DO:**
- **Always review generated test code** before adding it to your project
- **Validate test logic** to ensure it tests what you intended
- **Check for edge cases** that the AI might have missed
- **Run tests locally** before committing them to version control
- **Keep audit logs** for compliance and debugging purposes
- **Start with simple tests** and gradually increase complexity
- **Use descriptive prompts** to get better test generation results

❌ **DON'T:**
- Blindly accept generated test code without review
- Use generated tests in production without validation
- Ignore security warnings from the validation system
- Generate tests for code you don't understand
- Skip testing the generated tests themselves

### General Security

- **Review all AI-generated content** before applying it to your project
- **Use version control** (Git) to track all changes
- **Test in isolated environments** before applying changes to production projects
- **Monitor API usage** to detect unusual activity
- **Keep the plugin updated** to receive security patches
- **Review the audit log** regularly to track all operations
- **Documentation changes are previewed** - always review before confirming

### Documentation-Specific Security

✅ **DO:**
- Always preview documentation changes before applying them
- Review AI-generated documentation for accuracy and appropriateness
- Keep backups of important documentation files
- Use the audit log to track all documentation modifications
- Test documentation operations in a safe environment first

❌ **DON'T:**
- Blindly accept all documentation changes without review
- Allow File I/O permissions to remain enabled permanently
- Apply documentation changes to critical files without backups
- Ignore the audit log entries

## Technical Details

### Architecture

- **Module Type**: Editor-only module (not included in packaged builds)
- **Loading Phase**: Default
- **Dependencies**: Core, CoreUObject, Engine, Slate, SlateCore, InputCore, UnrealEd, LevelEditor, HTTP, Json, JsonUtilities, BlueprintGraph, Kismet, KismetCompiler, GraphEditor
- **Dependencies**: Core, CoreUObject, Engine, Slate, SlateCore, InputCore, UnrealEd, LevelEditor, HTTP, Json, JsonUtilities, AssetTools, AssetRegistry

### API Integration

- **Endpoint**: `https://api.openai.com/v1/chat/completions`
- **Model**: GPT-3.5-turbo
- **Authentication**: Bearer token (from OPENAI_API_KEY environment variable)
- **Max Tokens**: 1000 per request
- **Temperature**: 
  - 0.7 for general conversation
  - 0.3 for Blueprint generation (more deterministic)
  - 0.5 for Blueprint explanation (balanced)

### Blueprint Scripting Assistant Details

- **Preview System**: All Blueprint generation requests show a preview dialog before creation
- **Approval Required**: User must explicitly approve each Blueprint generation
- **Audit Logging**: All operations are logged with timestamps and details
- **Permission-Based**: Requires "Allow Asset Write Operations" to generate Blueprints
- **AI-Powered**: Uses GPT-3.5-turbo to interpret natural language and generate Blueprint descriptions
- **Safety-First**: No direct code execution; all changes require manual approval

### File Structure

```
ChatGPTEditor/
├── ChatGPTEditor.uplugin                       # Plugin descriptor
├── Source/
│   └── ChatGPTEditor/
│       ├── ChatGPTEditor.Build.cs              # Build configuration
│       ├── Public/
│       │   ├── ChatGPTEditor.h                 # Module header
│       │   ├── SceneEditingTypes.h             # Scene editing data structures
│       │   ├── AuditLogger.h                   # Audit logging header
│       │   └── SceneEditingManager.h           # Scene editing manager header
│       └── Private/
│           ├── ChatGPTEditor.cpp               # Module implementation
│           ├── SChatGPTWindow.h                # Slate window header
│           ├── SChatGPTWindow.cpp              # Slate window implementation
│           ├── SSceneEditPreviewDialog.h       # Preview dialog header
│           ├── SSceneEditPreviewDialog.cpp     # Preview dialog implementation
│           ├── AuditLogger.cpp                 # Audit logging implementation
│           └── SceneEditingManager.cpp         # Scene editing manager implementation
└── README.md                                    # This file
│           ├── ChatGPTEditor.cpp            # Module implementation
│           ├── SChatGPTWindow.h             # Slate window header
│           ├── SChatGPTWindow.cpp           # Slate window implementation
│           ├── AuditLog.h                   # Audit logging system
│           ├── DocumentationHandler.h       # Documentation operations header
│           └── DocumentationHandler.cpp     # Documentation operations implementation
│           ├── SBlueprintAssistantPanel.h   # Blueprint assistant UI header
│           ├── SBlueprintAssistantPanel.cpp # Blueprint assistant UI implementation
│           ├── BlueprintAuditLog.h          # Audit logging system header
│           └── BlueprintAuditLog.cpp        # Audit logging system implementation
│           ├── AssetAutomation.h            # Asset automation parser
│           └── AssetAutomation.cpp          # Asset automation implementation
├── Saved/
│   └── ChatGPTEditor/
│       └── audit.log                        # Asset operations audit log
└── README.md                                 # This file
```

### Documentation Features

The plugin includes a documentation generation and code review system:

- **AuditLog**: Thread-safe logging system that tracks all operations
- **DocumentationHandler**: Manages documentation file operations with security restrictions
  - Safe file path validation (plugin directory only)
  - Allowed file types: .md, .txt, .rst, .adoc
  - Path traversal protection
  - Preview generation
  - File read/write with error handling
- **Integration**: Automatically detects documentation requests and provides preview/confirmation workflow

## Troubleshooting

### "Please set the OPENAI_API_KEY environment variable"

- Ensure you've set the environment variable correctly
- Restart Unreal Editor after setting the variable
- Verify the variable is set: `echo %OPENAI_API_KEY%` (Windows) or `echo $OPENAI_API_KEY%` (macOS/Linux)

### "Failed to connect to OpenAI API"

- Check your internet connection
- Verify your API key is valid
- Check OpenAI service status at [status.openai.com](https://status.openai.com)
- Ensure your firewall isn't blocking HTTPS requests

### API Error (HTTP 401)

- Your API key is invalid or has been revoked
- Regenerate your API key from the OpenAI dashboard

### API Error (HTTP 429)

- You've exceeded your API rate limit or quota
- Check your OpenAI account usage and billing settings

### Test Generation Issues

If test generation isn't working or producing poor results:

- **Use specific prompts**: Instead of "test my code", try "Write a unit test for a function that validates asset names"
- **Specify the test type**: Make sure you've selected the appropriate test type (Unit/Integration/Functional)
- **Check security warnings**: If code is rejected, review the warnings and adjust your request
- **Review audit log**: Check `Saved/ChatGPTEditor/audit.log` for detailed error messages

## Test Automation Examples

### Example 1: Basic Unit Test

**Prompt**: "Write a unit test that validates FString concatenation"

**Test Type**: Unit Test

**Result**: Generates a simple automation test that verifies string operations work correctly.

### Example 2: Asset Validation Test

**Prompt**: "Create a test that checks if asset names follow naming conventions (prefix with T_ for textures)"

**Test Type**: Functional Test

**Result**: Generates a test that iterates through assets and validates their naming.

### Example 3: Blueprint Integration Test

**Prompt**: "Write an integration test that verifies a Blueprint can be loaded and its default values are correct"

**Test Type**: Integration Test

**Result**: Generates a test that loads a Blueprint asset and validates its properties.

### Writing Effective Test Prompts

For best results, include:
- **What to test**: The specific functionality or component
- **Expected behavior**: What should happen in the test
- **Test scope**: Unit, integration, or functional level
- **Any constraints**: Platform-specific, performance requirements, etc.

**Good prompt example**:
```
Write a unit test for a custom Actor component that manages health.
The test should verify:
- Initial health is set correctly
- TakeDamage reduces health
- Health never goes below zero
- OnHealthChanged delegate fires when health changes
```

**Poor prompt example**:
```
test health
```

## Known Limitations

- **Test execution integration**: Currently, tests must be manually copied to your project and run via Unreal's Test Automation window. Direct execution from the ChatGPT window is not yet implemented.
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **Code review required**: All generated test code must be manually reviewed for correctness and security
### Console Command Not Executing

- Ensure "Allow Console Commands" permission is enabled
- Check that the command is not blacklisted
- Verify the command syntax is correct for Unreal Engine
- Check the audit log at `Saved/ChatGPTEditor/audit.log` for error details

### Python Script Not Executing

- Verify the "Python Editor Script Plugin" is enabled (Edit → Plugins)
- Ensure "Allow Console Commands" permission is enabled
- Restart the editor after enabling the Python plugin
- Check the Output Log for Python execution errors
- Review the audit log at `Saved/ChatGPTEditor/audit.log`

## Known Limitations

- **File I/O limited to project files**: Only files within the project directory can be accessed for security
- **Asset Write operations**: UI toggle present but not yet implemented
- **Console Commands**: UI toggle present but not yet implemented
- **Python dependency**: Python scripting requires the Python Editor Script Plugin to be enabled
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **Command parsing**: Natural language command extraction may not work for all phrasings
### Scene Editing Not Working

- Ensure you have enabled the "Allow Scene Editing" permission
- Make sure you have an active level open in the editor
- Check that your command follows the supported syntax (see Usage section)
- Review the audit log for error details

### "No active world found"

- This error appears when no level is open in the editor
- Open or create a level before using scene editing commands

## Known Limitations

- **Scene editing command parsing**: Currently supports predefined patterns (see Usage section for examples)
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **Actor type detection**: Limited to common actor types (lights, cameras, static meshes, triggers)
### Asset Operation Failed

- Ensure "Allow Asset Write Operations" permission is enabled
- Check that the asset name is valid (no special characters)
- Verify the target path exists (e.g., /Game/Materials)
- Review the audit log at `Saved/ChatGPTEditor/audit.log` for details
- Make sure you confirmed the operation in the dialog

### Asset Not Found for Rename/Delete

- Ensure the asset name exactly matches an existing asset
- Check the Content Browser for the correct asset name
- Try using the full package path instead of just the name

### Keyboard Shortcuts Not Working

- Ensure the ChatGPT window has focus
- On some systems, modifier keys may differ (try Cmd instead of Ctrl on macOS)
- Check that no other plugins are intercepting the same shortcuts

## Accessibility

The ChatGPTEditor plugin is designed with accessibility in mind:

### Visual Accessibility
- **Adjustable Text Size**: Font sizes from 8pt to 24pt accommodate different visual needs
- **High Contrast**: Uses Unreal Editor's standard styling for consistent contrast
- **Visual Icons**: Emoji and symbols provide visual cues (🔒 for security, ⏳ for loading, etc.)
- **Clear Layout**: Well-organized interface with clear visual hierarchy

### Keyboard Accessibility
- **Full Keyboard Navigation**: All functions accessible via keyboard shortcuts
- **Tab Navigation**: Standard tab key navigation between UI elements
- **No Mouse Required**: Can send messages, clear history, and adjust settings without mouse

### Cognitive Accessibility
- **Tooltips**: Every interactive element has descriptive tooltip text
- **Consistent Layout**: Interface elements stay in predictable locations
- **Clear Labels**: All buttons and controls are clearly labeled
- **Status Indicators**: Visual feedback for ongoing operations

### Best Practices for Accessible Use
1. Adjust font size to your comfort level using Ctrl+/- or the accessibility buttons
2. Hover over any element to see its tooltip description
3. Use keyboard shortcuts for faster workflow without mouse dependency
4. Enable only the permissions you need for clear security status

## Known Limitations

- **Documentation parsing is basic**: The system uses pattern matching to detect documentation requests. Complex requests may not be automatically detected.
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **File operations limited**: Only documentation files (.md, .txt, .rst, .adoc) in the plugin directory can be modified
- **Manual content extraction**: For documentation generation, you may need to manually extract and format the AI's response
- **Blueprint creation is conceptual**: The current implementation shows how Blueprint generation would work with preview and approval, but actual Blueprint node creation requires deeper integration with Blueprint editor APIs
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **Blueprint explanation is AI-based**: Explanations are generated by AI based on Blueprint names, not actual Blueprint analysis
- **Asset automation requires specific command formats**: Commands must follow the documented patterns (e.g., "Create material X")
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **Asset operations create basic assets**: Created assets have default settings and may need manual configuration

## Future Enhancements

Potential features for future versions:

**Test Automation:**
- Direct test execution from the ChatGPT window
- Automated test result parsing and display
- Test suite management and organization
- Performance benchmarking integration
- Code coverage analysis

**General Features:**
- Improved documentation request parsing and extraction
- Advanced natural language parsing for more complex scene editing commands
- Integration with OpenAI GPT models for smarter command interpretation
- Batch operations and scripting support
- Full Blueprint node creation using Blueprint editor APIs
- Visual Blueprint selection for explanation (instead of name-based)
- Streaming response support
- Custom system prompts for documentation generation
- Conversation save/load functionality
- Integration with Unreal's Blueprint system
- Enhanced code review with inline comments
- Asset analysis and recommendations
- Multi-model support (GPT-4, etc.)
- Automated documentation formatting
- Diff view for documentation changes
- Advanced Blueprint refactoring suggestions
- Multi-model support (GPT-4, etc.)
- More sophisticated actor property modification
- Support for landscape and foliage editing
- Screen reader support (ARIA-like attributes)
- High-contrast theme toggle
- Customizable keyboard shortcuts

## Contributing

Contributions are welcome! Please ensure any pull requests:
- Follow Unreal Engine coding standards
- Include appropriate security warnings for new features
- Update this README with relevant documentation
- Test thoroughly before submission
- Include audit logging for any AI-assisted operations

## License

This plugin is provided as-is for educational and development purposes. Please ensure compliance with OpenAI's terms of service and usage policies when using this plugin.

## Disclaimer

⚠️ **Important**: This plugin sends data to external services (OpenAI). Be mindful of:
- What information you share in conversations
- Your organization's data privacy policies
- OpenAI's data usage policies
- Potential costs associated with API usage

The authors of this plugin are not responsible for:
- API costs incurred
- Data shared with OpenAI
- Any modifications made to your project or documentation while using this plugin
- Any consequences of enabling security permissions
- Accuracy of AI-generated documentation or code explanations

**Always maintain backups and use this plugin responsibly.**

**Documentation Safety**: While the plugin includes safeguards (preview, confirmation, path restrictions), always review documentation changes carefully before applying them. The AI may generate incorrect or inappropriate content.

## Support

For issues, questions, or suggestions:
- Open an issue on GitHub
- Review the troubleshooting section above
- Check OpenAI's documentation for API-related questions

## Version History

### 1.1.0 (Current - Test Automation Release)
- **NEW**: Test Automation & QA feature
  - Generate unit, integration, and functional tests from natural language prompts
  - Test code preview and confirmation workflow
  - Security validation for generated code
  - Audit logging for all test operations
- **NEW**: Audit log system (`Saved/ChatGPTEditor/audit.log`)
- Enhanced security with test code validation
- Comprehensive documentation and examples
- UE5.5 compatibility maintained
### 1.1.0 (Current)
- Added project file and config management
- Implemented file read/write operations with natural language
- Added preview and confirmation dialogs for all file changes
- Automatic backup creation before file modifications
- Complete audit logging system
- Path validation and security controls
- Documentation and usage examples
- Added Editor Console Command execution with natural language support
- Added Python script generation and execution
- Implemented audit logging for all console and scripting actions
- Added command whitelist/blacklist security system
- Added script validation and security warnings
- Enhanced permission system with actual functionality
- Added comprehensive documentation and usage examples
- **Level Design & Scene Editing**: Natural language-based actor spawning, movement, deletion, and modification
- **Preview & Confirmation System**: All scene changes require explicit preview and confirmation before applying
- **Audit Logging**: Complete tracking of all scene editing operations with timestamps
- **Scene Editing Permission**: New dedicated permission toggle for scene editing operations
- **Safety Features**: Protected critical actors, undo support, comprehensive error handling
- **Documentation**: Extensive usage examples and troubleshooting guide
- Added Blueprint Scripting Assistant feature
- Blueprint generation with preview and approval workflow
- Blueprint explanation functionality
- Comprehensive audit logging system
- Security-first design with manual approval required
- Export audit logs for compliance and review
- UE5.5 compatibility maintained
### 1.1.0 (Current - UX, Accessibility & Asset Automation)
### 1.1.0 (Current - UX & Accessibility Update)
- ✨ NEW: Keyboard shortcuts for common actions (Ctrl+Enter, Ctrl+L, etc.)
- ✨ NEW: Adjustable font sizes (8-24pt) with keyboard shortcuts
- ✨ NEW: Comprehensive tooltips on all interactive elements
- ✨ NEW: Visual indicators and icons for better UX
- ✨ NEW: Loading indicators for API requests
- ✨ NEW: Accessibility controls section
- ✨ NEW: Editor Asset Automation with natural language commands
- ✨ NEW: Audit logging for all asset operations
- ✨ NEW: Confirmation dialogs with preview before asset changes
- 📚 Enhanced documentation with keyboard shortcuts guide
- 📚 Asset automation usage examples and security notes
- 📚 Enhanced documentation with keyboard shortcuts guide
- 🎨 Improved visual hierarchy and spacing

### 1.0.0
- Initial release
- Basic ChatGPT integration
- Security permission toggles
- Conversation history
- Environment-based API key storage
- **Documentation generation and code review features**
- **Audit logging system**
- **Preview and confirmation workflow for file changes**
- **Code explanation requests**
- **Safe file operations restricted to plugin directory**

---

**Remember**: This plugin is in beta. Always backup your project before use and test in a non-production environment first. Review all AI-generated content before applying changes.
**Remember**: This plugin is in beta. Always backup your project before use and test in a non-production environment first.

**Security Notice for Blueprint Scripting Assistant**: All Blueprint generation requires manual preview and approval. No code is executed without your explicit confirmation. All operations are logged in the audit log for security compliance.
