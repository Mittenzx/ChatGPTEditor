# ChatGPTEditor

A secure Unreal Engine 5.5 Editor plugin that integrates ChatGPT functionality directly into the Unreal Editor. This plugin adds a Slate-based Editor tab that allows you to send messages to the OpenAI Chat Completions API and receive responses in real-time.

## Features

- **Slate-based UI**: Clean, integrated Editor tab that fits naturally into the Unreal Editor workflow
- **OpenAI Integration**: Direct communication with OpenAI's Chat Completions API (GPT-3.5-turbo)
- **Security-First Design**: Permission toggles for potentially destructive operations
- **Conversation History**: Maintains context throughout your ChatGPT conversation
- **Environment-based API Keys**: Secure API key storage using environment variables
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
2. Click "Send" to submit your message to ChatGPT
3. Responses will appear in the conversation history area
4. Click "Clear" to start a new conversation

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

### Security Permissions

The plugin includes three permission toggles that are **disabled by default** for your safety:

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
- **Dependencies**: Core, CoreUObject, Engine, Slate, SlateCore, InputCore, UnrealEd, LevelEditor, HTTP, Json, JsonUtilities, AssetTools, AssetRegistry

### API Integration

- **Endpoint**: `https://api.openai.com/v1/chat/completions`
- **Model**: GPT-3.5-turbo
- **Authentication**: Bearer token (from OPENAI_API_KEY environment variable)
- **Max Tokens**: 1000 per request
- **Temperature**: 0.7

### File Structure

```
ChatGPTEditor/
├── ChatGPTEditor.uplugin                    # Plugin descriptor
├── Source/
│   └── ChatGPTEditor/
│       ├── ChatGPTEditor.Build.cs           # Build configuration
│       ├── Public/
│       │   └── ChatGPTEditor.h              # Module header
│       └── Private/
│           ├── ChatGPTEditor.cpp            # Module implementation
│           ├── SChatGPTWindow.h             # Slate window header
│           ├── SChatGPTWindow.cpp           # Slate window implementation
│           ├── AssetAutomation.h            # Asset automation parser
│           └── AssetAutomation.cpp          # Asset automation implementation
├── Saved/
│   └── ChatGPTEditor/
│       └── audit.log                        # Asset operations audit log
└── README.md                                 # This file
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

## Known Limitations

- **Asset automation requires specific command formats**: Commands must follow the documented patterns (e.g., "Create material X")
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **Asset operations create basic assets**: Created assets have default settings and may need manual configuration

## Future Enhancements

Potential features for future versions:
- Streaming response support
- Custom system prompts
- Conversation save/load functionality
- Integration with Unreal's Blueprint system
- Code generation assistance
- Asset analysis and recommendations
- Multi-model support (GPT-4, etc.)

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

### 1.0.0 (Current)
- Initial release
- Basic ChatGPT integration
- Security permission toggles (UI only)
- Conversation history
- Environment-based API key storage

---

**Remember**: This plugin is in beta. Always backup your project before use and test in a non-production environment first.