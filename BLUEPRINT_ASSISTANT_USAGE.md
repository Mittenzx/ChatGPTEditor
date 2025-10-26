# Blueprint Scripting Assistant - Usage Examples

This document provides usage examples for the Blueprint Scripting Assistant feature.

## Feature Overview

The Blueprint Scripting Assistant enables AI-powered Blueprint generation and explanation with:
- Natural language prompts for Blueprint generation
- Preview and approval workflow before any changes
- Comprehensive audit logging
- Blueprint explanation functionality

## Example 1: Generate a Health Pickup Blueprint

**User Prompt:**
```
Create a health pickup that restores 25 health points when the player overlaps it
```

**Expected Flow:**
1. User enters the prompt in the Blueprint generation field
2. User clicks "Generate Blueprint"
3. System sends request to ChatGPT API
4. Preview dialog shows:
   - User's original prompt
   - Generated description
   - List of nodes (e.g., Box Collision, OnComponentBeginOverlap, Cast to Character, etc.)
   - Connections between nodes
5. User reviews and clicks "Approve & Create" or "Reject"
6. If approved, the action is logged in the audit log
7. In full implementation, Blueprint nodes would be created

**Audit Log Entry:**
```
[2025-10-26 15:52:00] GENERATION
Description: Blueprint generation requested
User Prompt: Create a health pickup that restores 25 health points when the player overlaps it
Generated Content: {...}
Approved: Yes
```

## Example 2: Explain a Blueprint

**User Input:**
```
BP_PlayerController
```

**Expected Flow:**
1. User enters "BP_PlayerController" in the Blueprint name field
2. User clicks "Explain Blueprint"
3. System sends request to ChatGPT API
4. Explanation appears in conversation history with:
   - Summary of the Blueprint's purpose
   - Step-by-step breakdown of its logic
5. Explanation is logged in the audit log

**Audit Log Entry:**
```
[2025-10-26 15:53:00] EXPLANATION
Description: Blueprint explanation for: BP_PlayerController
Generated Content: {...}
```

## Example 3: Export Audit Log

**User Action:**
1. Click "Export Audit Log" button
2. Choose save location (e.g., `C:/Projects/MyGame/Saved/BlueprintAuditLog_2025-10-26.txt`)
3. Confirm save
4. Log file is created with all operations

**Audit Log File Format:**
```
Blueprint Scripting Assistant - Audit Log
========================================

[2025-10-26 15:52:00] GENERATION
Description: Blueprint generation requested
User Prompt: Create a health pickup that restores 25 health points when the player overlaps it
Generated Content: {...}
Approved: Yes

---

[2025-10-26 15:53:00] EXPLANATION
Description: Blueprint explanation for: BP_PlayerController
Generated Content: {...}
Approved: No

---
```

## Security Features

### Preview Dialog
- Shows all nodes and connections before creation
- Requires explicit user approval
- Can be rejected without any changes

### Audit Logging
- All operations are logged with timestamps
- Tracks user prompts and AI responses
- Records approval/rejection decisions
- Can be exported for compliance review

### Permission Requirements
- Blueprint generation requires "Allow Asset Write Operations" to be enabled
- Blueprint explanation works without special permissions
- Warning dialogs shown when enabling dangerous permissions

## Implementation Notes

**Current Implementation:**
- Preview and approval workflow is fully implemented
- Audit logging is fully implemented
- UI integration is complete
- API communication with ChatGPT is functional

**Future Enhancements:**
- Actual Blueprint node creation using Blueprint editor APIs
- Visual Blueprint selection for explanation
- Blueprint refactoring suggestions
- Integration with Blueprint debugging tools

## Testing Checklist

- [ ] Blueprint generation shows preview dialog
- [ ] Preview dialog displays user prompt correctly
- [ ] Preview dialog shows generated content
- [ ] Approve button logs approval
- [ ] Reject button logs rejection
- [ ] Blueprint explanation requests explanation from API
- [ ] Explanation appears in conversation history
- [ ] Audit log export creates file
- [ ] Audit log file contains all operations
- [ ] Permission toggle works correctly
- [ ] Error handling works for API failures
