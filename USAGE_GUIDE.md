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
