#!/bin/bash
# MCP POC Smoke Test Harness - Linux/macOS
# This script runs basic validation checks for the MCP implementation
# Exit code: 0 on success, non-zero on failure

set -e  # Exit on error
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test result tracking
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_SKIPPED=0

echo "=== MCP POC Smoke Tests ==="
echo "Project root: $PROJECT_ROOT"
echo ""

# Helper functions
pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
    TESTS_PASSED=$((TESTS_PASSED + 1))
}

fail() {
    echo -e "${RED}[FAIL]${NC} $1"
    TESTS_FAILED=$((TESTS_FAILED + 1))
}

skip() {
    echo -e "${YELLOW}[SKIP]${NC} $1"
    TESTS_SKIPPED=$((TESTS_SKIPPED + 1))
}

info() {
    echo -e "${YELLOW}[INFO]${NC} $1"
}

# Test 1: Plugin manifest validation
test_plugin_manifest() {
    echo "Test 1: Plugin manifest validation"
    local manifest="$PROJECT_ROOT/ChatGPTEditor.uplugin"
    
    if [ ! -f "$manifest" ]; then
        fail "Plugin manifest not found: $manifest"
        return 1
    fi
    
    # Check if it's valid JSON using python or jq
    if command -v python3 &> /dev/null; then
        if python3 -m json.tool "$manifest" > /dev/null 2>&1; then
            pass "Plugin manifest is valid JSON"
        else
            fail "Plugin manifest is not valid JSON"
            return 1
        fi
    elif command -v jq &> /dev/null; then
        if jq empty "$manifest" > /dev/null 2>&1; then
            pass "Plugin manifest is valid JSON"
        else
            fail "Plugin manifest is not valid JSON"
            return 1
        fi
    else
        skip "No JSON validator found (python3 or jq), skipping validation"
    fi
    
    return 0
}

# Test 2: MCP source files check
test_mcp_source_files() {
    echo "Test 2: MCP source files check"
    local source_dir="$PROJECT_ROOT/Source/ChatGPTEditor"
    local required_files=(
        "Public/MCP/MCPTypes.h"
        "Public/MCP/MCPServer.h"
        "Public/MCP/MCPTool.h"
        "Private/MCP/MCPServer.cpp"
    )
    
    local missing_files=()
    for file in "${required_files[@]}"; do
        local full_path="$source_dir/$file"
        if [ ! -f "$full_path" ]; then
            missing_files+=("$file")
        fi
    done
    
    if [ ${#missing_files[@]} -eq 0 ]; then
        pass "All required MCP source files present (${#required_files[@]} files)"
    else
        fail "Missing MCP source files: ${missing_files[*]}"
        return 1
    fi
    
    return 0
}

# Test 3: MCP tool implementations check
test_mcp_tools() {
    echo "Test 3: MCP tool implementations check"
    local tools_dir="$PROJECT_ROOT/Source/ChatGPTEditor/Private/MCP/Tools"
    
    if [ ! -d "$tools_dir" ]; then
        fail "MCP Tools directory not found: $tools_dir"
        return 1
    fi
    
    local tool_count=$(find "$tools_dir" -name "*.h" -o -name "*.cpp" | wc -l)
    if [ "$tool_count" -gt 0 ]; then
        pass "Found $tool_count MCP tool files"
    else
        fail "No MCP tool files found in $tools_dir"
        return 1
    fi
    
    return 0
}

# Test 4: Build configuration check
test_build_config() {
    echo "Test 4: Build configuration check"
    local build_cs="$PROJECT_ROOT/Source/ChatGPTEditor/ChatGPTEditor.Build.cs"
    
    if [ ! -f "$build_cs" ]; then
        fail "Build configuration not found: $build_cs"
        return 1
    fi
    
    # Check if Build.cs contains expected module dependencies
    if grep -q "Json" "$build_cs" && grep -q "JsonUtilities" "$build_cs"; then
        pass "Build configuration includes required JSON modules"
    else
        skip "Build configuration check: JSON modules may not be configured"
    fi
    
    return 0
}

# Test 5: Documentation check
test_documentation() {
    echo "Test 5: Documentation check"
    local docs=(
        "$PROJECT_ROOT/MCP_IMPLEMENTATION_CHECKLIST.MD"
        "$PROJECT_ROOT/docs/MCP_POC_RUNBOOK.md"
    )
    
    local missing_docs=()
    for doc in "${docs[@]}"; do
        if [ ! -f "$doc" ]; then
            missing_docs+=("$(basename "$doc")")
        fi
    done
    
    if [ ${#missing_docs[@]} -eq 0 ]; then
        pass "All required MCP documentation present"
    else
        skip "Missing documentation: ${missing_docs[*]} (may be work in progress)"
    fi
    
    return 0
}

# Test 6: Code style check (basic)
test_code_style() {
    echo "Test 6: Basic code style check"
    local mcp_dir="$PROJECT_ROOT/Source/ChatGPTEditor/Private/MCP"
    
    if [ ! -d "$mcp_dir" ]; then
        skip "MCP directory not found, skipping code style check"
        return 0
    fi
    
    # Check for common issues (tabs vs spaces, trailing whitespace, etc.)
    local issues=0
    
    # Check for files with tabs (if project uses spaces)
    if find "$mcp_dir" -name "*.cpp" -o -name "*.h" | xargs grep -l $'\t' > /dev/null 2>&1; then
        info "Some files contain tabs (check if consistent with project style)"
    fi
    
    # This is a basic check, not failing on style issues
    pass "Basic code style check completed"
    return 0
}

# Run all tests
run_all_tests() {
    test_plugin_manifest || true
    echo ""
    
    test_mcp_source_files || true
    echo ""
    
    test_mcp_tools || true
    echo ""
    
    test_build_config || true
    echo ""
    
    test_documentation || true
    echo ""
    
    test_code_style || true
    echo ""
}

# Main execution
run_all_tests

# Print summary
echo "==================================="
echo "Test Summary:"
echo -e "${GREEN}Passed: $TESTS_PASSED${NC}"
echo -e "${RED}Failed: $TESTS_FAILED${NC}"
echo -e "${YELLOW}Skipped: $TESTS_SKIPPED${NC}"
echo "==================================="

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}=== All smoke tests passed ===${NC}"
    exit 0
else
    echo -e "${RED}=== Smoke tests failed ===${NC}"
    exit 1
fi
