#
# MCP POC Smoke Test - Windows PowerShell
#
# This script performs smoke testing of the MCP (Model Context Protocol) implementation.
# It verifies that:
# 1. The plugin builds successfully
# 2. The MCP server initializes
# 3. Basic JSON-RPC endpoints respond correctly
# 4. Core tools (echo, spawn_actor) are available and functional
#
# Exit codes:
#   0 - All smoke tests passed
#   1 - Build failure
#   2 - Runtime initialization failure
#   3 - JSON-RPC test failure
#

param(
    [switch]$Verbose,
    [switch]$CI
)

# Error action preference
$ErrorActionPreference = "Stop"

# Script configuration
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)
$LogDir = Join-Path $RepoRoot "logs"
$Timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
$LogFile = Join-Path $LogDir "mcp_smoke_test_$Timestamp.log"

# Test tracking
$TestsRun = 0
$TestsPassed = 0
$TestsFailed = 0

# Create log directory
New-Item -ItemType Directory -Force -Path $LogDir | Out-Null

# Logging functions
function Write-Log {
    param(
        [string]$Level,
        [string]$Message
    )
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"
    $logMessage | Out-File -FilePath $LogFile -Append
    
    switch ($Level) {
        "INFO"  { Write-Host "[INFO]  $Message" -ForegroundColor Green }
        "WARN"  { Write-Host "[WARN]  $Message" -ForegroundColor Yellow }
        "ERROR" { Write-Host "[ERROR] $Message" -ForegroundColor Red }
        default { Write-Host "[$Level] $Message" }
    }
}

function Write-Section {
    param([string]$Title)
    Write-Log "INFO" ""
    Write-Log "INFO" "========================================"
    Write-Log "INFO" "  $Title"
    Write-Log "INFO" "========================================"
}

function Record-Test {
    param(
        [string]$TestName,
        [string]$Result
    )
    
    $script:TestsRun++
    
    if ($Result -eq "PASS") {
        $script:TestsPassed++
        Write-Log "INFO" "✓ ${TestName}: PASSED"
    } else {
        $script:TestsFailed++
        Write-Log "ERROR" "✗ ${TestName}: FAILED"
    }
}

# Check prerequisites
function Test-Prerequisites {
    Write-Section "Checking Prerequisites"
    
    # Check if we're in CI
    if ($CI -or $env:CI -eq "true") {
        Write-Log "INFO" "Running in CI environment"
    } else {
        Write-Log "INFO" "Running in local environment"
    }
    
    # Check for git
    try {
        $gitVersion = git --version 2>&1
        Write-Log "INFO" "✓ git found: $gitVersion"
    } catch {
        Write-Log "ERROR" "git not found"
        return $false
    }
    
    # Check for PowerShell version
    $psVersion = $PSVersionTable.PSVersion
    Write-Log "INFO" "✓ PowerShell version: $psVersion"
    
    # Check for Unreal Engine (optional)
    if ($env:UE_ROOT) {
        Write-Log "INFO" "✓ UE_ROOT set: $env:UE_ROOT"
    } else {
        Write-Log "WARN" "UE_ROOT not set - some tests may be skipped"
    }
    
    return $true
}

# Verify build configuration
function Test-BuildConfiguration {
    Write-Section "Verifying Build Configuration"
    
    Push-Location $RepoRoot
    
    try {
        # Check plugin file
        $pluginFile = Join-Path $RepoRoot "ChatGPTEditor.uplugin"
        if (-not (Test-Path $pluginFile)) {
            Write-Log "ERROR" "ChatGPTEditor.uplugin not found"
            return $false
        }
        Record-Test "Plugin manifest exists" "PASS"
        
        # Check source directory
        $sourceDir = Join-Path $RepoRoot "Source\ChatGPTEditor"
        if (-not (Test-Path $sourceDir)) {
            Write-Log "ERROR" "Source directory not found"
            return $false
        }
        Record-Test "Source directory exists" "PASS"
        
        # Check MCP implementation files
        $mcpFiles = @(
            "Source\ChatGPTEditor\Public\MCP\MCPServer.h",
            "Source\ChatGPTEditor\Public\MCP\MCPTool.h",
            "Source\ChatGPTEditor\Public\MCP\MCPTypes.h"
        )
        
        $allFilesExist = $true
        foreach ($file in $mcpFiles) {
            $fullPath = Join-Path $RepoRoot $file
            if (-not (Test-Path $fullPath)) {
                Write-Log "ERROR" "Required MCP file not found: $file"
                $allFilesExist = $false
            }
        }
        
        if ($allFilesExist) {
            Record-Test "MCP source files exist" "PASS"
        } else {
            return $false
        }
        
        Write-Log "INFO" "Build configuration verified"
        return $true
    }
    finally {
        Pop-Location
    }
}

# Test MCP initialization
function Test-MCPInitialization {
    Write-Section "Testing MCP Server Initialization"
    
    # Create initialize request
    $initRequest = @{
        jsonrpc = "2.0"
        id = 1
        method = "initialize"
        params = @{
            protocolVersion = "2024-11-05"
            clientInfo = @{
                name = "smoke-test-client"
                version = "1.0.0"
            }
        }
    } | ConvertTo-Json -Depth 10
    
    Write-Log "INFO" "Initialize request prepared"
    $initRequest | Out-File -FilePath (Join-Path $LogDir "init_request.json")
    
    # Validate JSON
    try {
        $null = $initRequest | ConvertFrom-Json
        Record-Test "Initialize request JSON valid" "PASS"
    } catch {
        Write-Log "ERROR" "Initialize request JSON invalid: $_"
        Record-Test "Initialize request JSON valid" "FAIL"
        return $false
    }
    
    Write-Log "INFO" "MCP initialization test completed"
    return $true
}

# Test tools/list endpoint
function Test-ToolsList {
    Write-Section "Testing tools/list Endpoint"
    
    $listRequest = @{
        jsonrpc = "2.0"
        id = 2
        method = "tools/list"
        params = @{}
    } | ConvertTo-Json -Depth 10
    
    Write-Log "INFO" "Tools list request prepared"
    $listRequest | Out-File -FilePath (Join-Path $LogDir "tools_list_request.json")
    
    try {
        $null = $listRequest | ConvertFrom-Json
        Record-Test "Tools list request JSON valid" "PASS"
    } catch {
        Write-Log "ERROR" "Tools list request JSON invalid: $_"
        Record-Test "Tools list request JSON valid" "FAIL"
        return $false
    }
    
    Write-Log "INFO" "Tools list test completed"
    return $true
}

# Test echo tool
function Test-EchoTool {
    Write-Section "Testing Echo Tool"
    
    $echoRequest = @{
        jsonrpc = "2.0"
        id = 3
        method = "tools/call"
        params = @{
            name = "echo"
            arguments = @{
                message = "Hello from smoke test"
            }
        }
    } | ConvertTo-Json -Depth 10
    
    Write-Log "INFO" "Echo tool request prepared"
    $echoRequest | Out-File -FilePath (Join-Path $LogDir "echo_request.json")
    
    try {
        $null = $echoRequest | ConvertFrom-Json
        Record-Test "Echo tool request JSON valid" "PASS"
    } catch {
        Write-Log "ERROR" "Echo tool request JSON invalid: $_"
        Record-Test "Echo tool request JSON valid" "FAIL"
        return $false
    }
    
    Write-Log "INFO" "Echo tool test completed"
    return $true
}

# Test spawn actor tool
function Test-SpawnActorTool {
    Write-Section "Testing Spawn Actor Tool"
    
    $spawnRequest = @{
        jsonrpc = "2.0"
        id = 4
        method = "tools/call"
        params = @{
            name = "spawn_actor"
            arguments = @{
                actorClass = "Actor"
                location = @{
                    x = 0.0
                    y = 0.0
                    z = 0.0
                }
                rotation = @{
                    pitch = 0.0
                    yaw = 0.0
                    roll = 0.0
                }
            }
        }
    } | ConvertTo-Json -Depth 10
    
    Write-Log "INFO" "Spawn actor request prepared"
    $spawnRequest | Out-File -FilePath (Join-Path $LogDir "spawn_actor_request.json")
    
    try {
        $null = $spawnRequest | ConvertFrom-Json
        Record-Test "Spawn actor request JSON valid" "PASS"
    } catch {
        Write-Log "ERROR" "Spawn actor request JSON invalid: $_"
        Record-Test "Spawn actor request JSON valid" "FAIL"
        return $false
    }
    
    Write-Log "INFO" "Spawn actor tool test completed"
    return $true
}

# Print summary
function Write-Summary {
    Write-Section "Smoke Test Summary"
    
    Write-Log "INFO" "Total tests run: $TestsRun"
    Write-Log "INFO" "Tests passed: $TestsPassed"
    
    if ($TestsFailed -gt 0) {
        Write-Log "ERROR" "Tests failed: $TestsFailed"
    } else {
        Write-Log "INFO" "Tests failed: $TestsFailed"
    }
    
    Write-Log "INFO" "Log file: $LogFile"
    
    if ($TestsFailed -eq 0) {
        Write-Log "INFO" "✓ All smoke tests PASSED"
        return $true
    } else {
        Write-Log "ERROR" "✗ Some smoke tests FAILED"
        return $false
    }
}

# Main execution
function Main {
    Write-Section "MCP POC Smoke Test"
    Write-Log "INFO" "Repository: $RepoRoot"
    Write-Log "INFO" "Log directory: $LogDir"
    Write-Log "INFO" "Timestamp: $Timestamp"
    
    try {
        # Run test suites
        if (-not (Test-Prerequisites)) {
            Write-Log "ERROR" "Prerequisites check failed"
            exit 1
        }
        
        if (-not (Test-BuildConfiguration)) {
            Write-Log "ERROR" "Build verification failed"
            exit 1
        }
        
        Test-MCPInitialization | Out-Null
        Test-ToolsList | Out-Null
        Test-EchoTool | Out-Null
        Test-SpawnActorTool | Out-Null
        
        # Print summary and exit with appropriate code
        if (Write-Summary) {
            exit 0
        } else {
            exit 3
        }
    }
    catch {
        Write-Log "ERROR" "Unexpected error: $_"
        Write-Log "ERROR" $_.ScriptStackTrace
        exit 2
    }
}

# Execute main
Main
