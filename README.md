# ChatGPTEditor

A secure Unreal Engine 5.5 Editor plugin that integrates ChatGPT functionality directly into the Unreal Editor. This plugin adds a Slate-based Editor tab that allows you to send messages to the OpenAI Chat Completions API and receive responses in real-time.

## Features

- **Slate-based UI**: Clean, integrated Editor tab that fits naturally into the Unreal Editor workflow
- **OpenAI Integration**: Direct communication with OpenAI's Chat Completions API (GPT-3.5-turbo)
- **Level Design & Scene Editing**: Natural language-based actor spawning, movement, deletion, and modification
- **Security-First Design**: Permission toggles for potentially destructive operations
- **Preview & Confirmation**: All scene changes require explicit preview and confirmation
- **Audit Logging**: Complete tracking of all scene editing operations
- **Conversation History**: Maintains context throughout your ChatGPT conversation
- **Environment-based API Keys**: Secure API key storage using environment variables

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
2. Click "Send" to submit your message to ChatGPT
3. Responses will appear in the conversation history area
4. Click "Clear" to start a new conversation

### Level Design & Scene Editing

The plugin supports natural language commands for level design and scene editing. **This feature requires the "Allow Scene Editing" permission to be enabled.**

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

### Security Permissions

The plugin includes four permission toggles that are **disabled by default** for your safety:

#### 🔒 Allow Asset Write Operations (DANGEROUS)
- **Default**: OFF
- **Risk**: Can modify or delete project assets
- **When to enable**: Only when you explicitly want ChatGPT to create or modify assets
- **Warning**: Always backup your project before enabling

#### 🔒 Allow Console Commands (DANGEROUS)
- **Default**: OFF
- **Risk**: Can execute arbitrary console commands in the editor
- **When to enable**: Only when you need ChatGPT to perform editor operations
- **Warning**: Can affect editor state and project settings

#### 🔒 Allow File I/O Operations (DANGEROUS)
- **Default**: OFF
- **Risk**: Can read and write files on your system
- **When to enable**: Only when you need ChatGPT to work with external files
- **Warning**: Can lead to data loss or file corruption

#### 🔒 Allow Scene Editing (DANGEROUS)
- **Default**: OFF
- **Risk**: Can spawn, move, modify, or delete actors in your level
- **When to enable**: Only when you want to use natural language level design features
- **Warning**: All changes require preview and confirmation, but can still affect your level
- **Safety**: All operations are logged in the audit log and can be undone using Unreal's Undo system

**Important**: Each permission requires explicit confirmation when enabled. You will see a warning dialog explaining the risks.

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

❌ **DON'T:**
- Enable all permissions by default
- Leave permissions enabled when not in use
- Use on production projects without thorough testing
- Trust AI-generated code without review

### General Security

- **Review all AI-generated code** before running it in your project
- **Use version control** (Git) to track all changes
- **Test in isolated environments** before applying changes to production projects
- **Monitor API usage** to detect unusual activity
- **Keep the plugin updated** to receive security patches

## Technical Details

### Architecture

- **Module Type**: Editor-only module (not included in packaged builds)
- **Loading Phase**: Default
- **Dependencies**: Core, CoreUObject, Engine, Slate, SlateCore, InputCore, UnrealEd, LevelEditor, HTTP, Json, JsonUtilities

### API Integration

- **Endpoint**: `https://api.openai.com/v1/chat/completions`
- **Model**: GPT-3.5-turbo
- **Authentication**: Bearer token (from OPENAI_API_KEY environment variable)
- **Max Tokens**: 1000 per request
- **Temperature**: 0.7

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
```

## Troubleshooting

### "Please set the OPENAI_API_KEY environment variable"

- Ensure you've set the environment variable correctly
- Restart Unreal Editor after setting the variable
- Verify the variable is set: `echo %OPENAI_API_KEY%` (Windows) or `echo $OPENAI_API_KEY` (macOS/Linux)

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

## Future Enhancements

Potential features for future versions:
- Advanced natural language parsing for more complex scene editing commands
- Integration with OpenAI GPT models for smarter command interpretation
- Batch operations and scripting support
- Streaming response support
- Custom system prompts
- Conversation save/load functionality
- Integration with Unreal's Blueprint system
- Code generation assistance
- Asset analysis and recommendations
- Multi-model support (GPT-4, etc.)
- More sophisticated actor property modification
- Support for landscape and foliage editing

## Contributing

Contributions are welcome! Please ensure any pull requests:
- Follow Unreal Engine coding standards
- Include appropriate security warnings for new features
- Update this README with relevant documentation
- Test thoroughly before submission

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
- Any modifications made to your project while using this plugin
- Any consequences of enabling security permissions

**Always maintain backups and use this plugin responsibly.**

## Support

For issues, questions, or suggestions:
- Open an issue on GitHub
- Review the troubleshooting section above
- Check OpenAI's documentation for API-related questions

## Version History

### 1.1.0 (Current)
- **Level Design & Scene Editing**: Natural language-based actor spawning, movement, deletion, and modification
- **Preview & Confirmation System**: All scene changes require explicit preview and confirmation before applying
- **Audit Logging**: Complete tracking of all scene editing operations with timestamps
- **Scene Editing Permission**: New dedicated permission toggle for scene editing operations
- **Safety Features**: Protected critical actors, undo support, comprehensive error handling
- **Documentation**: Extensive usage examples and troubleshooting guide

### 1.0.0
- Initial release
- Basic ChatGPT integration
- Security permission toggles
- Conversation history
- Environment-based API key storage

---

**Remember**: This plugin is in beta. Always backup your project before use and test in a non-production environment first.