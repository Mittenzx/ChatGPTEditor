# ChatGPTEditor

A secure Unreal Engine 5.5 Editor plugin that integrates ChatGPT functionality directly into the Unreal Editor. This plugin adds a Slate-based Editor tab that allows you to send messages to the OpenAI Chat Completions API and receive responses in real-time.

## Features

- **Slate-based UI**: Clean, integrated Editor tab that fits naturally into the Unreal Editor workflow
- **OpenAI Integration**: Direct communication with OpenAI's Chat Completions API (GPT-3.5-turbo)
- **Security-First Design**: Permission toggles for potentially destructive operations
- **Conversation History**: Maintains context throughout your ChatGPT conversation
- **Environment-based API Keys**: Secure API key storage using environment variables
- **📝 Documentation Generation**: Generate or update documentation files (README, guides) from natural language prompts
- **🔍 Code Review & Explanation**: Request explanations for code, assets, or project structure
- **📋 Audit Logging**: Complete audit trail of all documentation and code review operations
- **✅ Preview & Confirm**: All documentation changes are previewed and require explicit confirmation

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
- **When to enable**: When you need ChatGPT to generate or update documentation files
- **Warning**: Can lead to data loss or file corruption
- **Protection**: Limited to documentation files within the plugin directory

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

❌ **DON'T:**
- Enable all permissions by default
- Leave permissions enabled when not in use
- Use on production projects without thorough testing
- Trust AI-generated code without review

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
│           ├── SChatGPTWindow.cpp           # Slate window implementation
│           ├── AuditLog.h                   # Audit logging system
│           ├── DocumentationHandler.h       # Documentation operations header
│           └── DocumentationHandler.cpp     # Documentation operations implementation
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

- **Documentation parsing is basic**: The system uses pattern matching to detect documentation requests. Complex requests may not be automatically detected.
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **File operations limited**: Only documentation files (.md, .txt, .rst, .adoc) in the plugin directory can be modified
- **Manual content extraction**: For documentation generation, you may need to manually extract and format the AI's response

## Future Enhancements

Potential features for future versions:
- Improved documentation request parsing and extraction
- Streaming response support
- Custom system prompts for documentation generation
- Conversation save/load functionality
- Integration with Unreal's Blueprint system
- Enhanced code review with inline comments
- Asset analysis and recommendations
- Multi-model support (GPT-4, etc.)
- Automated documentation formatting
- Diff view for documentation changes

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

### 1.0.0 (Current)
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