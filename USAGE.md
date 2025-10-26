# ChatGPT Editor - Usage Guide

This guide provides detailed instructions on using the ChatGPT Editor plugin for Unreal Engine 5.5.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Basic Chat Operations](#basic-chat-operations)
3. [Code Review and Explanation](#code-review-and-explanation)
4. [Documentation Generation](#documentation-generation)
5. [Security and Permissions](#security-and-permissions)
6. [Audit Log](#audit-log)
7. [Best Practices](#best-practices)

## Getting Started

After installing the plugin and setting up your OpenAI API key:

1. Open Unreal Editor
2. Navigate to **Window → ChatGPT**
3. The ChatGPT window will open as a dockable tab
4. You can dock it anywhere in your editor layout

## Basic Chat Operations

### Sending Messages

1. Type your question or request in the input box at the bottom
2. Click **Send** or press Enter
3. Wait for the response to appear in the conversation history
4. Continue the conversation by sending more messages

### Clearing History

- Click **Clear** to start a new conversation
- This removes all messages from the current session
- Note: The audit log is not cleared

## Code Review and Explanation

You can ask ChatGPT to explain code, review implementations, or summarize architecture:

### Example Queries

**Understanding Code:**
```
What does the SChatGPTWindow class do?
Explain how the HTTP request handling works
How is the conversation history maintained?
```

**Architecture Questions:**
```
Summarize the plugin architecture
What are the main components of this plugin?
Explain the security model
```

**Code Review:**
```
Review the audit logging implementation
Are there any potential issues with thread safety?
How could the documentation handler be improved?
```

### Important Notes

- Code explanations do not modify any files
- All requests are logged in the audit trail
- No permissions are required for code review queries

## Documentation Generation

The plugin can help generate or update documentation files.

### Prerequisites

1. Enable **Allow File I/O Operations** permission
2. Accept the warning dialog

### Requesting Documentation

**Generate New Documentation:**
```
Create a user guide for this plugin
Generate API documentation for the DocumentationHandler class
Write a troubleshooting guide
```

**Update Existing Documentation:**
```
Update the README with the new features
Add a section about security to the documentation
Improve the installation instructions
```

### Documentation Workflow

1. **Request**: Send your documentation request
2. **Preview**: Review the proposed changes in the conversation history
3. **Confirm**: A dialog appears asking you to confirm the changes
4. **Apply**: If confirmed, the file is updated; otherwise, the change is cancelled

### Supported File Types

- Markdown (`.md`)
- Plain text (`.txt`)
- ReStructuredText (`.rst`)
- AsciiDoc (`.adoc`)

### Restrictions

- Files must be within the plugin directory
- Path traversal (`..`) is not allowed
- Only documentation file extensions are permitted

## Security and Permissions

### Permission Types

The plugin has three permission levels:

1. **Allow Asset Write Operations** - Not used for documentation
2. **Allow Console Commands** - Not used for documentation
3. **Allow File I/O Operations** - Required for documentation generation

### Enabling Permissions Safely

1. Only enable permissions when needed
2. Read and understand the warning dialogs
3. Disable permissions after completing your task
4. Maintain backups of important files

### Documentation-Specific Safety

Even with File I/O enabled:
- All changes are previewed
- Explicit confirmation is required
- Operations are logged
- File paths are validated

## Audit Log

### Viewing the Audit Log

Click **View Audit Log** button to see:
- All ChatGPT requests
- Documentation operations
- User confirmations/cancellations
- Success or failure of operations
- Timestamps for all events

### Audit Log Entries

Each entry shows:
```
[YYYY-MM-DD HH:MM:SS] STATUS: Operation (File: path) - Details
```

Example:
```
[2024-01-15 14:30:22] SUCCESS: DocumentationRequest - Generate user guide
[2024-01-15 14:30:45] SUCCESS: ApplyDocChange (File: Plugins/ChatGPTEditor/USAGE.md)
```

### Audit Log Features

- Thread-safe logging
- Keeps last 1000 entries
- Shows last 50 entries in dialog
- Cannot be disabled (for security)

## Best Practices

### General Usage

✅ **DO:**
- Test in a non-production environment first
- Use version control (Git) for all projects
- Review AI responses before applying changes
- Keep backups of important documentation
- Read the full response before confirming changes

❌ **DON'T:**
- Blindly accept all suggestions
- Enable all permissions permanently
- Skip reading the preview
- Ignore the audit log

### Documentation Generation

✅ **DO:**
- Be specific in your requests
- Review generated content for accuracy
- Edit and refine AI-generated documentation
- Test documentation for clarity
- Version control your documentation

❌ **DON'T:**
- Accept documentation without review
- Overwrite critical files without backups
- Use on production documentation without testing
- Share sensitive information in prompts

### Code Review

✅ **DO:**
- Ask specific questions
- Use for learning and understanding
- Cross-reference with official documentation
- Combine AI insights with human review

❌ **DON'T:**
- Rely solely on AI for critical reviews
- Assume all explanations are 100% accurate
- Skip manual code review processes

## Troubleshooting

### Documentation Not Saving

**Problem**: Documentation changes aren't being applied

**Solutions**:
1. Check that **Allow File I/O Operations** is enabled
2. Verify the file path is within the plugin directory
3. Ensure file extension is supported (.md, .txt, .rst, .adoc)
4. Check the audit log for error messages

### Preview Not Showing

**Problem**: No preview appears after documentation request

**Solutions**:
1. Ensure your request contains documentation keywords
2. Check if the response contains documentation content
3. Verify File I/O permission is enabled
4. Try being more explicit in your request

### Permission Dialog Not Appearing

**Problem**: Warning dialog doesn't show when enabling permissions

**Solutions**:
1. Permission may already be enabled
2. Check the checkbox state
3. Try disabling and re-enabling the permission

## Advanced Usage

### Custom System Prompts

When requesting documentation, be specific about the format:

```
Generate a README in markdown format with the following sections:
- Installation
- Usage
- Examples
- Troubleshooting
```

### Iterative Documentation

You can refine documentation through conversation:

```
User: Generate a user guide
[Review the response]
User: Add more examples to section 3
User: Make the language more beginner-friendly
```

### Multi-File Documentation

For complex documentation needs, request files one at a time:

```
1. Create README.md with overview
2. Create USAGE.md with detailed usage
3. Create API.md with API reference
```

## Getting Help

If you encounter issues:

1. Check the **Troubleshooting** section in README.md
2. Review the **Audit Log** for error messages
3. Verify your OpenAI API key is valid
4. Check your internet connection
5. Open an issue on GitHub with:
   - Description of the problem
   - Relevant audit log entries
   - Steps to reproduce

## Feedback and Contributions

We welcome feedback and contributions:

- Report bugs via GitHub issues
- Suggest features via GitHub discussions
- Submit pull requests with improvements
- Share your experience using the plugin

---

**Version**: 1.0.0  
**Last Updated**: 2024-01-15  
**Compatibility**: Unreal Engine 5.5+
