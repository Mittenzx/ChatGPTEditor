# ChatGPTEditor

A secure Unreal Engine 5.5 Editor plugin that integrates ChatGPT functionality directly into the Unreal Editor. This plugin adds a Slate-based Editor tab that allows you to send messages to the OpenAI Chat Completions API and receive responses in real-time.

## Features

- **Slate-based UI**: Clean, integrated Editor tab that fits naturally into the Unreal Editor workflow
- **OpenAI Integration**: Direct communication with OpenAI's Chat Completions API (GPT-3.5-turbo)
- **External API & Web Integration**: Connect to external REST APIs and web services via chat prompts
- **Security-First Design**: Permission toggles for potentially destructive operations
- **Preview & Approval System**: All external API requests require manual approval before execution
- **Audit Logging**: Comprehensive logging of all API connections and code changes
- **Code Generation**: Automatic generation of UE5 integration code for external APIs
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

#### 🌐 Allow External API/Web Integration (REQUIRES APPROVAL)
- **Default**: OFF
- **Risk**: Can make HTTP requests to external web services and APIs
- **When to enable**: When you want to integrate external REST APIs or web services into your UE5 project
- **Security**: Every API request requires preview and manual approval before execution
- **Logging**: All connections are logged to `Saved/ChatGPTEditor/audit.log`
- **Features**:
  - Preview all request details (endpoint, method, headers, body)
  - Automatic generation of UE5-compatible integration code
  - Audit trail of all approved and denied requests

**Important**: Each permission requires explicit confirmation when enabled. You will see a warning dialog explaining the risks.

### External API & Web Integration

The External API integration feature allows you to connect Unreal Engine to external web services using natural language prompts.

#### How to Use

1. **Enable the permission**: Check "Allow External API/Web Integration" in the permissions section
2. **Type your request**: Use natural language to describe what you want to do:
   - "Fetch data from https://api.example.com/users"
   - "Send a POST request to https://api.example.com/analytics with event data"
   - "Get weather data from a REST API"
3. **Review the preview**: A dialog will show you exactly what HTTP request will be made
4. **Approve or deny**: Choose whether to execute the request
5. **View results**: See the API response and generated UE5 integration code
6. **Check audit log**: All requests are logged to `Saved/ChatGPTEditor/audit.log`

#### Example Usage

**Example 1: GET Request**
```
User: "Fetch data from https://jsonplaceholder.typicode.com/posts/1"

System will:
1. Parse the request
2. Show preview with endpoint, method (GET), and headers
3. Wait for your approval
4. Execute the request
5. Display the response
6. Generate UE5 C++ code for integration
7. Log everything to audit.log
```

**Example 2: POST Request**
```
User: "Send a POST request to https://api.example.com/events with analytics data"

System will:
1. Detect POST method
2. Show preview dialog
3. Wait for approval
4. Execute if approved
5. Provide integration code
```

#### Generated Code

The plugin automatically generates UE5-compatible C++ code for any approved API request. This code includes:
- Proper HTTP module setup
- Request configuration (URL, method, headers)
- Response handling
- Error handling
- Complete example ready to integrate into your project

#### Security Features

- **Preview First**: Every request shows a detailed preview before execution
- **Manual Approval**: No request executes without explicit user approval
- **Audit Logging**: Complete log of all attempts in `Saved/ChatGPTEditor/audit.log`
- **Validation**: Automatic validation of URLs and HTTP methods
- **No Auto-Execution**: External connections are never made automatically

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
- **Review all API request previews carefully before approving**
- **Check the audit log regularly** (`Saved/ChatGPTEditor/audit.log`)
- **Verify external API endpoints** before approving requests

❌ **DON'T:**
- Enable all permissions by default
- Leave permissions enabled when not in use
- Use on production projects without thorough testing
- Trust AI-generated code without review
- **Approve API requests to unknown or untrusted endpoints**
- **Share sensitive data in API requests without encryption**

### General Security

- **Review all AI-generated code** before running it in your project
- **Use version control** (Git) to track all changes
- **Test in isolated environments** before applying changes to production projects
- **Monitor API usage** to detect unusual activity
- **Keep the plugin updated** to receive security patches
- **Regularly review audit logs** at `Saved/ChatGPTEditor/audit.log`
- **Be cautious with external API integrations** - only connect to trusted services
- **Never send sensitive data** through API requests without proper encryption and authorization

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
│           ├── AuditLogger.h                # Audit logging system header
│           ├── AuditLogger.cpp              # Audit logging implementation
│           ├── ExternalAPIHandler.h         # External API handler header
│           └── ExternalAPIHandler.cpp       # External API handler implementation
└── README.md                                 # This file
```

### Audit Logging

All plugin activities are logged to `Saved/ChatGPTEditor/audit.log` in your project directory. The audit log includes:

- Module startup/shutdown events
- API connection attempts (approved and denied)
- Code generation events
- Endpoint URLs, HTTP methods, and timestamps
- User approval/denial decisions

**Example audit log entry:**
```
[2024-10-26 15:30:45] API_CONNECTION | Status: APPROVED | Method: GET | Endpoint: https://api.example.com/data
[2024-10-26 15:30:47] CODE_CHANGE | Status: APPROVED | Description: Fetch data from REST endpoint
Code Preview:
// Unreal Engine 5 HTTP Request Code
...
---
```

**Accessing the audit log:**
- Location: `YourProject/Saved/ChatGPTEditor/audit.log`
- Format: Plain text, timestamped entries
- Persistence: Log persists across editor sessions

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

### External API Request Failed

- Check the endpoint URL is correct and accessible
- Verify the target service is online and responding
- Check your internet connection and firewall settings
- Review the audit log for detailed error information
- Ensure the HTTP method is appropriate for the endpoint

### Audit Log Not Found

- The audit log is created automatically when the plugin starts
- Check `YourProject/Saved/ChatGPTEditor/` directory
- If the directory doesn't exist, it will be created on first use
- Ensure the editor has write permissions to the Saved directory

## Known Limitations

- **Asset Write/Console Command/File I/O permissions**: UI elements for future functionality. Core destructive operations not yet implemented.
- **No local AI support**: Requires internet connection and OpenAI API access
- **Conversation context**: Limited by API token limits (approximately 4096 tokens for GPT-3.5-turbo)
- **No streaming responses**: Responses arrive all at once rather than streaming
- **API parsing**: Natural language API request parsing uses simple pattern matching; complex requests may require explicit endpoint URLs

## Future Enhancements

Potential features for future versions:
- Streaming response support
- Custom system prompts
- Conversation save/load functionality
- Integration with Unreal's Blueprint system
- Asset creation and modification (when Asset Write permission is active)
- Multi-model support (GPT-4, etc.)
- Advanced API request builder with custom headers and authentication
- Webhook support for real-time integrations
- API response caching and retry logic

## Contributing

Contributions are welcome! Please ensure any pull requests:
- Follow Unreal Engine coding standards
- Include appropriate security warnings for new features
- Update this README with relevant documentation
- Test thoroughly before submission

## License

This plugin is provided as-is for educational and development purposes. Please ensure compliance with OpenAI's terms of service and usage policies when using this plugin.

## Disclaimer

⚠️ **Important**: This plugin sends data to external services (OpenAI and potentially other APIs). Be mindful of:
- What information you share in conversations
- Your organization's data privacy policies
- OpenAI's data usage policies
- Potential costs associated with API usage
- **Data sent to external APIs when using the External API/Web Integration feature**
- **Security implications of connecting to third-party services**

The authors of this plugin are not responsible for:
- API costs incurred
- Data shared with OpenAI or external APIs
- Any modifications made to your project while using this plugin
- Any consequences of enabling security permissions
- **Security breaches or data leaks from external API connections**
- **Malicious responses from third-party APIs**

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
- Security permission toggles (UI only for Asset Write, Console Commands, File I/O)
- **External API & Web Integration feature**
  - Natural language API request parsing
  - Preview and approval system for all external connections
  - Automatic UE5 C++ code generation
  - Comprehensive audit logging (Saved/ChatGPTEditor/audit.log)
  - Support for GET, POST, PUT, DELETE, PATCH methods
  - Secure request validation
- Conversation history
- Environment-based API key storage
- UE5.5 compatibility

---

**Remember**: This plugin is in beta. Always backup your project before use and test in a non-production environment first.