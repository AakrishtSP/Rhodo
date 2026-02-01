# Contributing to Rhodo Game Engine

Thank you for contributing to Rhodo! This guide explains our development process, code standards, and submission
workflow.

---

## 📋 Quick Links

- [Code of Conduct](#code-of-conduct)
- [Development Setup](#development-setup)
- [Code Standards](#code-standards)
- [Development Workflow](#development-workflow)
- [Submission Process](#submission-process)
- [Common Tasks](#common-tasks)

---

## 🤝 Code of Conduct

- **Be respectful** of all contributors
- **Be collaborative** and open to feedback
- **Be professional** in communications
- **Focus on code quality** and project goals

---

## 🚀 Development Setup

### Prerequisites

```bash
# Linux (Ubuntu/Debian)
sudo apt-get install cmake ninja-build clang-17 clang-format-17 clang-tidy-17

# macOS
brew install cmake ninja llvm@17

# Windows
# Use LLVM/Clang installer or Visual Studio with C++ tools
```

### Initial Setup

```bash
# Clone and configure
git clone https://github.com/AakrishtSP/Rhodo.git
cd Rhodo

# Configure with code quality enabled
cmake -S . -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DRH_ENABLE_CLANG_TIDY=ON \
  -DRH_ENABLE_CLANG_FORMAT=ON

# Build
cmake --build build

# Verify setup
ctest --test-dir build --output-on-failure
```

### Enable Pre-Commit Hook

The pre-commit hook automatically validates your changes:

```bash
# It's already installed at .git/hooks/pre-commit
# Just make sure it's executable (done automatically)
git config core.hooksPath .git/hooks
```

---

## 💻 Code Standards

### Naming Conventions

**Engine Core** (`engine/`) - Performance-first LLVM style:

```cpp
// ✅ Classes & Structs: CamelCase
class EntityManager { };

// ✅ Functions/Methods: CamelCase
void ProcessEntities();

// ✅ Variables: lower_case
int entity_count;

// ✅ Private members: lower_case_
int buffer_size_;

// ✅ Constants: kCamelCase
static constexpr int kMaxEntities = 1000;

// ✅ Macros: UPPER_CASE
#define RH_ASSERT(x) assert(x)
```

**Tools/Editor** (`application/`) - Safety-first Google style:

- Same naming as above
- Stricter function size limits
- More aggressive const correctness

### Module Naming

All modules use **PascalCase hierarchical naming**:

```cpp
// ✅ root module
export module Rhodo;

// ✅ Subsystems
export module Rhodo.Core.Logger;
export module Rhodo.Core.Signals;

// ✅ Partitions (implementation)
export module Rhodo.Core.Logger:Impl;
export module Rhodo.Core.Signals:ScopedConnection;
```

Files must match module names:

- `Rhodo.Core.Logger` → `Logger.cppm`
- `Rhodo.Core.Logger:Impl` → `LoggerImpl.cppm`
- `Rhodo.Core.Signals:ScopedConnection` → `ScopedConnection.cppm`

See [Module Naming Guide](docs/MODULE_NAMING.md) for details.

### Code Patterns

**Engine code - Performance-first:**

```cpp
// ✅ Prefer manual memory for hot paths
void* Allocate(size_t size) {
  // Custom allocator logic
}

// ✅ Use raw pointers where appropriate
int* buffer = arena.Allocate<int>(size);

// ✅ Intrusive data structures for cache efficiency
struct Node {
  T data;
  Node* next;  // Part of the structure
};
```

**Tools code - Safety-first:**

```cpp
// ✅ Use smart pointers
auto buffer = std::make_unique<int[]>(size);

// ✅ Defensive programming
if (ptr && ptr->IsValid()) {
  ptr->Process();
}

// ✅ Use standard containers
std::vector<Entity> entities;
```

---

## 🔄 Development Workflow

### 1. Create a Feature Branch

```bash
git checkout -b feature/my-feature
# or
git checkout -b fix/bug-description
```

### 2. Make Changes

```bash
# Edit files
# Your code is automatically checked by CMake at build time

# Build and validate
cmake --build build
```

### 3. Auto-Fix Issues

```bash
# Format code
ninja -C build format

# Fix clang-tidy issues
ninja -C build tidy-fix

# Nuclear button (format + tidy)
ninja -C build fix

# Stage fixed code
git add -A
```

### 4. Commit

```bash
# Pre-commit hook validates automatically
git commit -m "Add feature: description"

# If hook blocks, it shows what to fix
# Common issue: ninja fix and retry
```

### 5. Push & Create PR

```bash
git push origin feature/my-feature
# Then create PR on GitHub
```

---

## ✅ Submission Process

### Before Creating a PR

- [ ] Code builds: `cmake --build build`
- [ ] Tests pass: `ctest --test-dir build`
- [ ] Format is correct: `ninja -C build format-check`
- [ ] Tidy passes: `ninja -C build tidy`
- [ ] Pre-commit passes: `git commit` (hook runs)

### PR Checklist

- [ ] Branch name describes the change
- [ ] Commit messages are clear
- [ ] Code follows naming conventions
- [ ] Module names are PascalCase hierarchical
- [ ] Tests added for new functionality
- [ ] Documentation updated
- [ ] CI passes (GitHub Actions)

### CI Pipeline

All PRs must pass:

1. **enforce-layout** - Naming validation
2. **run-tests** - Unit test suite
3. **clang-tidy-full** - Complete analysis

View status on your PR's Checks tab.

---

## 🛠️ Common Tasks

### Adding a New Module

1. Create directory: `Rhodo/NewSystem/`
2. Create main module: `NewSystem.cppm`
   ```cpp
   export module Rhodo.NewSystem;
   export namespace rhodo::newsystem { ... }
   ```
3. Create `CMakeLists.txt` with module registration
4. Update parent `CMakeLists.txt`

### Adding Implementation Partitions

```cpp
// File: MyImpl.cppm
export module Rhodo.MyModule:Impl;
module Rhodo.MyModule:Impl;

// Internal implementation (not exported to users)
namespace rhodo::mymodule {
  void InternalHelper() { ... }
}
```

### Disabling Checks Temporarily

```bash
# During development (NOT FOR PRODUCTION)
cmake -S . -B build \
  -DRH_ENFORCE_NAMING=OFF \
  -DRH_ENABLE_CLANG_TIDY=OFF

# Or bypass pre-commit (dangerous!)
git commit --no-verify
```

### Debugging Tidy Issues

```bash
# See what tidy is complaining about
ninja -C build tidy

# Fix automatically
ninja -C build tidy-fix

# Or suppress with NOLINT (use sparingly)
int legacy_variable;  // NOLINT(readability-identifier-naming)
```

---

## 📚 Additional Resources

- [Code Quality Architecture](docs/CODE_QUALITY.md)
- [Enforcement System](docs/ENFORCEMENT.md)
- [Module Naming Guide](docs/MODULE_NAMING.md)
- [CMake Documentation](https://cmake.org/cmake/help/latest/)
- [clang-tidy Checks](https://clang.llvm.org/extra/clang-tidy/checks/list.html)

---

## ❓ Questions?

- **Build issues?** Check the [README](README.md#troubleshooting) section
- **Code style questions?** See [Code Quality](docs/CODE_QUALITY.md)
- **Need help?** Open a [Discussion](https://github.com/AakrishtSP/Rhodo/discussions)

---

## 🎓 Learning Rhodo

- Module system: Understand C++20 modules
- ECS concepts: Entity Component System patterns
- Performance: Data-oriented design principles
- Game engines: Study existing engines for patterns

---

**Thank you for contributing to Rhodo! 🚀**

Last Updated: December 9, 2025
