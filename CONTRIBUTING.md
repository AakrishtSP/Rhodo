# Contributing to Rhodo Game Engine

Thank you for your interest in contributing to Rhodo! This document provides guidelines and information for contributors to help make the development process smooth and efficient.

---

## 📋 Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
  - [Reporting Bugs](#reporting-bugs)
  - [Suggesting Features](#suggesting-features)
  - [Contributing Code](#contributing-code)
- [Development Setup](#development-setup)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Commit Guidelines](#commit-guidelines)
- [Pull Request Process](#pull-request-process)
- [Project Phases and Issues](#project-phases-and-issues)
- [Testing](#testing)
- [Documentation](#documentation)

---

## 🤝 Code of Conduct

This project follows a simple code of conduct:

- **Be respectful** and considerate of others
- **Be collaborative** and open to feedback
- **Be professional** in all communications
- **Focus on what is best** for the project and community

Unacceptable behavior will not be tolerated and may result in being banned from the project.

---

## 💡 How Can I Contribute?

### Reporting Bugs

Before submitting a bug report:

1. **Check existing issues** to avoid duplicates
2. **Verify** you're using the latest version
3. **Gather information** about your environment

When creating a bug report, include:

- **Clear title** describing the issue
- **Steps to reproduce** the problem
- **Expected behavior** vs actual behavior
- **Environment details**:
  - OS and version
  - Compiler and version
  - CMake version
  - Build configuration (Debug/Release)
- **Code snippets** or screenshots if applicable
- **Error messages** or log outputs

**Example:**
```markdown
**Bug**: Application crashes on startup in Release mode

**Environment**:
- OS: Ubuntu 22.04
- Compiler: GCC 11.3
- CMake: 3.31
- Build: Release

**Steps to Reproduce**:
1. Build in Release mode: `cmake --build . --config Release`
2. Run ExampleApp
3. Application crashes with segmentation fault

**Expected**: Application should run normally
**Actual**: Segmentation fault at startup

**Error Log**:
```
[Error] Segmentation fault at 0x...
```
```

### Suggesting Features

Feature suggestions are welcome! Before creating a feature request:

1. **Check the [roadmap](README.md#roadmap)** - your feature may already be planned
2. **Search existing issues** for similar suggestions
3. **Consider the project scope** - does it fit Rhodo's goals?

When suggesting a feature:

- **Use a clear, descriptive title**
- **Provide detailed description** of the feature
- **Explain the use case** - why is this needed?
- **Suggest implementation approach** if you have ideas
- **Consider alternatives** you've explored

### Contributing Code

All code contributions are made through pull requests. See [Pull Request Process](#pull-request-process) below.

**Good first contributions:**
- Fixing typos or improving documentation
- Adding unit tests for existing code
- Implementing features marked with "good first issue" label
- Fixing bugs marked with "help wanted" label

---

## 🛠️ Development Setup

### Prerequisites

- CMake 3.31+
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Git
- (Optional) IDE with CMake support (CLion, VS Code, Visual Studio)

### Initial Setup

1. **Fork the repository** on GitHub

2. **Clone your fork**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Rhodo.git
   cd Rhodo
   ```

3. **Add upstream remote**:
   ```bash
   git remote add upstream https://github.com/AakrishtSP/Rhodo.git
   ```

4. **Create build directory**:
   ```bash
   mkdir build
   cd build
   ```

5. **Configure for development** (Debug mode with tests):
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Debug -DRH_BUILD_TESTS=ON
   ```

6. **Build**:
   ```bash
   cmake --build .
   ```

7. **Verify tests pass**:
   ```bash
   ctest
   ```

---

## 🔄 Development Workflow

1. **Sync with upstream**:
   ```bash
   git checkout main
   git fetch upstream
   git merge upstream/main
   ```

2. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/issue-number-description
   ```

3. **Make your changes**:
   - Write code following [coding standards](#coding-standards)
   - Add or update tests as needed
   - Update documentation if necessary

4. **Build and test locally**:
   ```bash
   cmake --build build
   ctest --test-dir build
   ```

5. **Commit your changes**:
   ```bash
   git add .
   git commit -m "feat: add new feature"
   ```
   Follow [commit guidelines](#commit-guidelines)

6. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

7. **Create a Pull Request** on GitHub

---

## 📝 Coding Standards

### General Principles

- **Write clear, self-documenting code**
- **Prefer readability over cleverness**
- **Follow RAII principles**
- **Use modern C++23 features** appropriately
- **Minimize dependencies** between modules

### Code Style

#### Naming Conventions

```cpp
// Classes, Structs, Enums: PascalCase
class GameObject {};
struct Transform {};
enum class RenderMode {};

// Functions, Methods: camelCase
void updateTransform() {}
bool isActive() const {}

// Variables: camelCase
int entityCount = 0;
float deltaTime = 0.0f;

// Constants: UPPER_SNAKE_CASE
constexpr int MAX_ENTITIES = 10000;
const float PI = 3.14159f;

// Private members: camelCase with m_ prefix
class Player {
private:
    int m_health;
    float m_speed;
};

// Namespaces: PascalCase
namespace rhodo::Core {}
```

#### Formatting

- **Indentation**: 4 spaces (no tabs)
- **Braces**: Allman style (braces on new line)
- **Line length**: Aim for 100-120 characters max
- **Spaces**: Around operators, after commas

```cpp
// Good
if (condition)
{
    doSomething();
}
else
{
    doSomethingElse();
}

// Function declarations
void myFunction(int param1, float param2)
{
    // Implementation
}
```

#### Header Guards

Use `#pragma once` for header guards:

```cpp
#pragma once

namespace rhodo
{
    // Header content
}
```

#### Include Order

1. Corresponding header (for .cpp files)
2. Blank line
3. Rhodo headers
4. Blank line
5. Third-party library headers
6. Blank line
7. Standard library headers

```cpp
#include "MyClass.h"

#include "Rhodo/Core/Logger.h"
#include "Rhodo/Signals/Signal.h"

#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>
```

#### Comments

- Use `//` for single-line comments
- Use `/* */` for multi-line comments
- Document public APIs with clear comments
- Avoid obvious comments
- Explain "why", not "what"

```cpp
// Good: Explains reasoning
// Use logarithmic scaling to improve precision for small values
float scaledValue = std::log(value + 1.0f);

// Bad: States the obvious
// Increment counter by 1
counter++;
```

#### Modern C++ Features

**Use:**
- `auto` for complex types and iterators
- Range-based for loops
- Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- `constexpr` for compile-time constants
- `nullptr` instead of `NULL` or `0`
- Uniform initialization `{}`
- `enum class` instead of plain enums

```cpp
// Good
auto entities = getEntities();
for (const auto& entity : entities)
{
    // Process entity
}

std::unique_ptr<GameObject> obj = std::make_unique<GameObject>();
constexpr float GRAVITY = 9.81f;
```

### Platform-Specific Code

Use the provided platform detection macros:

```cpp
#ifdef RH_PLATFORM_WINDOWS
    // Windows-specific code
#elif defined(RH_PLATFORM_LINUX)
    // Linux-specific code
#elif defined(RH_PLATFORM_MAC)
    // macOS-specific code
#endif
```

### Error Handling

- Use assertions for debugging (`RH_CORE_ASSERT`, `RH_ASSERT`)
- Use exceptions for recoverable errors
- Log errors appropriately
- Provide meaningful error messages

```cpp
RH_CORE_ASSERT(ptr != nullptr, "Pointer cannot be null");

if (!loadResource(path))
{
    RH_CORE_ERROR("Failed to load resource: {}", path);
    return false;
}
```

---

## 📝 Commit Guidelines

We follow [Conventional Commits](https://www.conventionalcommits.org/) specification:

### Commit Message Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Types

- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, no logic change)
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Adding or updating tests
- `build`: Build system or dependencies changes
- `ci`: CI/CD configuration changes
- `chore`: Other changes (maintenance, etc.)

### Examples

```bash
# Simple feature
git commit -m "feat: add signal connection scoping"

# Bug fix with scope
git commit -m "fix(logger): resolve crash on null message"

# With body
git commit -m "feat(ecs): implement component system

Added basic component registration and management.
Includes type-safe component access and iteration."

# Breaking change
git commit -m "feat(core)!: change API signature

BREAKING CHANGE: Logger::init() now requires configuration object"
```

### Rules

- Use imperative mood ("add feature" not "added feature")
- Don't capitalize first letter of subject
- No period at the end of subject
- Keep subject line under 50 characters
- Wrap body at 72 characters
- Reference issues in footer: `Closes #123` or `Fixes #456`

---

## 🔀 Pull Request Process

### Before Creating a PR

1. **Ensure your code compiles** without warnings
2. **Run all tests** and ensure they pass
3. **Update documentation** if needed
4. **Sync with upstream** main branch
5. **Rebase if necessary** to maintain clean history

### Creating the PR

1. **Title**: Follow commit message format
   - Example: `feat: add entity component system`

2. **Description**: Include:
   - **What** changes were made
   - **Why** the changes were necessary
   - **How** the changes were implemented
   - **Testing** performed
   - **Related issues**: `Closes #123`, `Relates to #456`

3. **Template**:
   ```markdown
   ## Description
   Brief description of changes
   
   ## Type of Change
   - [ ] Bug fix
   - [ ] New feature
   - [ ] Breaking change
   - [ ] Documentation update
   
   ## Changes Made
   - List key changes
   - Another change
   
   ## Testing
   - Describe testing performed
   - List test cases added/updated
   
   ## Checklist
   - [ ] Code follows project style guidelines
   - [ ] Self-reviewed the code
   - [ ] Commented complex areas
   - [ ] Updated documentation
   - [ ] Added/updated tests
   - [ ] All tests pass locally
   - [ ] No new warnings introduced
   
   ## Related Issues
   Closes #123
   ```

### Review Process

1. **Automated checks** must pass (when CI/CD is set up)
2. **Code review** by maintainer(s)
3. **Address feedback** by pushing additional commits
4. **Approval** required before merging
5. **Squash or rebase** may be requested for clean history

### After Merge

- Delete your feature branch
- Update your local repository
- Celebrate! 🎉

---

## 🗺️ Project Phases and Issues

Rhodo development follows a phased approach. Each phase has associated GitHub issues:

### Phase Organization

- **Phase 0**: Core Infrastructure (Issues #1-8, #45-46)
- **Phase 1**: Engine Core & ECS (Issues #9-13)
- **Phase 2**: Rendering (Issues #14-21)
- **Phase 3**: Math & Physics (Issues #22-26)
- **Phase 4**: Input, Audio, UI (Issues #27-30)
- **Phase 5**: Scene, Scripting, Editor (Issues #31-35)
- **Phase 6**: Web Builds (Issues #36-39)
- **Phase 7**: Optimization (Issues #40-44)

### Finding Issues to Work On

1. Check [GitHub Issues](https://github.com/AakrishtSP/Rhodo/issues)
2. Look for labels:
   - `good first issue` - Good for newcomers
   - `help wanted` - Extra attention needed
   - `Phase 0`, `Phase 1`, etc. - Phase-specific work
3. Comment on an issue to express interest before starting work
4. Coordinate with maintainers to avoid duplicate effort

### Working on Current Phase

Focus contributions on the current phase in progress (Phase 0 as of now). Features from future phases may not be accepted until earlier phases are complete.

---

## 🧪 Testing

### Writing Tests

- **Unit tests** for all new functionality
- **Integration tests** for system interactions
- **Use Google Test** framework (when integrated)
- **Test edge cases** and error conditions

### Test Organization

```
Tests/
├── Core/
│   ├── LoggerTests.cpp
│   └── AssertTests.cpp
├── Signals/
│   └── SignalTests.cpp
└── CMakeLists.txt
```

### Running Tests

```bash
# Run all tests
ctest --test-dir build

# Run specific test
./build/build/bin/<Platform>/RhodoTests/RhodoTests --gtest_filter=SignalTests.*

# Run with verbose output
ctest --test-dir build --verbose
```

### Test Guidelines

- **Test names**: Descriptive and clear (`TestSignalEmission`, `TestConnectionLifetime`)
- **One concept per test**: Keep tests focused
- **Arrange-Act-Assert**: Follow AAA pattern
- **Independent tests**: No dependencies between tests
- **Deterministic**: Same input = same output

---

## 📚 Documentation

### Code Documentation

- **Public APIs**: Document all public classes, methods, and functions
- **Complex logic**: Add comments explaining non-obvious code
- **Examples**: Provide usage examples for new features

### Documentation Updates

When adding features, update:

- **README.md**: Feature lists, usage examples
- **CONTRIBUTING.md**: Development guidelines if needed
- **Header comments**: API documentation
- **Example code**: Demonstrate new functionality

### Documentation Style

```cpp
/**
 * @brief Brief description of the class
 * 
 * Detailed description of what the class does and how to use it.
 * Can span multiple lines.
 * 
 * @example
 * Signal<int> mySignal;
 * mySignal.connect([](int value) { 
 *     std::cout << value << std::endl; 
 * });
 * mySignal.emit(42);
 */
class Signal
{
public:
    /**
     * @brief Connect a callback to this signal
     * @param callback Function to call when signal is emitted
     * @return Connection object for managing the connection
     */
    Connection connect(Callback callback);
};
```

---

## ❓ Questions?

If you have questions not covered in this guide:

1. **Check existing documentation** in the repository
2. **Search closed issues** for similar questions
3. **Open a discussion** on GitHub Discussions (when available)
4. **Ask in an issue** with the `question` label

---

## 🙏 Thank You!

Your contributions help make Rhodo better for everyone. Whether you're fixing a typo or implementing a major feature, every contribution is valued and appreciated!

---

<div align="center">

**Happy Coding!** 🚀

</div>
