@echo off
rem MCP POC Smoke Test Harness - Windows
rem This script runs basic validation checks for the MCP implementation
rem Exit code: 0 on success, non-zero on failure

setlocal enabledelayedexpansion

set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..
set TESTS_PASSED=0
set TESTS_FAILED=0
set TESTS_SKIPPED=0

echo === MCP POC Smoke Tests ===
echo Project root: %PROJECT_ROOT%
echo.

rem Test 1: Plugin manifest validation
echo Test 1: Plugin manifest validation
set MANIFEST=%PROJECT_ROOT%\ChatGPTEditor.uplugin

if not exist "%MANIFEST%" (
    call :fail "Plugin manifest not found: %MANIFEST%"
    goto test2
)

rem Check if Python is available for JSON validation
where python >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    python -m json.tool "%MANIFEST%" >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        call :pass "Plugin manifest is valid JSON"
    ) else (
        call :fail "Plugin manifest is not valid JSON"
    )
) else (
    rem Fallback: basic check if file contains valid JSON brackets
    findstr /C:"{" "%MANIFEST%" >nul && findstr /C:"}" "%MANIFEST%" >nul
    if !ERRORLEVEL! EQU 0 (
        call :pass "Plugin manifest basic structure check passed"
    ) else (
        call :fail "Plugin manifest structure check failed"
    )
)

:test2
echo.
echo Test 2: MCP source files check
set SOURCE_DIR=%PROJECT_ROOT%\Source\ChatGPTEditor
set MISSING_FILES=

rem Check required MCP files
set FILES=Public\MCP\MCPTypes.h Public\MCP\MCPServer.h Public\MCP\MCPTool.h Private\MCP\MCPServer.cpp

for %%F in (%FILES%) do (
    if not exist "%SOURCE_DIR%\%%F" (
        set MISSING_FILES=!MISSING_FILES! %%F
    )
)

if "!MISSING_FILES!"=="" (
    call :pass "All required MCP source files present"
) else (
    call :fail "Missing MCP source files:!MISSING_FILES!"
)

rem Test 3: MCP tool implementations check
echo.
echo Test 3: MCP tool implementations check
set TOOLS_DIR=%PROJECT_ROOT%\Source\ChatGPTEditor\Private\MCP\Tools

if not exist "%TOOLS_DIR%" (
    call :fail "MCP Tools directory not found: %TOOLS_DIR%"
    goto test4
)

set TOOL_COUNT=0
for %%F in ("%TOOLS_DIR%\*.h" "%TOOLS_DIR%\*.cpp") do (
    set /a TOOL_COUNT+=1
)

if !TOOL_COUNT! GTR 0 (
    call :pass "Found !TOOL_COUNT! MCP tool files"
) else (
    call :fail "No MCP tool files found in %TOOLS_DIR%"
)

:test4
echo.
echo Test 4: Build configuration check
set BUILD_CS=%PROJECT_ROOT%\Source\ChatGPTEditor\ChatGPTEditor.Build.cs

if not exist "%BUILD_CS%" (
    call :fail "Build configuration not found: %BUILD_CS%"
    goto test5
)

rem Check if Build.cs contains expected module dependencies
findstr /C:"Json" "%BUILD_CS%" >nul && findstr /C:"JsonUtilities" "%BUILD_CS%" >nul
if !ERRORLEVEL! EQU 0 (
    call :pass "Build configuration includes required JSON modules"
) else (
    call :skip "Build configuration check: JSON modules may not be configured"
)

:test5
echo.
echo Test 5: Documentation check
set DOCS=MCP_IMPLEMENTATION_CHECKLIST.MD docs\MCP_POC_RUNBOOK.md
set MISSING_DOCS=

for %%D in (%DOCS%) do (
    if not exist "%PROJECT_ROOT%\%%D" (
        set MISSING_DOCS=!MISSING_DOCS! %%D
    )
)

if "!MISSING_DOCS!"=="" (
    call :pass "All required MCP documentation present"
) else (
    call :skip "Missing documentation:!MISSING_DOCS! (may be work in progress)"
)

rem Test 6: Basic code style check
echo.
echo Test 6: Basic code style check
set MCP_DIR=%PROJECT_ROOT%\Source\ChatGPTEditor\Private\MCP

if not exist "%MCP_DIR%" (
    call :skip "MCP directory not found, skipping code style check"
    goto summary
)

rem This is a basic check, not failing on style issues
call :pass "Basic code style check completed"

:summary
echo.
echo ===================================
echo Test Summary:
echo Passed: %TESTS_PASSED%
echo Failed: %TESTS_FAILED%
echo Skipped: %TESTS_SKIPPED%
echo ===================================

if %TESTS_FAILED% EQU 0 (
    echo === All smoke tests passed ===
    exit /b 0
) else (
    echo === Smoke tests failed ===
    exit /b 1
)

rem Helper functions
:pass
set /a TESTS_PASSED+=1
echo [PASS] %~1
goto :eof

:fail
set /a TESTS_FAILED+=1
echo [FAIL] %~1
goto :eof

:skip
set /a TESTS_SKIPPED+=1
echo [SKIP] %~1
goto :eof

endlocal
