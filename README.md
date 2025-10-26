# ChatGPTEditor

A secure Unreal Engine 5.5 Editor plugin that integrates ChatGPT functionality directly into the Unreal Editor. This plugin adds a Slate-based Editor tab that allows you to send messages to the OpenAI Chat Completions API and receive responses in real-time.

## Features

- **Slate-based UI**: Clean, integrated Editor tab that fits naturally into the Unreal Editor workflow
- **OpenAI Integration**: Direct communication with OpenAI's Chat Completions API (GPT-3.5-turbo)
- **Security-First Design**: Permission toggles for potentially destructive operations
- **Conversation History**: Maintains context throughout your ChatGPT conversation
- **Environment-based API Keys**: Secure API key storage using environment variables
- **Editor Console Commands**: Execute Unreal Editor console commands via natural language (with permission gating)
- **Python Scripting**: Generate and execute Python scripts to automate Editor tasks (with preview and confirmation)
- **Audit Logging**: All console and scripting actions are logged to Saved/ChatGPTEditor/audit.log

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

2. **Enable the permission**: Check "Allow Console Commands" in the ChatGPT window

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
├── ChatGPTEditor.uplugin                    # Plugin descriptor
├── Source/
│   └── ChatGPTEditor/
│       ├── ChatGPTEditor.Build.cs           # Build configuration
│       ├── Public/
│       │   └── ChatGPTEditor.h              # Module header
│       └── Private/
│           ├── ChatGPTEditor.cpp            # Module implementation
│           ├── SChatGPTWindow.h             # Slate window header
│           └── SChatGPTWindow.cpp           # Slate window implementation
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

- **Python dependency**: Python scripting requires the Python Editor Script Plugin to be enabled
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **Command parsing**: Natural language command extraction may not work for all phrasings

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

### 1.1.0 (Current)
- Added Editor Console Command execution with natural language support
- Added Python script generation and execution
- Implemented audit logging for all console and scripting actions
- Added command whitelist/blacklist security system
- Added script validation and security warnings
- Enhanced permission system with actual functionality
- Added comprehensive documentation and usage examples

### 1.0.0
- Initial release
- Basic ChatGPT integration
- Security permission toggles (UI only)
- Conversation history
- Environment-based API key storage

---

**Remember**: This plugin is in beta. Always backup your project before use and test in a non-production environment first.