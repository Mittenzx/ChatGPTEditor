# Blueprint Scripting Assistant - Security Documentation

## Security Model

The Blueprint Scripting Assistant implements a security-first design with multiple layers of protection:

### 1. Preview and Approval Workflow

**Design Principle:** Never execute AI-generated code without human review

**Implementation:**
- All Blueprint generation requests go through a preview dialog
- Users must explicitly click "Approve & Create" to proceed
- Preview shows:
  - Original user prompt
  - AI-generated description
  - List of nodes to be created
  - Connections between nodes
  - Security warning
- Users can reject at any time by clicking "Reject"

**Code Location:**
- Preview dialog: `SBlueprintAssistantPanel.h/cpp`
- Preview workflow: `SChatGPTWindow::ShowBlueprintPreview()`

### 2. Permission System

**Design Principle:** Dangerous operations require explicit permission

**Implementation:**
- Blueprint generation requires "Allow Asset Write Operations" to be enabled
- Permission toggles are OFF by default
- Enabling any permission shows a warning dialog
- Permissions can be disabled at any time

**Permission Levels:**
1. **Allow Asset Write Operations (DANGEROUS)**
   - Required for: Blueprint generation
   - Risk: Can modify project assets
   - Default: OFF

2. **Allow Console Commands (DANGEROUS)**
   - Not currently used by Blueprint Assistant
   - Future: Could enable automated testing
   - Default: OFF

3. **Allow File I/O Operations (DANGEROUS)**
   - Not currently used by Blueprint Assistant
   - Future: Could enable Blueprint export/import
   - Default: OFF

### 3. Audit Logging

**Design Principle:** All AI-assisted operations must be tracked

**Implementation:**
- Singleton audit log (`FBlueprintAuditLog`)
- Logs every operation with timestamp
- Tracks user prompts, AI responses, and decisions
- Can be exported for review

**Logged Events:**
1. **Generation Request**
   - User prompt
   - Generated content
   - Timestamp

2. **Preview Shown**
   - Content shown in preview
   - Timestamp

3. **User Approval**
   - Blueprint name
   - Timestamp
   - Sets `bWasApproved = true`

4. **User Rejection**
   - Rejection reason
   - Timestamp

5. **Explanation Request**
   - Blueprint name
   - Generated explanation
   - Timestamp

**Code Location:**
- Audit system: `BlueprintAuditLog.h/cpp`
- Logging calls throughout `SChatGPTWindow.cpp`

### 4. No Direct Code Execution

**Design Principle:** AI cannot execute code or commands directly

**Implementation:**
- No `eval()` or equivalent functions
- No automatic Blueprint creation
- No file system modifications without explicit approval
- All operations go through Unreal Engine's standard APIs

### 5. Input Validation

**Design Principle:** Validate all user inputs and API responses

**Implementation:**
- Empty prompt check before API calls
- API key validation before requests
- HTTP response code validation
- JSON parsing with error handling
- Content validation before preview

**Validation Points:**
1. User prompt not empty
2. API key is set
3. HTTP response is successful (200)
4. JSON response is well-formed
5. Required fields present in response

## Security Best Practices for Users

### Before Using Blueprint Assistant

1. **Backup Your Project**
   - Use version control (Git)
   - Create a backup before enabling permissions
   - Test in a separate project first

2. **Review Permissions**
   - Only enable "Allow Asset Write Operations" when needed
   - Disable immediately after use
   - Understand the risks

3. **Review API Key Security**
   - Store in environment variable
   - Never commit to version control
   - Use separate keys for dev/prod

### During Blueprint Generation

1. **Review Previews Carefully**
   - Read the generated description
   - Check the nodes list
   - Verify connections make sense
   - Look for unexpected operations

2. **Reject if Uncertain**
   - When in doubt, reject
   - Request a different prompt
   - Consult documentation or experts

3. **Monitor Audit Logs**
   - Export regularly
   - Review for unusual activity
   - Keep for compliance records

### After Blueprint Operations

1. **Test Generated Blueprints**
   - Verify behavior matches expectations
   - Check for unintended side effects
   - Test edge cases

2. **Review Audit Logs**
   - Confirm all operations were intentional
   - Look for anomalies
   - Document any issues

3. **Disable Permissions**
   - Turn off "Allow Asset Write Operations"
   - Keep permissions OFF by default

## Security Considerations for Developers

### If Extending This Plugin

1. **Maintain Preview/Approval Pattern**
   - Never bypass the preview dialog
   - Always require explicit approval
   - Log all operations

2. **Follow Permission Model**
   - Check permissions before operations
   - Show clear warnings
   - Default to most restrictive

3. **Add Comprehensive Logging**
   - Log all AI interactions
   - Include timestamps and user context
   - Make logs exportable

4. **Validate All Inputs**
   - Never trust AI output directly
   - Validate before passing to APIs
   - Handle errors gracefully

5. **Document Security Impact**
   - Update security docs
   - Add usage examples
   - Include warnings in UI

## Threat Model

### Potential Threats

1. **Malicious AI Responses**
   - **Mitigation:** Preview and approval required
   - **Mitigation:** Permission system
   - **Mitigation:** Audit logging

2. **API Key Theft**
   - **Mitigation:** Environment variable storage
   - **Mitigation:** No hardcoded keys
   - **Mitigation:** User education

3. **Unintended Blueprint Creation**
   - **Mitigation:** Preview dialog shows all changes
   - **Mitigation:** Explicit approval required
   - **Mitigation:** Audit log tracks all operations

4. **Permission Escalation**
   - **Mitigation:** Warning dialogs on permission enable
   - **Mitigation:** Permissions OFF by default
   - **Mitigation:** Clear documentation of risks

5. **Data Exfiltration**
   - **Mitigation:** No automatic data sending
   - **Mitigation:** User controls all prompts
   - **Mitigation:** Audit log tracks what's sent

### Out of Scope

- Network security (HTTPS by default)
- OpenAI API security (managed by OpenAI)
- Unreal Engine security (managed by Epic)

## Compliance and Auditing

### Audit Log Contents

All entries include:
- Timestamp (UTC)
- Operation type
- Description
- User prompt (if applicable)
- Generated content (if applicable)
- Approval status

### Export Format

Plain text file with:
- Clear section headers
- Timestamp for each operation
- All relevant details
- Approval/rejection status

### Retention Recommendations

- Keep audit logs for compliance period
- Regular exports (daily/weekly)
- Store in secure location
- Include in project documentation

## Incident Response

### If Unexpected Blueprint Created

1. Immediately disable "Allow Asset Write Operations"
2. Export audit log
3. Review log for the operation
4. Restore from backup if needed
5. Report issue to plugin maintainers

### If API Key Compromised

1. Revoke API key immediately
2. Generate new API key
3. Update environment variable
4. Review audit logs for suspicious activity
5. Monitor API usage on OpenAI dashboard

### If Suspicious AI Behavior

1. Reject the operation
2. Export audit log
3. Document the incident
4. Report to plugin maintainers
5. Review OpenAI content policy

## Version History

### 1.1.0
- Initial security implementation
- Preview and approval workflow
- Audit logging system
- Permission-based access control
- Documentation and examples

## Contact

For security issues:
- Open a GitHub issue with "Security:" prefix
- Do not disclose vulnerabilities publicly
- Include audit logs if relevant
