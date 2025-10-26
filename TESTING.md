# ChatGPTEditor Console and Scripting Test Examples

This document provides examples for testing the Editor Console and Python Scripting features.

## Console Command Examples

### Safe Commands (No Confirmation Required)
These commands are whitelisted and will execute without additional confirmation:

```
User: "Execute the console command `stat fps`"
Expected: Command executes immediately, shows FPS stats in viewport

User: "Run `stat unit` to show performance metrics"
Expected: Command executes, displays unit time stats

User: "Execute `help`"
Expected: Shows help information in console
```

### Unsafe Commands (Confirmation Required)
These commands require user confirmation:

```
User: "Execute the console command `r.SetNearClipPlane 1.0`"
Expected: Shows confirmation dialog, executes if confirmed

User: "Run `t.MaxFPS 60`"
Expected: Shows confirmation dialog (contains "set" pattern)
```

### Blacklisted Commands (Never Allowed)
These commands are rejected:

```
User: "Execute `exit`"
Expected: Shows error dialog "Command is blacklisted"

User: "Run `quit`"
Expected: Shows error dialog "Command is blacklisted"
```

## Python Script Examples

### Simple Script (With Warnings)
```
User: "Write a Python script to print all actor names in the current level"

Expected Response from ChatGPT:
```python
import unreal

# Get the editor level
editor_actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
all_actors = editor_actor_subsystem.get_all_level_actors()

# Print actor names
for actor in all_actors:
    unreal.log(f"Actor: {actor.get_name()}")
```

Expected Behavior:
1. Script preview shown in confirmation dialog
2. Warning about "import" operation
3. User confirms
4. Script executes
5. Actor names appear in Output Log
6. Entry added to audit.log
```

### Script with Forbidden Operations (Rejected)
```
User: "Write a Python script that uses subprocess to run a system command"

Expected Response from ChatGPT:
```python
import subprocess
result = subprocess.run(['ls'], capture_output=True)
```

Expected Behavior:
1. Script validation fails
2. Error dialog: "Script contains prohibited operation: import subprocess"
3. Script is NOT executed
4. Failure logged to audit.log
```

### Script with Dangerous Operations (Warning)
```
User: "Write a Python script to delete all empty folders in the Content directory"

Expected Response may contain:
```python
import os
import unreal

# Code to delete folders
```

Expected Behavior:
1. Script validation passes but shows warnings
2. Warning dialog lists dangerous operations (import os, delete)
3. User must confirm to proceed
4. If confirmed, script executes
5. Execution logged to audit.log
```

## Audit Log Verification

After running commands and scripts, check the audit log:

**Location**: `[YourProject]/Saved/ChatGPTEditor/audit.log`

**Expected Format**:
```
[2025-10-26 16:30:45.123] CONSOLE_COMMAND: stat fps | Success: YES
[2025-10-26 16:31:12.456] PYTHON_SCRIPT: import unreal; unreal.log("Test") | Success: YES
[2025-10-26 16:32:05.789] CONSOLE_COMMAND: exit | Success: NO | Error: Command is blacklisted
```

## Permission Testing

### Test Console Command Permission
1. Open ChatGPT window
2. Ensure "Allow Console Commands" is UNCHECKED
3. Ask: "Execute the console command `stat fps`"
4. Expected: No command execution, no system messages about commands

5. CHECK "Allow Console Commands"
6. Ask: "Execute the console command `stat fps`"
7. Expected: Command executes (safe command, no confirmation)

### Test Python Scripting Permission
1. Enable Python Editor Script Plugin
2. Open ChatGPT window
3. Ensure "Allow Python Scripting" is UNCHECKED
4. Ask: "Write a Python script to list all actors"
5. Expected: ChatGPT may generate script, but no execution attempt

6. CHECK "Allow Python Scripting"
7. Ask: "Write a Python script to list all actors"
8. Expected: Script generated, preview shown, can be executed

## Natural Language Parsing Tests

### Console Command Detection
```
✓ "Execute the console command `stat fps`" - Should detect
✓ "Run command: stat fps" - Should detect
✓ "Run the console command to show FPS" - Should detect if contains code block
✗ "What does stat fps do?" - Should NOT detect (informational query)
```

### Python Script Detection
```
✓ "Write a Python script to list actors" - Should detect
✓ "Generate a script to automate this task" - Should detect
✓ "Create a Python script for the editor" - Should detect
✗ "How do I use Python in Unreal?" - Should NOT detect (informational query)
```

## Testing Checklist

- [ ] Safe console commands execute without confirmation
- [ ] Unsafe console commands show confirmation dialog
- [ ] Blacklisted commands are rejected
- [ ] Python scripts show preview before execution
- [ ] Scripts with forbidden operations are rejected
- [ ] Scripts with dangerous operations show warnings
- [ ] Audit log is created and populated correctly
- [ ] Console command permission toggle works
- [ ] Python scripting permission toggle works
- [ ] Permission dialogs show appropriate warnings
- [ ] Natural language parsing works for common phrasings
- [ ] Code block extraction works for markdown format
