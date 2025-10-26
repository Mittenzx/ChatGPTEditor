# ChatGPTEditor - Usage Guide

This comprehensive guide covers all features of the ChatGPTEditor plugin for Unreal Engine 5.5.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Basic ChatGPT Conversation](#basic-chatgpt-conversation)
3. [Blueprint Scripting Assistant](#blueprint-scripting-assistant)
4. [Asset Automation](#asset-automation)
5. [Keyboard Shortcuts](#keyboard-shortcuts)
6. [Accessibility Features](#accessibility-features)
7. [Security Permissions](#security-permissions)
8. [Audit Logs](#audit-logs)
9. [Tips and Best Practices](#tips-and-best-practices)
10. [Troubleshooting](#troubleshooting)
# ChatGPTEditor Usage Guide

This guide provides detailed information on how to use the ChatGPTEditor plugin effectively, with a focus on UX features, accessibility options, and asset automation.
This guide provides detailed information on how to use the ChatGPTEditor plugin effectively, with a focus on UX features and accessibility options.

## Table of Contents
1. [Getting Started](#getting-started)
2. [Keyboard Shortcuts Reference](#keyboard-shortcuts-reference)
3. [Asset Automation](#asset-automation)
4. [Accessibility Features](#accessibility-features)
5. [Tips and Best Practices](#tips-and-best-practices)
6. [Troubleshooting](#troubleshooting)
3. [Accessibility Features](#accessibility-features)
4. [Tips and Best Practices](#tips-and-best-practices)
5. [Troubleshooting](#troubleshooting)

## Getting Started

### Opening the ChatGPT Window

1. Open your Unreal Engine project
2. Go to **Window → ChatGPT** in the menu bar
3. The ChatGPT window will open as a dockable tab
4. You can dock it anywhere in your editor layout

### First-Time Setup

Before using the plugin, ensure you have:
- Set the `OPENAI_API_KEY` environment variable with your API key
- Restarted Unreal Editor after setting the environment variable
- Verified the API key is set correctly

## Basic ChatGPT Conversation

### Sending Messages

1. Type your question or request in the input box at the bottom
2. Click "Send" or press **Ctrl+Enter**
3. Wait for the response to appear in the conversation history
4. Continue the conversation - the plugin maintains context

### Clearing History

- Click the "Clear" button to start a new conversation
- Or press **Ctrl+L** to clear via keyboard shortcut
- This removes all messages and resets the conversation context

### Example Conversations

**General Questions:**
```
You: "How do I create a character controller in Unreal Engine?"
Assistant: [Provides detailed explanation]
```

**Code Help:**
```
You: "Show me how to spawn an actor at runtime in C++"
Assistant: [Provides code examples]
```

## Blueprint Scripting Assistant

The Blueprint Scripting Assistant helps you generate and understand Blueprints using natural language.

### Generating Blueprints

**Requirements:**
- Enable "Allow Asset Write Operations" permission toggle
- Accept the security warning when enabling

**Steps:**
1. Enable the "Allow Asset Write Operations" checkbox
2. In the "Blueprint Scripting Assistant" section, enter your Blueprint description
   - Example: "Create a health pickup that restores 25 health points"
3. Click "Generate Blueprint"
4. Review the preview dialog showing:
   - Your original request
   - Generated Blueprint description
   - List of nodes to be created
   - Connections between nodes
   - Security warning
5. Click "Approve & Create" to generate, or "Reject" to cancel
6. Check the conversation history for confirmation

**Example Prompts:**
- "Create a damage zone that hurts the player when they enter it"
- "Make a collectible coin that disappears when picked up"
- "Build a door that opens when the player presses E"

### Explaining Blueprints

**Steps:**
1. In the Blueprint name field, enter the name of a Blueprint
   - Example: "BP_PlayerController"
2. Click "Explain Blueprint"
3. View the explanation in the conversation history with:
   - Summary of the Blueprint's purpose
   - Step-by-step breakdown of its logic

**Note:** Currently provides AI-generated explanations based on Blueprint names. Future versions will support visual selection and actual Blueprint analysis.

### Blueprint Audit Log

**Viewing Operations:**
- All Blueprint generation and explanation requests are logged
- Includes timestamps, user prompts, AI responses, and approval decisions

**Exporting Logs:**
1. Click "Export Audit Log" in the Blueprint Scripting Assistant section
2. Choose a save location
3. Review the exported text file for compliance and security review

## Asset Automation

Asset Automation allows you to create and manage Unreal Engine assets using natural language commands.
1. Launch Unreal Editor with your project
2. Navigate to **Window → ChatGPT** in the menu bar
3. The ChatGPT window will open as a dockable tab
4. You can dock it anywhere in your editor layout

### Your First Conversation

1. Type your message in the input box at the bottom
2. Press **Ctrl+Enter** or click the **Send** button
3. Wait for the response (you'll see a loading indicator)
4. Continue the conversation - context is maintained automatically

## Keyboard Shortcuts Reference

The plugin provides several keyboard shortcuts for efficient workflow:

### Message Operations
| Shortcut | Action | Description |
|----------|--------|-------------|
| **Ctrl+Enter** | Send Message | Sends the current message to ChatGPT |
| **Ctrl+L** | Clear History | Clears all conversation history |

### Accessibility Controls
| Shortcut | Action | Description |
|----------|--------|-------------|
| **Ctrl+Plus** (+) | Increase Font | Makes text larger (up to 24pt) |
| **Ctrl+Minus** (-) | Decrease Font | Makes text smaller (down to 8pt) |
| **Ctrl+0** | Reset Font | Resets font to default size (10pt) |

### Navigation
| Shortcut | Action | Description |
|----------|--------|-------------|
| **Tab** | Next Element | Move focus to next UI element |
| **Shift+Tab** | Previous Element | Move focus to previous UI element |

## Asset Automation

### Enabling Asset Automation

1. Check the "Allow Asset Write Operations" checkbox in the plugin window
2. Read and accept the security warning
3. You can now use natural language commands to create and manage assets

### Supported Commands

**Creating Assets:**
- `Create material MaterialName` - Creates a new material
- `Create texture TextureName` - Creates a new texture
- `Create blueprint BlueprintName` - Creates a new blueprint

**Managing Assets:**
- `Rename OldName to NewName` - Renames an asset
- `Move AssetName to /Game/NewPath` - Moves an asset
- `Delete AssetName` - Deletes an asset (requires extra confirmation)

### Using Asset Automation

**Setup:**
1. Enable "Allow Asset Write Operations" permission
2. Accept the security warning

**Workflow:**
1. Ask ChatGPT to create or modify assets using natural language
   - Example: "Create a material called M_GlowingMetal and a texture called T_MetalBase"
2. ChatGPT will respond with the appropriate commands
3. The system detects asset operations in the response
4. For each operation:
   - A confirmation dialog appears showing what will be created/modified
   - Review the details carefully
   - Click "OK" to proceed or "Cancel" to skip
5. Check the conversation history for operation results

**Example Conversation:**
```
You: "I need materials for my game - M_Metal, M_Wood, and M_Stone"
Assistant: "I'll help you create those materials. Create material M_Metal. Create material M_Wood. Create material M_Stone."
System: Detected 3 asset operation(s) in response.
System: Processing: Create Material - M_Metal
[Confirmation Dialog] → User approves
System: ✓ Successfully executed: Create Material
[Process repeats for M_Wood and M_Stone]
```

### Asset Audit Log

All asset operations are logged to `Saved/ChatGPTEditor/audit.log` with:
- Timestamp
- Operation type
- Asset name
- User name
- Success/failure status
- Additional details

**Viewing the Log:**
- Navigate to `YourProject/Saved/ChatGPTEditor/audit.log`
- Open with any text editor
- Review all operations performed

## Keyboard Shortcuts

The plugin supports these keyboard shortcuts for efficient workflow:

| Shortcut | Action |
|----------|--------|
| **Ctrl+Enter** | Send message to ChatGPT |
| **Ctrl+L** | Clear conversation history |
| **Ctrl+Plus** (+) | Increase font size |
| **Ctrl+Minus** (-) | Decrease font size |
| **Ctrl+0** | Reset font size to default |

**Tips:**
- Shortcuts work when the ChatGPT window has focus
- On macOS, use Cmd instead of Ctrl
- Combine with mouse navigation for fastest workflow

## Accessibility Features

### Adjustable Font Size

**Using Buttons:**
1. Locate the accessibility controls section
2. Click **A-** to decrease font size
3. Click **A+** to increase font size
4. Click the font size display (e.g., "10pt") to reset to default

**Using Keyboard:**
- Press **Ctrl+Plus** to increase
- Press **Ctrl+Minus** to decrease
- Press **Ctrl+0** to reset

**Range:** 8pt to 24pt

### Tooltips

- Hover over any button, checkbox, or input field to see helpful tooltips
- Tooltips explain what each control does
- Security permissions show detailed risk warnings in tooltips

### Visual Indicators

- 🔒 Lock icons indicate security-sensitive permissions
- ⏳ Loading indicators show when API requests are in progress
- 💡 Light bulb icons highlight tips and helpful information
- ⌨️ Keyboard icons show shortcut information

## Security Permissions

The plugin includes three permission toggles for safety:

### Allow Asset Write Operations (DANGEROUS)

**Default:** OFF  
**Risk:** Can create, modify, or delete project assets  
**Required for:**
- Blueprint Scripting Assistant generation
- Asset Automation commands

**When to enable:**
- Only when you want to create or modify assets
- After reviewing what will be created
- With project backups in place

### Allow Console Commands (DANGEROUS)

**Default:** OFF  
**Risk:** Can execute arbitrary console commands  
**Currently unused by this plugin**

### Allow File I/O Operations (DANGEROUS)

**Default:** OFF  
**Risk:** Can read and write files on your system  
**Currently unused by this plugin**

**Important:** Each permission shows a warning dialog when enabled. Read the warnings carefully before proceeding.

## Audit Logs

### Blueprint Audit Log

**Location:** Exported via "Export Audit Log" button  
**Contains:**
- Blueprint generation requests with user prompts
- Preview dialogs shown to user
- User approval/rejection decisions
- Blueprint explanation requests
- Timestamps for all operations

**Format:**
```
[2025-10-26 15:30:45] GENERATION
Description: Blueprint generation requested
User Prompt: Create a health pickup that restores 25 health points
Generated Content: {...}
Approved: Yes
```

### Asset Audit Log

**Location:** `YourProject/Saved/ChatGPTEditor/audit.log`  
**Contains:**
- Asset creation, modification, deletion operations
- User name and timestamp
- Success/failure status
- Operation details

**Format:**
```
[2025-10-26 15:30:45] User: JohnDoe | Operation: Create Material | Asset: M_Metal | Success: YES | Details: Operation completed successfully
```

## Tips and Best Practices

### General Usage

1. **Start with simple questions** to test the connection
2. **Provide context** in your prompts for better responses
3. **Use clear, specific language** when requesting asset operations
4. **Keep conversations focused** on one topic for better context

### Blueprint Scripting

1. **Always review the preview** before approving Blueprint generation
2. **Start with simple Blueprints** to understand the workflow
3. **Test generated Blueprints** in a separate project first
4. **Export audit logs regularly** for compliance records

### Asset Automation

1. **Enable permissions only when needed** and disable immediately after
2. **Review confirmation dialogs carefully** before proceeding
3. **Use descriptive asset names** (e.g., M_MetalChrome not Mat1)
4. **Check the audit log** after operations to verify success
5. **Keep backups** before using asset automation on production projects

### Security

1. **Never enable all permissions by default**
2. **Read warning dialogs** before clicking OK
3. **Review audit logs** periodically for unusual activity
4. **Test new features** in non-production projects
5. **Keep the plugin updated** for security patches

## Troubleshooting

### Plugin Window Not Opening

- Check that the plugin is enabled in Edit → Plugins
- Restart Unreal Editor after enabling
- Look for the ChatGPT menu item under Window

### API Connection Failed

- Verify OPENAI_API_KEY environment variable is set
- Restart Unreal Editor after setting the variable
- Check internet connection
- Verify API key is valid at platform.openai.com

### Blueprint Generation Not Working

- Ensure "Allow Asset Write Operations" is enabled
- Check that you approved the warning dialog
- Verify the Blueprint prompt is clear and specific
- Review the conversation history for error messages

### Asset Automation Not Detecting Commands

- Make sure ChatGPT's response contains explicit commands like "Create material X"
- Enable "Allow Asset Write Operations" permission
- Check the conversation history for "Detected X asset operation(s)" message
- Try rephrasing your request to be more explicit

### Keyboard Shortcuts Not Working

- Ensure the ChatGPT window has focus (click on it first)
- On macOS, try Cmd instead of Ctrl
- Check that other plugins aren't intercepting the shortcuts
- Restart the editor if shortcuts stop responding

### Font Size Not Changing

- Use the A-/A+ buttons if keyboard shortcuts don't work
- Check that the conversation history box is visible
- Try clicking directly on the font size display to reset

### Audit Log Not Exporting

- Ensure you have write permissions to the target directory
- Try exporting to a different location (e.g., Desktop)
- Check the Unreal Editor output log for error messages
- Verify operations have been performed (log is only created when there are entries)

### Permission Toggle Not Working

- Click the checkbox itself, not just the label
- Read the warning dialog completely before dismissing
- If stuck enabled, restart the editor to reset to default (OFF)

## Advanced Usage

### Combining Features

You can use multiple features together:
1. Ask ChatGPT for advice on Blueprint design
2. Use the advice to craft a Blueprint generation prompt
3. Generate the Blueprint with approval
4. Create supporting assets using Asset Automation
5. Export both audit logs for documentation

### Workflow Integration

**Level Design Workflow:**
1. Chat with GPT about level mechanics
2. Generate blueprints for interactive elements
3. Create materials and textures via asset automation
4. Review audit logs at end of session

**Documentation Workflow:**
1. Use Blueprint Explanation for existing Blueprints
2. Copy explanations to design documents
3. Export audit log showing all analyzed Blueprints

## Getting Help

- Review this guide for feature-specific help
- Check the README.md for installation and setup
- Review SECURITY.md for security best practices
- Check ACCESSIBILITY.md for accessibility features
- Open GitHub issues for bugs or feature requests

---

**Version:** 1.2  
**Last Updated:** October 26, 2025  
**Compatible with:** Unreal Engine 5.5+
- `Create material MaterialName` - Creates a new material asset
- `Create texture TextureName` - Creates a new texture asset  
- `Create blueprint BlueprintName` - Creates a new blueprint asset

**Managing Assets:**
- `Rename OldName to NewName` - Renames an existing asset
- `Delete AssetName` - Deletes an asset (requires extra confirmation)

### Example Workflows

**Example 1: Creating Multiple Assets**
```
You: "Create a material called M_Metal and a texture called T_MetalBase"

ChatGPT: "I'll help you create those. Create material M_Metal. Create texture T_MetalBase."

[System detects commands and shows confirmation dialogs]
```

**Example 2: Renaming an Asset**
```
You: "Rename M_OldMaterial to M_NewMaterial"

ChatGPT: "I'll rename that for you. Rename M_OldMaterial to M_NewMaterial."

[System shows confirmation with preview]
```

### Audit Logging

All asset operations are automatically logged to `Saved/ChatGPTEditor/audit.log`:

```
[2024-10-26 15:30:45] User: JohnDoe | Operation: Create Material | Asset: M_Metal | Success: YES
[2024-10-26 15:31:12] User: JohnDoe | Operation: Rename Asset | Asset: M_OldMaterial->M_NewMaterial | Success: YES
```

## Accessibility Features

### Font Size Adjustment

The plugin allows you to customize the text size for better readability:

#### Using Buttons
1. Locate the "Accessibility" section above the keyboard shortcuts
2. Click **A-** to decrease font size
3. Click **A+** to increase font size
4. Click the font size display to reset to default

#### Using Keyboard
- **Ctrl+Plus**: Increase font size by 1 point
- **Ctrl+Minus**: Decrease font size by 1 point
- **Ctrl+0**: Reset to default (10pt)

#### Font Size Range
- **Minimum**: 8pt (for compact display)
- **Default**: 10pt (comfortable reading)
- **Maximum**: 24pt (for larger displays or visual accessibility)

### Tooltips

Every interactive element in the plugin includes helpful tooltips:

#### How to Use Tooltips
1. Hover your mouse over any button, checkbox, or input field
2. Wait for 1 second
3. A tooltip will appear with detailed information
4. Move away to dismiss the tooltip

#### What Tooltips Show
- **Buttons**: Action description and keyboard shortcut
- **Checkboxes**: Detailed security warning and implications
- **Input Box**: Usage instructions and tips

### Visual Indicators

The plugin uses several visual indicators for better user experience:

#### Icons
- 🔒 **Lock Icon**: Indicates security-sensitive permissions
- ⏳ **Hourglass**: Shows when an API request is in progress
- 💡 **Lightbulb**: Highlights tips and helpful information
- ⌨️ **Keyboard**: Marks keyboard shortcuts reference

#### Status Messages
- **System messages**: Shown in gray for informational updates
- **User messages**: Prefixed with [User] for clarity
- **Assistant messages**: Prefixed with [Assistant] for clarity
- **Error messages**: Prefixed with [Error] for issues

### Keyboard-Only Navigation

The plugin is fully accessible via keyboard:

1. **Press Tab** to move between elements
2. **Press Space** or **Enter** to activate buttons
3. **Use arrow keys** to navigate within text areas
4. **Use Ctrl+shortcuts** for quick actions

## Tips and Best Practices

### Efficient Workflow

1. **Use Keyboard Shortcuts**: Learn and use Ctrl+Enter for sending messages quickly
2. **Adjust Font Early**: Set your preferred font size at the start of your session
3. **Keep Context**: Don't clear history unless starting a completely new topic
4. **Read Tooltips**: Hover over unfamiliar elements to learn their purpose

### Asset Automation Best Practices

1. **Enable Only When Needed**: Keep "Allow Asset Write Operations" disabled unless actively creating/modifying assets
2. **Review Confirmations**: Always read the confirmation dialog before proceeding
3. **Check Audit Log**: Regularly review the audit log to track changes
4. **Backup First**: Create backups before using automation on important projects
5. **Test First**: Try commands in a test project before using on production assets

### Accessibility Best Practices

1. **Visual Comfort**:
   - Adjust font size to match your display and viewing distance
   - Use Ctrl+0 to reset if you're experimenting with sizes
   - Take breaks if you're reading long responses

2. **Keyboard Efficiency**:
   - Use Ctrl+Enter instead of clicking Send for faster input
   - Use Ctrl+L to quickly clear and start fresh
   - Keep your hands on the keyboard for uninterrupted workflow

3. **Understanding Security**:
   - Read the tooltips on security checkboxes before enabling
   - Keep permissions disabled unless actively needed
   - Understand the risks before enabling any permission

### Working with Long Conversations

1. **Context Management**:
   - ChatGPT remembers previous messages in the conversation
   - Clear history when starting a new, unrelated topic
   - Be aware of token limits (~4096 tokens for GPT-3.5-turbo)

2. **Readability**:
   - Increase font size for long reading sessions
   - Scroll through history using mouse wheel or scrollbar
   - Use clear, specific prompts for better responses

## Troubleshooting

### Keyboard Shortcuts Not Working

**Problem**: Pressing Ctrl+Enter doesn't send the message

**Solutions**:
1. Ensure the ChatGPT window has focus (click inside it)
2. Check that the input box is not empty
3. Try clicking in the input box first, then using the shortcut
4. On macOS, try Cmd instead of Ctrl

### Asset Automation Not Working

**Problem**: Commands are not being detected

**Solutions**:
1. Ensure "Allow Asset Write Operations" is checked
2. Use exact command format (e.g., "Create material MaterialName")
3. Check that ChatGPT includes the command in its response
4. Review the conversation history for error messages

### Asset Not Found

**Problem**: "Asset not found" error when trying to rename/delete

**Solutions**:
1. Verify the asset name exactly matches an existing asset
2. Check the Content Browser for the correct name
3. Try using the full package path
4. Ensure the asset is not already open in an editor

### Font Size Not Changing

**Problem**: Clicking A+ or A- doesn't change the font size

**Solutions**:
1. Check if you've reached the minimum (8pt) or maximum (24pt) limit
2. Try using keyboard shortcuts (Ctrl+/-) instead
3. Reset font size with Ctrl+0 and try again

### Tooltips Not Appearing

**Problem**: Hovering over elements doesn't show tooltips

**Solutions**:
1. Wait at least 1 second while hovering
2. Ensure your mouse is directly over the element
3. Check that Unreal Editor tooltips are enabled in settings
4. Try restarting the ChatGPT window (close and reopen)

### Can't See All Text

**Problem**: Text is too small or too large

**Solutions**:
1. Press **Ctrl+0** to reset to default font size
2. Use **Ctrl+Plus** to make text larger
3. Use **Ctrl+Minus** to make text smaller
4. Adjust your display scaling in Windows/macOS if needed

### Window Layout Issues

**Problem**: ChatGPT window is too small or misplaced

**Solutions**:
1. Drag the window edges to resize
2. Drag the window tab to redock it
3. Close and reopen from Window → ChatGPT
4. Reset your editor layout in Unreal Editor preferences

## Advanced Usage

### Customizing Your Workflow

1. **Dock Position**: Place the ChatGPT window where it's most convenient
   - Bottom panel for quick reference
   - Side panel for continuous access
   - Floating for multi-monitor setups

2. **Font Size Presets**:
   - 8pt: Compact view for more screen space
   - 10pt: Default balanced view
   - 14pt: Comfortable for long reading
   - 18pt: Large text for accessibility
   - 24pt: Maximum size for visual impairment

3. **Keyboard-Centric Workflow**:
   - Keep hands on keyboard with Ctrl+Enter
   - Clear quickly with Ctrl+L when needed
   - Adjust view with Ctrl+/- without mouse

### Integration with Unreal Workflow

1. **Parallel Work**: Keep ChatGPT open while working in viewports
2. **Quick Queries**: Use Ctrl+Enter for rapid back-and-forth
3. **Reference**: Use increased font for code snippets ChatGPT provides
4. **Documentation**: Ask about Unreal features while developing
5. **Asset Creation**: Use natural language to quickly create placeholder assets

## Feedback and Support

Found an issue or have a suggestion?
- Open an issue on GitHub
- Check the README.md for additional information
- Review this guide for commonly asked questions

---

**Last Updated**: Version 1.1.0 (UX, Accessibility & Asset Automation Update)
**Last Updated**: Version 1.1.0 (UX & Accessibility Update)
