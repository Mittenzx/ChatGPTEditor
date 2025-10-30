# MCP POC Runbook

This runbook provides exact build and run commands for the ChatGPTEditor MCP (Model Context Protocol) POC implementation on Windows and Linux.
# MCP POC Build & Run Runbook

This runbook provides step-by-step instructions to build and verify the MCP (Model Context Protocol) Proof of Concept implementation in the ChatGPTEditor Unreal Engine plugin.

## Prerequisites

### Windows
- Unreal Engine 5.5 installed (typically at `C:\Program Files\Epic Games\UE_5.5`)
- Visual Studio 2022 with C++ workload

- Unreal Engine 5.5 or later installed
- Visual Studio 2022 with C++ development tools
- Git for Windows
- PowerShell 5.1 or later

### Linux
- Unreal Engine 5.5 installed
- Build essentials (gcc, g++, make)
- Git
- Bash shell

## Clean Build Instructions

### Windows

#### Step 1: Clone Repository
```powershell
git clone https://github.com/Mittenzx/ChatGPTEditor.git
cd ChatGPTEditor
```

#### Step 2: Set Environment Variables
```powershell
# Set OpenAI API key (required for testing)
[System.Environment]::SetEnvironmentVariable('OPENAI_API_KEY', 'your-api-key-here', 'User')

# Set UE_ROOT if not using default location
# [System.Environment]::SetEnvironmentVariable('UE_ROOT', 'C:\Path\To\UE_5.5', 'User')
```

#### Step 3: Generate Visual Studio Solution
```powershell
# If you have a .uproject file in your test project:
& "$env:UE_ROOT\Engine\Build\BatchFiles\GenerateProjectFiles.bat" -project="path\to\YourProject.uproject" -game -engine

# Or, to build plugin standalone:
& "$env:UE_ROOT\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="$PWD\ChatGPTEditor.uplugin" -Package="$PWD\Package" -CreateSubFolder
```

#### Step 4: Build Plugin
```powershell
# Using MSBuild (if solution generated):
msbuild YourProject.sln /p:Configuration=Development /p:Platform=Win64

# Or using Unreal Build Tool directly:
& "$env:UE_ROOT\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" ChatGPTEditor Win64 Development -Project="path\to\YourProject.uproject"
```

#### Step 5: Run Smoke Tests
```powershell
# Run the smoke test script
.\scripts\mcp_smoke_test.bat
```

### Linux

#### Step 1: Clone Repository
```bash
git clone https://github.com/Mittenzx/ChatGPTEditor.git
cd ChatGPTEditor
```

#### Step 2: Set Environment Variables
```bash
# Set OpenAI API key (required for testing)
export OPENAI_API_KEY="your-api-key-here"

# Add to shell profile for persistence
echo 'export OPENAI_API_KEY="your-api-key-here"' >> ~/.bashrc

# Set UE_ROOT if not using default location
# export UE_ROOT="/path/to/UE_5.5"
```

#### Step 3: Generate Project Files
```bash
# If you have a .uproject file in your test project:
$UE_ROOT/Engine/Build/BatchFiles/Linux/GenerateProjectFiles.sh -project="path/to/YourProject.uproject" -game -engine

# Or, to build plugin standalone:
$UE_ROOT/Engine/Build/BatchFiles/RunUAT.sh BuildPlugin -Plugin="$PWD/ChatGPTEditor.uplugin" -Package="$PWD/Package" -CreateSubFolder
```

#### Step 4: Build Plugin
```bash
# Using Unreal Build Tool:
$UE_ROOT/Engine/Binaries/Linux/UnrealBuildTool ChatGPTEditor Linux Development -Project="path/to/YourProject.uproject"
```

#### Step 5: Run Smoke Tests
```bash
# Make script executable and run
chmod +x scripts/mcp_smoke_test.sh
./scripts/mcp_smoke_test.sh
```

## Running Smoke Tests

The smoke test harness validates the following MCP POC components:

1. **Plugin Manifest Validation**: Ensures ChatGPTEditor.uplugin is valid JSON
2. **MCP Source Files Check**: Verifies MCP implementation files exist
3. **Build Artifact Validation**: Checks for compiled binaries (if built)
4. **Code Quality Checks**: Basic static analysis on MCP files

### Test Output

Success output:
```
=== MCP POC Smoke Tests ===
[PASS] Plugin manifest validation
[PASS] MCP source files check
[PASS] Build artifacts check
[PASS] Code quality checks
=== All smoke tests passed ===
```

Failure output:
```
=== MCP POC Smoke Tests ===
[PASS] Plugin manifest validation
[FAIL] MCP source files check: Missing MCPServer.h
[SKIP] Build artifacts check
[SKIP] Code quality checks
=== Smoke tests failed ===
```

## Troubleshooting

### Common Issues

#### Issue: "Unreal Engine not found"
**Solution**: Set UE_ROOT environment variable to your UE5.5 installation path
```powershell
# Windows
$env:UE_ROOT = "C:\Program Files\Epic Games\UE_5.5"

# Linux
export UE_ROOT="/path/to/UE_5.5"
```

#### Issue: "GenerateProjectFiles.bat not found"
**Solution**: Verify Unreal Engine installation is complete
```powershell
Test-Path "$env:UE_ROOT\Engine\Build\BatchFiles\GenerateProjectFiles.bat"
```

#### Issue: Build errors related to missing modules
**Solution**: Ensure all required modules are listed in ChatGPTEditor.Build.cs
```csharp
PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Slate", "SlateCore", ... });
```

#### Issue: Smoke tests fail with permission errors
**Solution**: Run with appropriate permissions
```powershell
# Windows: Run PowerShell as Administrator
# Linux: Check file permissions
chmod +x scripts/mcp_smoke_test.sh
```

## Attaching Logs and Screenshots to PR

When submitting a PR, include the following artifacts:

### Required Attachments

1. **Build Log**: Full output from build commands
   - Windows: Redirect output `> build.log 2>&1`
   - Linux: Redirect output `2>&1 | tee build.log`

2. **Smoke Test Results**: Complete output from smoke test harness
   ```powershell
   .\scripts\mcp_smoke_test.bat > smoke_test_results.log 2>&1
   ```

3. **Screenshots** (if UI changes):
   - MCP Test Window (Window → MCP Test)
   - Tool execution results
   - Any error dialogs

### Attachment Format

Create a PR comment with:
```markdown
## Test Results

### Build Log
<details>
<summary>Click to expand build log</summary>

```
[paste build.log content here]
```
</details>

### Smoke Test Results
<details>
<summary>Click to expand smoke test results</summary>

```
[paste smoke_test_results.log content here]
```
</details>

### Screenshots
![MCP Test Window](path/to/screenshot.png)
```

## Running in Unreal Editor

After successful build and plugin installation:

1. **Launch Unreal Editor**:
   ```powershell
   # Windows
   & "$env:UE_ROOT\Engine\Binaries\Win64\UnrealEditor.exe" "path\to\YourProject.uproject"
   ```

2. **Enable Plugin**:
   - Edit → Plugins
   - Search "ChatGPT Editor"
   - Check enabled
   - Restart editor

3. **Open MCP Test Window**:
   - Window → MCP Test
   - Verify tools listed (EchoTool, SpawnActorTool)

4. **Test MCP Functionality**:
   - Use "Test Echo" button to verify MCP server communication
   - Use "List Tools" to see available tools
   - Test tool execution with sample inputs

## CI/CD Integration

The smoke tests can be integrated into GitHub Actions:

```yaml
- name: Run MCP Smoke Tests
  run: |
    chmod +x scripts/mcp_smoke_test.sh
    ./scripts/mcp_smoke_test.sh
```

## Additional Resources

- [MCP Implementation Guide](../MCP_IMPLEMENTATION_GUIDE.md)
- [MCP Implementation Checklist](../MCP_IMPLEMENTATION_CHECKLIST.MD)
- [Project README](../README.md)
- [Contributing Guidelines](../CONTRIBUTING.md)

- Unreal Engine 5.5 or later (built from source)
- Clang 13.0 or later
- Git
- Bash shell

## Building the Plugin

### Windows Build

1. **Generate Visual Studio Project Files**

   ```powershell
   cd /path/to/ChatGPTEditor
   .\scripts\generate-sln.bat
   ```

   Or manually:

   ```powershell
   "C:\Program Files\Epic Games\UE_5.5\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin `
     -Plugin="${PWD}\ChatGPTEditor.uplugin" `
     -Package="${PWD}\Package" `
     -CreateSubFolder
   ```

2. **Build the Plugin**

   ```powershell
   # Using Visual Studio
   # Open ChatGPTEditor.sln and build in Visual Studio

   # Or using MSBuild from command line
   msbuild ChatGPTEditor.sln /p:Configuration=Development /p:Platform=Win64
   ```

3. **Verify Build Output**

   Check that the plugin binaries are created in:
   - `Binaries/Win64/UnrealEditor-ChatGPTEditor.dll`
   - `Intermediate/Build/Win64/UnrealEditor/Development/ChatGPTEditor/`

### Linux Build

1. **Generate Makefiles**

   ```bash
   cd /path/to/ChatGPTEditor

   # Set UE root (adjust path to your installation)
   export UE_ROOT=/home/user/UnrealEngine

   $UE_ROOT/Engine/Build/BatchFiles/RunUAT.sh BuildPlugin \
     -Plugin="$(pwd)/ChatGPTEditor.uplugin" \
     -Package="$(pwd)/Package" \
     -CreateSubFolder
   ```

2. **Build the Plugin**

   ```bash
   # Using Unreal Build Tool
   $UE_ROOT/Engine/Build/BatchFiles/Linux/Build.sh \
     ChatGPTEditor \
     Linux \
     Development \
     -Project="$(pwd)/ChatGPTEditor.uproject"
   ```

3. **Verify Build Output**

   Check that the plugin binaries are created in:
   - `Binaries/Linux/libUnrealEditor-ChatGPTEditor.so`
   - `Intermediate/Build/Linux/B4D820EA/UnrealEditor/Development/ChatGPTEditor/`

## Running the MCP Server

### In-Editor (Interactive Mode)

1. **Launch Unreal Editor**

   ```powershell
   # Windows
   "C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe" `
     -skipcompile
   ```

   ```bash
   # Linux
   $UE_ROOT/Engine/Binaries/Linux/UnrealEditor -skipcompile
   ```

2. **Open MCP Test Window**

   - From Editor: `Window` → `Developer Tools` → `MCP Test Window`
   - Or run command in Output Log: `OpenMCPTestWindow`

3. **Test MCP Server Operations**

   In the MCP Test Window:
   - Click **Initialize** to start the MCP server
   - Click **List Tools** to see available tools
   - Enter custom JSON-RPC requests in the text box
   - Click **Send Message** to execute

### Headless Mode (Automated Testing)

```powershell
# Windows
"C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
  -ExecCmds="Automation RunTests MCP" `
  -unattended -nopause -nosplash -nullrhi -log
```

```bash
# Linux
$UE_ROOT/Engine/Binaries/Linux/UnrealEditor-Cmd \
  -ExecCmds="Automation RunTests MCP" \
  -unattended -nopause -nosplash -nullrhi -log
```

## Smoke Testing

### Automated Smoke Tests

Run the provided smoke test scripts to verify basic MCP functionality:

**Windows:**

```powershell
.\tools\mcp\smoke_test.ps1
```

**Linux:**

```bash
./tools/mcp/smoke_test.sh
```

### Manual Smoke Tests

The smoke test verifies the following operations:

1. **Initialize Request**

   ```json
   {
     "jsonrpc": "2.0",
     "id": 1,
     "method": "initialize",
     "params": {
       "protocolVersion": "2024-11-05",
       "clientInfo": {
         "name": "mcp-test-client",
         "version": "1.0.0"
       }
     }
   }
   ```

   Expected: Success response with server capabilities

2. **List Tools Request**

   ```json
   {
     "jsonrpc": "2.0",
     "id": 2,
     "method": "tools/list",
     "params": {}
   }
   ```

   Expected: List of available tools including EchoTool

3. **Echo Tool Execution**

   ```json
   {
     "jsonrpc": "2.0",
     "id": 3,
     "method": "tools/call",
     "params": {
       "name": "echo",
       "arguments": {
         "message": "Hello MCP"
       }
     }
   }
   ```

   Expected: Echo response with the input message

4. **Spawn Actor Tool (if available)**

   ```json
   {
     "jsonrpc": "2.0",
     "id": 4,
     "method": "tools/call",
     "params": {
       "name": "spawn_actor",
       "arguments": {
         "actorClass": "Actor",
         "location": {"x": 0, "y": 0, "z": 0}
       }
     }
   }
   ```

   Expected: Success response or appropriate error if not implemented

## Viewing Logs

### Build Logs

**Windows:**
- Visual Studio Output window
- `%TEMP%\UnrealBuildTool\*.log`
- `Saved/Logs/UnrealBuildTool.log`

**Linux:**
- Terminal output
- `~/.local/share/Epic/UnrealBuildTool/Logs/*.log`
- `Saved/Logs/UnrealBuildTool.log`

### Runtime Logs

**Windows:**
```powershell
Get-Content Saved\Logs\ChatGPTEditor.log -Tail 50 -Wait
```

**Linux:**
```bash
tail -f Saved/Logs/ChatGPTEditor.log
```

### MCP-Specific Logs

MCP operation logs are written to:
- `Saved/Logs/ChatGPTEditor.log` (search for `[MCP]` prefix)
- `logs/mcp_smoke_test_*.log` (smoke test output)

## Troubleshooting

### Build Failures

**Issue:** Plugin fails to compile
- Check Unreal Engine version (requires 5.5+)
- Verify Visual Studio/Clang compiler version
- Clean intermediate files: `rm -rf Binaries/ Intermediate/`
- Regenerate project files

**Issue:** Missing dependencies
- Ensure all required modules are listed in `ChatGPTEditor.Build.cs`
- Verify `ChatGPTEditor.uplugin` module list

### Runtime Failures

**Issue:** MCP Server fails to initialize
- Check editor logs for initialization errors
- Verify JSON parsing libraries are available
- Ensure no conflicting plugins

**Issue:** Tools not listed
- Verify tool registration in module startup
- Check tool implementation inherits from `IMCPTool`
- Review tool registration logs

**Issue:** JSON-RPC errors
- Validate JSON syntax in requests
- Check method names match protocol spec
- Verify parameter structure

## CI/CD Integration

The project includes a GitHub Actions workflow at `.github/workflows/mcp-poc-smoke.yml` that:

1. Checks out the repository
2. Sets up build environment (on Linux runner)
3. Runs the smoke test script
4. Reports pass/fail status
5. Uploads test logs as artifacts

To run locally in a similar environment:

```bash
# Simulate CI environment
export CI=true
./tools/mcp/smoke_test.sh
echo "Exit code: $?"
```

## Next Steps

After verifying the POC build and smoke tests:

1. Review smoke test logs in `logs/` directory
2. Attach logs to PR for reviewer validation
3. Address any failures or warnings
4. Proceed with Phase 1-B tasks (see `MCP_IMPLEMENTATION_CHECKLIST.MD`)

## References

- [MCP Implementation Guide](../MCP_IMPLEMENTATION_GUIDE.md)
- [MCP Implementation Checklist](../MCP_IMPLEMENTATION_CHECKLIST.MD)
- [Plugin Testing Documentation](../TESTING.md)
- [Model Context Protocol Specification](https://modelcontextprotocol.io/)
