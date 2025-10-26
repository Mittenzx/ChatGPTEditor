# ChatGPTEditor

A secure Unreal Engine 5.5 Editor plugin that integrates ChatGPT functionality directly into the Unreal Editor. This plugin adds a Slate-based Editor tab that allows you to send messages to the OpenAI Chat Completions API and receive responses in real-time.

## Features

- **Slate-based UI**: Clean, integrated Editor tab that fits naturally into the Unreal Editor workflow
- **OpenAI Integration**: Direct communication with OpenAI's Chat Completions API (GPT-3.5-turbo)
- **Security-First Design**: Permission toggles for potentially destructive operations
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

### Security Permissions

The plugin includes three permission toggles that are **disabled by default** for your safety:

#### 🔒 Allow Asset Write Operations (DANGEROUS)
- **Default**: OFF
- **Risk**: Can modify or delete project assets
- **When to enable**: Required for Blueprint generation feature
- **Warning**: Always backup your project before enabling
- **Blueprint Assistant**: Must be enabled to generate Blueprints

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
- **Dependencies**: Core, CoreUObject, Engine, Slate, SlateCore, InputCore, UnrealEd, LevelEditor, HTTP, Json, JsonUtilities, BlueprintGraph, Kismet, KismetCompiler, GraphEditor

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
│           ├── SBlueprintAssistantPanel.h   # Blueprint assistant UI header
│           ├── SBlueprintAssistantPanel.cpp # Blueprint assistant UI implementation
│           ├── BlueprintAuditLog.h          # Audit logging system header
│           └── BlueprintAuditLog.cpp        # Audit logging system implementation
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

## Known Limitations

- **Blueprint creation is conceptual**: The current implementation shows how Blueprint generation would work with preview and approval, but actual Blueprint node creation requires deeper integration with Blueprint editor APIs
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **Blueprint explanation is AI-based**: Explanations are generated by AI based on Blueprint names, not actual Blueprint analysis

## Future Enhancements

Potential features for future versions:
- Full Blueprint node creation using Blueprint editor APIs
- Visual Blueprint selection for explanation (instead of name-based)
- Streaming response support
- Custom system prompts
- Conversation save/load functionality
- Advanced Blueprint refactoring suggestions
- Multi-model support (GPT-4, etc.)

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
- Added Blueprint Scripting Assistant feature
- Blueprint generation with preview and approval workflow
- Blueprint explanation functionality
- Comprehensive audit logging system
- Security-first design with manual approval required
- Export audit logs for compliance and review
- UE5.5 compatibility maintained

### 1.0.0
- Initial release
- Basic ChatGPT integration
- Security permission toggles
- Conversation history
- Environment-based API key storage

---

**Remember**: This plugin is in beta. Always backup your project before use and test in a non-production environment first.

**Security Notice for Blueprint Scripting Assistant**: All Blueprint generation requires manual preview and approval. No code is executed without your explicit confirmation. All operations are logged in the audit log for security compliance.