# Changelog

All notable changes to the ChatGPTEditor plugin will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- GitHub Actions workflows for automated builds, tests, and releases
- Comprehensive automated testing framework using Unreal's automation system
- Code quality checks and security scanning in CI/CD pipeline
- This CHANGELOG.md file to track all changes

### Changed
- Improved code documentation with inline comments
- Enhanced error handling throughout the codebase
- Refactored for better modularity and maintainability

### Security
- Enhanced security scanning in CI/CD pipeline
- Improved audit logging for sensitive operations

## [1.2.0] - 2024-10-26

### Added
- Blueprint Scripting Assistant feature
  - Generate Blueprint logic from natural language prompts
  - Explain existing Blueprint functionality
  - Preview and approval workflow for all Blueprint operations
  - Comprehensive audit logging for Blueprint operations
- Test Automation & QA feature
  - Generate unit, integration, and functional tests from natural language
  - Test code preview and confirmation workflow
  - Security validation for generated test code
  - Audit logging for all test operations
- Asset Automation feature
  - Create, modify, and delete Unreal Engine assets via natural language
  - Support for materials, textures, blueprints, and more
  - Confirmation dialogs with preview before asset changes
  - Audit logging for all asset operations
- Scene Editing & Level Design
  - Natural language-based actor spawning, movement, deletion, and modification
  - Preview & confirmation system for all scene changes
  - Protected critical actors with safety features
  - Undo support and comprehensive error handling
- Documentation Generation
  - Generate or update documentation files from natural language prompts
  - Support for README, guides, and technical documentation
  - Preview and confirmation workflow
  - Safe file operations restricted to plugin directory
- Console Command Execution
  - Execute Unreal Editor console commands via natural language
  - Command whitelist/blacklist security system
  - Confirmation required for potentially dangerous commands
  - Comprehensive audit logging
- Python Scripting Integration
  - Generate and execute Python scripts for Editor automation
  - Script validation and security warnings
  - Preview and confirmation before execution
  - Audit logging for all scripting operations
- External API & Web Integration
  - Connect to external REST APIs via natural language prompts
  - Support for GET, POST, PUT, DELETE, PATCH methods
  - Preview and approval system for all external connections
  - Automatic UE5 C++ code generation for API integration
  - Secure request validation
- Project File Management
  - Edit project configuration files (DefaultEngine.ini, .uproject, etc.)
  - File read/write operations with natural language
  - Automatic backup creation before modifications
  - Path validation and security controls
- Keyboard Shortcuts
  - Ctrl+Enter: Send message
  - Ctrl+L: Clear history
  - Ctrl+Plus/Minus: Adjust font size
  - Ctrl+0: Reset font size
- Accessibility Features
  - Adjustable font sizes (8-24pt)
  - Comprehensive tooltips on all interactive elements
  - Visual indicators and icons for better UX
  - Loading indicators for API requests
- Comprehensive Audit Logging System
  - All operations logged to `Saved/ChatGPTEditor/audit.log`
  - Timestamped entries with operation details
  - Export functionality for compliance and review
  - Thread-safe implementation

### Changed
- Improved UI with better visual hierarchy and spacing
- Enhanced security permission system with actual functionality
- Consolidated audit logging into single unified system
- Reorganized README with categorized feature list
- Updated plugin description with comprehensive feature overview

### Fixed
- Missing comma in ChatGPTEditor.Build.cs after PythonScriptPlugin
- Duplicate Description fields in ChatGPTEditor.uplugin
- Syntax errors in AuditLogger implementation
- Duplicate header files causing compilation conflicts
- Inconsistent audit logging across different modules

### Security
- All destructive operations require explicit permission toggles
- Preview and approval required for all code execution
- Blacklist system for dangerous console commands
- Script validation to prevent malicious code execution
- Automatic backup creation before file modifications
- Comprehensive audit trail for all operations

## [1.1.0] - 2024-10-25

### Added
- Enhanced documentation features
- Code explanation requests
- Basic audit logging system

### Changed
- Improved error messages
- Better API error handling

## [1.0.0] - 2024-10-20

### Added
- Initial release
- Basic ChatGPT integration with OpenAI API
- Slate-based Editor tab UI
- Conversation history
- Environment-based API key storage
- Security permission toggles (UI only)
- Support for GPT-3.5-turbo model
- Clear conversation functionality

### Security
- API key stored as environment variable (not in project files)
- Basic security toggles for future features

## Migration Guides

### Migrating to 1.2.0

#### Audit Logging Changes
If you were using custom audit logging integration:
- Old: Multiple audit logging systems (`FAuditLog`, `FAssetAuditLogEntry`)
- New: Unified `FAuditLogger` system
- Update your code to use `FAuditLogger::Get()` singleton

#### Build Configuration
- Ensure `PythonScriptPlugin` is enabled in your project
- Required modules added to Build.cs: BlueprintGraph, Kismet, KismetCompiler, GraphEditor, DesktopPlatform, AssetTools, AssetRegistry

### Migrating to 1.1.0

No breaking changes. Upgrade is straightforward.

### Migrating to 1.0.0

Initial release - no migration needed.

---

## Support

For questions or issues with any version:
- Check the README.md for detailed documentation
- Review SECURITY.md for security best practices
- Open an issue on GitHub for bugs or feature requests

## Contributors

We welcome contributions! Please see CONTRIBUTING.md for guidelines.

---

**Note**: This project is in active development. Always backup your project before upgrading to a new version and test in a non-production environment first.
