# Contributing to ChatGPTEditor

Thank you for your interest in contributing to ChatGPTEditor! This document provides guidelines and information for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [How to Contribute](#how-to-contribute)
- [Coding Standards](#coding-standards)
- [Testing Guidelines](#testing-guidelines)
- [Pull Request Process](#pull-request-process)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Features](#suggesting-features)

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors. Please be respectful and professional in all interactions.

### Expected Behavior

- Use welcoming and inclusive language
- Be respectful of differing viewpoints and experiences
- Gracefully accept constructive criticism
- Focus on what is best for the community
- Show empathy towards other community members

## Getting Started

### Prerequisites

Before you begin, ensure you have:

- Unreal Engine 5.5 or later installed
- Visual Studio 2022 (Windows) or Xcode (macOS)
- Git for version control
- OpenAI API key for testing

### Setting Up Your Development Environment

1. **Fork the Repository**
   ```bash
   # Fork via GitHub UI, then clone your fork
   git clone https://github.com/YOUR_USERNAME/ChatGPTEditor.git
   cd ChatGPTEditor
   ```

2. **Set Up Remote**
   ```bash
   git remote add upstream https://github.com/Mittenzx/ChatGPTEditor.git
   git fetch upstream
   ```

3. **Create Test Project**
   - Create a new UE5.5 project or use an existing one
   - Copy the plugin to `YourProject/Plugins/ChatGPTEditor/`
   - Enable the plugin in the Unreal Editor

4. **Set Environment Variable**
   ```bash
   # Set your OpenAI API key
   export OPENAI_API_KEY="your-api-key-here"
   ```

## How to Contribute

### Types of Contributions

We welcome various types of contributions:

- **Bug Fixes**: Fix issues reported in GitHub Issues
- **New Features**: Implement new functionality (discuss first in an issue)
- **Documentation**: Improve or expand documentation
- **Tests**: Add or improve automated tests
- **Code Quality**: Refactoring, optimization, or cleanup

### Workflow

1. **Check Existing Issues**: Look for existing issues or create a new one
2. **Create a Branch**: Use descriptive branch names
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/bug-description
   ```

3. **Make Changes**: Implement your changes following our coding standards
4. **Test**: Ensure all tests pass and add new tests if needed
5. **Commit**: Write clear, descriptive commit messages
6. **Push**: Push your changes to your fork
7. **Pull Request**: Create a PR with a clear description

## Coding Standards

### Unreal Engine C++ Guidelines

Follow Epic Games' [Coding Standard](https://docs.unrealengine.com/5.5/en-US/epic-cplusplus-coding-standard-for-unreal-engine/):

#### Naming Conventions

```cpp
// Classes: Prefix with appropriate letter
class FMyClass { };        // F prefix for POD types
class UMyObject { };       // U prefix for UObject-derived
class AMyActor { };        // A prefix for AActor-derived
class SMyWidget { };       // S prefix for Slate widgets

// Variables
bool bIsEnabled;           // Boolean prefix with 'b'
TArray<int32> ItemCounts;  // Arrays use plural nouns
FString PlayerName;        // Clear, descriptive names

// Functions
void DoSomething();        // Verbs for actions
bool IsValid() const;      // Booleans start with Is/Has/Can
FString GetName() const;   // Getters start with Get
```

#### Code Style

```cpp
// Use modern C++ features
TSharedPtr<FMyClass> Ptr = MakeShared<FMyClass>();  // NOT new
TUniquePtr<FData> Data = MakeUnique<FData>();       // For unique ownership

// Proper error handling
if (!IsValid(MyObject))
{
    UE_LOG(LogChatGPTEditor, Error, TEXT("MyObject is invalid"));
    return;
}

// Use const correctness
const FString& GetName() const { return Name; }

// RAII for resource management
{
    FScopeLock Lock(&MyMutex);  // Automatically unlocks
    // ... critical section ...
}
```

### Documentation

#### Header File Comments

```cpp
/**
 * Brief description of the class
 * 
 * Detailed explanation of what this class does,
 * its purpose, and any important usage notes.
 */
class FMyClass
{
public:
    /**
     * Does something important
     * @param InValue The input value to process
     * @return True if successful, false otherwise
     */
    bool DoSomething(int32 InValue);
};
```

#### Implementation Comments

```cpp
void FMyClass::ComplexFunction()
{
    // Explain why, not what (code should be self-documenting for "what")
    // This is necessary because of XYZ limitation in Unreal Engine
    TempWorkaround();
    
    // Group related operations
    {
        // Initialize subsystem
        Initialize();
        Configure();
        Start();
    }
}
```

### Security Guidelines

- **Never commit API keys or secrets**
- **Validate all user input**
- **Use permission checks before destructive operations**
- **Log security-relevant events to audit log**
- **Sanitize data before logging** (no passwords, API keys)

Example:
```cpp
// BAD: Logging sensitive data
UE_LOG(LogChatGPTEditor, Log, TEXT("API Key: %s"), *APIKey);

// GOOD: Logging without sensitive data
UE_LOG(LogChatGPTEditor, Log, TEXT("API Key validated successfully"));
```

## Testing Guidelines

### Running Tests

```bash
# Via Unreal Editor
# 1. Open Session Frontend
# 2. Go to Automation tab
# 3. Filter for "ChatGPTEditor"
# 4. Run tests

# Via Command Line
UnrealEditor-Cmd.exe YourProject.uproject -ExecCmds="Automation RunTests ChatGPTEditor" -unattended -nopause -nosplash -nullrhi
```

### Writing Tests

```cpp
// Unit test example
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FMyFeatureTest, 
    "ChatGPTEditor.MyFeature.BasicTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FMyFeatureTest::RunTest(const FString& Parameters)
{
    // Arrange
    FMyClass TestObject;
    
    // Act
    bool bResult = TestObject.DoSomething();
    
    // Assert
    TestTrue(TEXT("DoSomething should succeed"), bResult);
    
    return true;
}
```

### Test Coverage

Aim for:
- Unit tests for all public API functions
- Integration tests for feature workflows
- Security tests for permission systems
- Regression tests for fixed bugs

## Pull Request Process

### Before Submitting

- [ ] Code compiles without errors or warnings
- [ ] All existing tests pass
- [ ] New tests added for new functionality
- [ ] Code follows our coding standards
- [ ] Documentation updated (if applicable)
- [ ] Audit log working for new features
- [ ] Security permissions implemented correctly
- [ ] No sensitive data in commits

### PR Description Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Code refactoring
- [ ] Performance improvement

## Testing
Describe how you tested your changes

## Checklist
- [ ] Code compiles
- [ ] Tests pass
- [ ] Documentation updated
- [ ] No security issues
- [ ] Follows coding standards

## Related Issues
Fixes #(issue number)
```

### Review Process

1. Maintainers will review your PR
2. Address any requested changes
3. Once approved, a maintainer will merge

## Reporting Bugs

### Before Reporting

- Check existing issues to avoid duplicates
- Test with the latest version
- Gather relevant information

### Bug Report Template

```markdown
**Describe the Bug**
Clear description of what the bug is

**To Reproduce**
Steps to reproduce:
1. Go to '...'
2. Click on '...'
3. See error

**Expected Behavior**
What you expected to happen

**Screenshots**
If applicable, add screenshots

**Environment**
- Unreal Engine Version: [e.g., 5.5]
- Plugin Version: [e.g., 1.2.0]
- OS: [e.g., Windows 11]

**Additional Context**
Any other relevant information
```

## Suggesting Features

### Feature Request Template

```markdown
**Feature Description**
Clear description of the proposed feature

**Use Case**
Explain why this feature would be useful

**Proposed Solution**
How you think it should work

**Alternatives Considered**
Other approaches you've thought about

**Additional Context**
Screenshots, mockups, examples, etc.
```

## Community

### Getting Help

- **Documentation**: Check README.md and other docs
- **Issues**: Search or create GitHub issues
- **Discussions**: Use GitHub Discussions for questions

### Recognition

Contributors will be recognized in:
- GitHub contributors list
- CHANGELOG.md (for significant contributions)
- Release notes

## License

By contributing to ChatGPTEditor, you agree that your contributions will be licensed under the same license as the project.

---

**Thank you for contributing to ChatGPTEditor!** 🎉

Your efforts help make this plugin better for everyone in the Unreal Engine community.
