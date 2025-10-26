# ChatGPTEditor

A secure Unreal Engine 5.5 Editor plugin that integrates ChatGPT functionality directly into the Unreal Editor. This plugin adds a Slate-based Editor tab that allows you to send messages to the OpenAI Chat Completions API and receive responses in real-time.

## Features

- **Slate-based UI**: Clean, integrated Editor tab that fits naturally into the Unreal Editor workflow
- **OpenAI Integration**: Direct communication with OpenAI's Chat Completions API (GPT-3.5-turbo)
- **Test Automation & QA**: Generate, preview, and manage automated tests with natural language prompts
- **Security-First Design**: Permission toggles for potentially destructive operations
- **Conversation History**: Maintains context throughout your ChatGPT conversation
- **Audit Logging**: All test generation and execution requests are logged for security and compliance
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

## Future Enhancements

Potential features for future versions:

**Test Automation:**
- Direct test execution from the ChatGPT window
- Automated test result parsing and display
- Test suite management and organization
- Performance benchmarking integration
- Code coverage analysis

**General Features:**
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

### 1.0.0
- Initial release
- Basic ChatGPT integration
- Security permission toggles (UI only)
- Conversation history
- Environment-based API key storage

---

**Remember**: This plugin is in beta. Always backup your project before use and test in a non-production environment first.