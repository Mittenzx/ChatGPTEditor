---
name: MCP Phase 1 Pull Request
about: Template for MCP Phase 1 POC stabilization PRs
title: '[MCP][Phase1] '
labels: 'mcp, phase-1, enhancement'
---

## Summary

<!-- Provide a clear, concise summary of the changes in this PR -->

## Rationale

<!-- Explain why these changes are necessary and how they fit into the MCP implementation plan -->

## Files Changed

<!-- List the key files modified, added, or deleted in this PR -->

- 
- 
- 

## General PR Checklist

<!-- Include in every MCP PR description -->

- [ ] Branch follows naming convention: `feature/mcp/<phase>-<short>`
- [ ] PR title follows format: `[MCP][PhaseX] Short description`
- [ ] PR includes summary, rationale, and files changed
- [ ] How to test locally (commands/editor steps) included below
- [ ] Acceptance criteria listed and test coverage described
- [ ] Linked issues/PRs and dependency notes included
- [ ] Suggested reviewers assigned
- [ ] Labels: mcp, phase-1/phase-2/phase-3, enhancement/ci/docs/test
- [ ] If API/schema changes: include schema version bump and migration notes
- [ ] CI status green (build + tests where applicable)

## Phase 1 Acceptance Criteria

### P1-A: POC Build & Run Verification
- [ ] Project builds cleanly on target platforms (Windows/Linux as applicable)
- [ ] Smoke tests run successfully
- [ ] Build and smoke test logs attached to PR
- [ ] Screenshots of MCP Test Window included (if applicable)

### P1-B: Core Functionality Validation
- [ ] MCP server initializes correctly
- [ ] Tools are registered and listed properly
- [ ] Basic tool execution (EchoTool) works as expected
- [ ] No regression in existing plugin functionality

## How to Test Locally

### Build Instructions

**Windows:**
```powershell
# Set environment variable if needed
$env:OPENAI_API_KEY = "your-api-key-here"

# Build command here
# Example: .\scripts\build.bat
```

**Linux:**
```bash
# Set environment variable if needed
export OPENAI_API_KEY="your-api-key-here"

# Build command here
# Example: ./scripts/build.sh
```

### Run Smoke Tests

**Windows:**
```powershell
.\scripts\mcp_smoke_test.bat
```

**Linux:**
```bash
chmod +x scripts/mcp_smoke_test.sh
./scripts/mcp_smoke_test.sh
```

### Manual Testing Steps

1. 
2. 
3. 

## Test Coverage

<!-- Describe what tests were added or modified, and what scenarios they cover -->

- Unit tests: 
- Integration tests: 
- Manual tests performed: 

## Attached Logs and Screenshots

### Build Log

<details>
<summary>Click to expand build log</summary>

```
[Paste build log here or attach as file]
```

</details>

### Smoke Test Results

<details>
<summary>Click to expand smoke test results</summary>

```
[Paste smoke test results here]
```

</details>

### Screenshots

<!-- Add any relevant screenshots, especially for UI changes -->

#### MCP Test Window (if applicable)
<!-- ![MCP Test Window](path/to/screenshot.png) -->

#### Test Results
<!-- ![Test Results](path/to/screenshot.png) -->

## Dependencies and Linked Issues

<!-- List any related issues, PRs, or dependencies -->

- Depends on: #
- Related to: #
- Closes: #

## Migration Notes

<!-- If this PR includes breaking changes or requires migration steps, document them here -->

N/A

## API/Schema Changes

<!-- If this PR modifies APIs or JSON schemas, document the changes here -->

N/A

## Suggested Reviewers

<!-- Tag reviewers based on their expertise -->

- **MCP Architect**: @username
- **Engine Integrator**: @username
- **Security Reviewer**: @username (if permissions/audit changes)

## Additional Notes

<!-- Any other context, concerns, or information that reviewers should know -->

## Checklist Before Merging

- [ ] All acceptance criteria met
- [ ] All CI checks passing
- [ ] Code review approved by at least one reviewer
- [ ] Documentation updated (if needed)
- [ ] No merge conflicts
- [ ] Tested in target environment(s)
