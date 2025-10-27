# Troubleshooting Guide

This guide helps you resolve common issues with the ChatGPTEditor plugin.

## Table of Contents

- [Installation Issues](#installation-issues)
- [API Connection Problems](#api-connection-problems)
- [Plugin Not Loading](#plugin-not-loading)
- [Feature-Specific Issues](#feature-specific-issues)
- [Performance Issues](#performance-issues)
- [Build and Compilation Errors](#build-and-compilation-errors)
- [Logging and Diagnostics](#logging-and-diagnostics)

## Installation Issues

### Plugin Not Visible in Plugin List

**Problem**: Plugin doesn't appear in Edit → Plugins menu.

**Solutions**:
1. Verify plugin is in correct location:
   ```
   YourProject/
   └── Plugins/
       └── ChatGPTEditor/
           ├── ChatGPTEditor.uplugin
           └── Source/
   ```

2. Check plugin manifest is valid:
   ```bash
   # On Linux/macOS
   cat Plugins/ChatGPTEditor/ChatGPTEditor.uplugin | python -m json.tool
   
   # On Windows PowerShell
   Get-Content Plugins\ChatGPTEditor\ChatGPTEditor.uplugin | ConvertFrom-Json
   ```

3. Restart Unreal Editor completely

4. Check Output Log for errors:
   - Window → Developer Tools → Output Log
   - Filter for "ChatGPT" or "Plugin"

### Plugin Fails to Enable

**Problem**: Checkbox to enable plugin is grayed out or error occurs when enabling.

**Solutions**:
1. Check Unreal Engine version compatibility (requires 5.5+)
   ```
   Edit → About Unreal Editor → Version
   ```

2. Verify all dependencies are available:
   - PythonScriptPlugin (enable if needed)
   - All required modules in .uplugin file

3. Check for compilation errors:
   - Build the plugin manually
   - Check Build Output for errors

4. Review plugin log:
   ```
   Saved/Logs/YourProject.log
   ```

## API Connection Problems

### "Please set OPENAI_API_KEY environment variable"

**Problem**: Plugin can't find your OpenAI API key.

**Solutions**:

1. **Verify environment variable is set**:
   
   Windows (PowerShell):
   ```powershell
   $env:OPENAI_API_KEY
   ```
   
   Windows (CMD):
   ```cmd
   echo %OPENAI_API_KEY%
   ```
   
   macOS/Linux:
   ```bash
   echo $OPENAI_API_KEY
   ```

2. **Set the environment variable correctly**:

   Windows (PowerShell - System):
   ```powershell
   [System.Environment]::SetEnvironmentVariable('OPENAI_API_KEY', 'sk-your-key-here', 'User')
   ```
   
   Windows (CMD):
   ```cmd
   setx OPENAI_API_KEY "sk-your-key-here"
   ```
   
   macOS/Linux (persistent):
   ```bash
   echo 'export OPENAI_API_KEY="sk-your-key-here"' >> ~/.bashrc
   # or for zsh:
   echo 'export OPENAI_API_KEY="sk-your-key-here"' >> ~/.zshrc
   source ~/.bashrc  # or ~/.zshrc
   ```

3. **Restart Unreal Editor** after setting the variable

4. **Verify key format**: Should start with `sk-`

### API Request Fails / No Response

**Problem**: Messages sent but no response received.

**Solutions**:

1. **Check internet connection**

2. **Verify API key is valid**:
   - Test at https://platform.openai.com/playground
   - Check your OpenAI account status
   - Ensure you have credits/billing set up

3. **Check firewall/proxy settings**:
   - Allow outbound HTTPS to api.openai.com
   - Port 443 must be open

4. **Review Output Log**:
   ```
   Window → Developer Tools → Output Log
   Filter: "LogChatGPTEditor" or "HTTP"
   ```

5. **Common API errors**:
   - `401 Unauthorized`: Invalid API key
   - `429 Too Many Requests`: Rate limit exceeded
   - `500 Server Error`: OpenAI service issue
   - `Timeout`: Network or API latency issue

6. **Check audit log**:
   ```
   YourProject/Saved/ChatGPTEditor/audit.log
   ```

### Slow API Responses

**Problem**: Responses take very long to arrive.

**Solutions**:

1. **Network latency**: 
   - Test your internet speed
   - Try at different times

2. **Complex prompts**:
   - Large requests take longer to process
   - Try breaking into smaller requests

3. **OpenAI service status**:
   - Check https://status.openai.com/

4. **Rate limiting**:
   - You may be approaching rate limits
   - Check OpenAI dashboard for usage

## Plugin Not Loading

### Module Failed to Load

**Problem**: Error: "Plugin 'ChatGPTEditor' failed to load because module 'ChatGPTEditor' could not be loaded"

**Solutions**:

1. **Rebuild the plugin**:
   - Close Unreal Editor
   - Delete `Binaries` and `Intermediate` folders in plugin directory
   - Right-click .uproject → Generate Visual Studio project files
   - Build in Visual Studio

2. **Check for missing dependencies**:
   - Ensure PythonScriptPlugin is installed and enabled
   - Verify all required modules in Build.cs

3. **Check for DLL conflicts**:
   - On Windows, check for missing runtime dependencies
   - Install Visual C++ Redistributables if needed

4. **Review detailed error**:
   ```
   Saved/Logs/YourProject.log
   ```
   Look for "ChatGPTEditor" errors

### Tab Doesn't Appear in Window Menu

**Problem**: "Window → ChatGPT" option is missing.

**Solutions**:

1. **Verify plugin is enabled**:
   - Edit → Plugins → Search "ChatGPT"
   - Checkbox should be checked

2. **Restart editor after enabling**

3. **Check startup code**:
   - Ensure module startup is successful
   - Check Output Log for initialization errors

4. **Manually invoke tab** (if startup succeeded):
   ```cpp
   // In Developer Console (~ key)
   // This shouldn't be necessary, but can help diagnose
   ```

## Feature-Specific Issues

### Asset Automation Not Working

**Problem**: Asset creation/modification commands not executing.

**Solutions**:

1. **Enable permission**:
   - Check "Allow Asset Write" in ChatGPT window
   - Confirm warning dialog

2. **Use correct syntax**:
   ```
   ✓ "Create material MyMaterial"
   ✓ "Create texture MyTexture"
   ✗ "Make a material" (too vague)
   ```

3. **Check audit log**:
   ```
   YourProject/Saved/ChatGPTEditor/audit.log
   ```

4. **Verify content browser is accessible**

### Console Commands Not Executing

**Problem**: Console command requests ignored or failing.

**Solutions**:

1. **Enable permission**:
   - Check "Allow Console Commands"

2. **Check command syntax**:
   ```
   ✓ "Execute console command `stat fps`"
   ✓ "Run `stat unit`"
   ```

3. **Verify command exists**:
   - Test in regular console (~ key)

4. **Check blacklist**:
   - Commands like `exit`, `quit` are blacklisted

5. **Review logs**:
   - Output Log for execution errors
   - Audit log for security denials

### Python Scripts Not Running

**Problem**: Python script generation works but execution fails.

**Solutions**:

1. **Enable Python support**:
   - Edit → Plugins → Enable "Python Editor Script Plugin"
   - Restart editor

2. **Enable permission**:
   - Check "Allow Python Scripting"

3. **Check script validation**:
   - Scripts with dangerous operations are blocked
   - Review security warnings carefully

4. **Test Python manually**:
   - Try simple Python in regular Python console
   - Verify Python plugin is working

5. **Check for forbidden operations**:
   ```python
   # Blocked:
   import subprocess  # System access
   import os         # File system (some operations)
   
   # Allowed:
   import unreal     # Unreal Engine API
   ```

### Scene Editing Not Working

**Problem**: Actor spawn/move/delete commands not working.

**Solutions**:

1. **Enable permission**:
   - Check "Allow Scene Editing"

2. **Have a level open**:
   - Scene editing requires an active level

3. **Use correct syntax**:
   ```
   ✓ "Spawn a cube at 0,0,100"
   ✓ "Move PlayerStart to 100,200,300"
   ✓ "Delete all cubes"
   ```

4. **Check preview dialog**:
   - Approve changes in preview dialog

5. **Verify no protected actors**:
   - Some actors can't be modified (safety feature)

### Blueprint Assistant Not Generating

**Problem**: Blueprint generation requests not working.

**Solutions**:

1. **Use correct prompt format**:
   ```
   ✓ "Generate a blueprint that spawns an actor when E is pressed"
   ✗ "Make a blueprint" (too vague)
   ```

2. **Check preview dialog**:
   - Must approve before generation

3. **Verify Blueprint Graph modules**:
   - Required for Blueprint creation
   - Check plugin dependencies

## Performance Issues

### Editor Lag When Using Plugin

**Problem**: Unreal Editor becomes slow when plugin is active.

**Solutions**:

1. **Large conversation history**:
   - Click "Clear" to reset conversation
   - History is kept in memory

2. **Network timeout**:
   - Slow API responses can block UI temporarily
   - Wait for response or clear request

3. **Audit log size**:
   - Very large audit logs can slow file writes
   - Archive or clear old logs periodically

4. **Too many concurrent requests**:
   - Wait for current request to complete
   - Don't spam the send button

### High Memory Usage

**Problem**: Plugin using excessive memory.

**Solutions**:

1. **Clear conversation history regularly**

2. **Restart plugin**:
   - Disable and re-enable in Plugin Manager

3. **Check for memory leaks**:
   - Report if issue persists

## Build and Compilation Errors

### Missing Module Dependencies

**Error**: `error: cannot open include file: 'PythonScriptPlugin/...'`

**Solution**:
- Ensure all dependencies in .Build.cs are installed
- Enable PythonScriptPlugin

### Linker Errors

**Error**: `unresolved external symbol`

**Solutions**:

1. **Clean and rebuild**:
   ```
   1. Close editor
   2. Delete Binaries/ and Intermediate/
   3. Generate project files
   4. Rebuild
   ```

2. **Check module dependencies** in ChatGPTEditor.Build.cs

3. **Verify Unreal Engine installation**

### C++ Compilation Errors

**Error**: Various C++ syntax or compilation errors

**Solutions**:

1. **Check Unreal Engine version**:
   - Plugin requires UE 5.5+
   - Some APIs may differ in other versions

2. **Ensure correct Visual Studio version**:
   - UE 5.5 requires VS 2022

3. **Update to latest plugin version**:
   - Check for updates

## Logging and Diagnostics

### Enabling Detailed Logging

Add to `Config/DefaultEngine.ini`:

```ini
[Core.Log]
LogChatGPTEditor=Verbose
LogHttp=Verbose
LogJson=Log
```

### Viewing Logs

**Output Log** (real-time):
```
Window → Developer Tools → Output Log
```

**Log File** (persistent):
```
YourProject/Saved/Logs/YourProject.log
```

**Audit Log** (plugin-specific):
```
YourProject/Saved/ChatGPTEditor/audit.log
```

### Common Log Messages

```
# Success
[LogChatGPTEditor] AuditLogger initialized successfully
[LogChatGPTEditor] Log path: .../audit.log

# Warnings
[LogChatGPTEditor] Warning: AuditLogger already initialized

# Errors
[LogChatGPTEditor] Error: Failed to open audit log file
[LogChatGPTEditor] Error: Failed to create audit log directory
```

### Collecting Information for Bug Reports

When reporting issues, include:

1. **Unreal Engine version**
2. **Plugin version**
3. **Operating system**
4. **Relevant log excerpts**:
   ```
   Output Log (filter: LogChatGPTEditor)
   YourProject.log (last 100 lines)
   audit.log (if relevant)
   ```
5. **Steps to reproduce**
6. **Expected vs actual behavior**
7. **Screenshots** (if applicable)

## Still Having Issues?

If none of these solutions work:

1. **Check existing GitHub issues**:
   - https://github.com/Mittenzx/ChatGPTEditor/issues

2. **Create a new issue**:
   - Use the bug report template
   - Include all diagnostic information

3. **Join discussions**:
   - GitHub Discussions for questions and help

4. **Review documentation**:
   - README.md for basic usage
   - SECURITY.md for security features
   - CHANGELOG.md for version history

---

**Note**: This plugin is under active development. Some issues may be resolved in newer versions. Always ensure you're using the latest release.
