# Rhodo Code Quality Enforcement

This document describes the **4-layer enforcement system** that ensures code quality and consistency across the Rhodo
Engine codebase.

---

## 🛡️ Enforcement Layers

```
┌─────────────────────────────────────────────────────────────┐
│  Layer 1: CMake Configure (Hard Fail)                       │
│  ├─ Validates: File/directory naming                        │
│  ├─ Validates: Module name = filename                       │
│  └─ Blocks: Configuration if violations found               │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│  Layer 2: clang-tidy (Build-time)                           │
│  ├─ Validates: C++ symbol naming (classes, variables, etc)  │
│  ├─ Validates: Code patterns & best practices               │
│  └─ Blocks: Build if critical warnings found                │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│  Layer 3: Git Pre-Commit Hook (Local Gatekeeper)            │
│  ├─ Validates: Formatting (clang-format)                    │
│  ├─ Validates: CMake configuration                          │
│  ├─ Warns: Debug prints, TODOs without issues               │
│  └─ Blocks: Commit if violations found                      │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│  Layer 4: CI/CD (Authoritative Enforcer)                    │
│  ├─ Validates: All of the above                             │
│  ├─ Runs: Full test suite                                   │
│  ├─ Runs: Complete clang-tidy analysis                      │
│  └─ Blocks: Merge if any check fails                        │
└─────────────────────────────────────────────────────────────┘
```

---

## 📋 Naming Conventions

### Engine Core (`engine/`)

**Files:** `CamelCase` only

```
✅ Logger.cpp
✅ EntityManager.hpp
✅ Debug.cppm
❌ logger.cpp
❌ entity_manager.hpp
❌ debug_utils.cppm
```

**Modules:** Filename must match module declaration

```cpp
// ✅ File: Logger.cppm
export module Logger;

// ❌ File: logger.cppm  (wrong case)
export module Logger;

// ❌ File: Logger.cppm  (mismatch)
export module LoggingSystem;
```

**C++ Symbols:**

- **Classes/Structs:** `CamelCase` (e.g., `EntityManager`)
- **Functions/Methods:** `CamelCase` (e.g., `GetComponent()`)
- **Variables:** `lower_case` (e.g., `entity_count`)
- **Private Members:** `lower_case_` (e.g., `buffer_size_`)
- **Constants:** `kCamelCase` (e.g., `kMaxEntities`)
- **Macros:** `UPPER_CASE` (e.g., `RH_ASSERT`)

### Application/Tools (`Application/`)

**Files:** `CamelCase` or `snake_case` allowed

```
✅ Application.cpp
✅ editor_window.cpp
✅ AssetImporter.hpp
```

**Symbols:** Same as engine (enforced by clang-tidy)

### Tests (`Tests/`)

**Files:** Flexible naming

```
✅ LoggerTests.cpp
✅ test_entity_manager.cpp
✅ EntityManagerTest.cpp
```

### CMake (`cmake/`)

**Files:** `CamelCase.cmake`

```
✅ ClangTidy.cmake
✅ EnforceLayout.cmake
❌ clang_tidy.cmake
```

---

## 🚀 Usage

### Testing Enforcement Locally

```bash
# 1. CMake layer - validates at configure time
cmake -S . -B build
# ❌ Fails if file/directory naming is wrong

# 2. Build layer - validates code during compilation
cmake --build build
# ❌ Fails if C++ symbol naming is wrong

# 3. Format check - validates formatting
ninja format-check
# ❌ Fails if formatting is wrong

# 4. Fix everything automatically
ninja fix
# ✅ Auto-fixes formatting + tidy issues
```

### Git Workflow

```bash
# Stage your changes
git add MyChanges.cpp

# Pre-commit hook runs automatically
git commit -m "Add feature"
# ❌ Blocked if any violations found

# If blocked, fix issues:
ninja format          # Fix formatting
ninja tidy-fix        # Fix tidy issues
# Then retry commit
```

### Disabling Enforcement (Not Recommended)

```bash
# Disable CMake layout checks
cmake -S . -B build -DRH_ENFORCE_NAMING=OFF

# Disable clang-tidy
cmake -S . -B build -DRH_ENABLE_CLANG_TIDY=OFF

# Bypass pre-commit hook (dangerous!)
git commit --no-verify -m "Message"
```

⚠️ **Warning:** CI will still enforce all rules regardless of local settings.

---

## 🔧 Configuration Files

| File                                 | Purpose                             |
|--------------------------------------|-------------------------------------|
| `cmake/EnforceLayout.cmake`          | File/directory naming validation    |
| `.clang-tidy` (root)                 | Baseline C++ naming & checks        |
| `engine/.clang-tidy`                 | Engine-specific rules (performance) |
| `Application/.clang-tidy`            | Tools-specific rules (safety)       |
| `.clang-format` (root)               | Formatting baseline                 |
| `engine/.clang-format`               | Engine formatting (100 cols)        |
| `Application/.clang-format`          | Tools formatting (80 cols)          |
| `.git/hooks/pre-commit`              | Local commit validation             |
| `.github/workflows/code-quality.yml` | CI enforcement                      |

---

## 🐛 Common Issues

### "CMake configuration failed - check naming conventions"

**Cause:** File or directory doesn't match naming convention.

**Fix:**

```bash
# Find the violating file
cmake -S . -B build 2>&1 | grep "Invalid"

# Rename it
mv engine/bad_file.cpp engine/BadFile.cpp
```

### "clang-tidy warnings as errors"

**Cause:** C++ symbol naming violation.

**Fix:**

```cpp
// ❌ Before
class my_class {
    int BadVariable;
};

// ✅ After
class MyClass {
    int bad_variable_;
};
```

### "Formatting violations detected"

**Fix:**

```bash
ninja format
git add -u
```

### "Module name mismatch"

**Cause:** Module filename doesn't match declared module name.

**Fix:**

```cpp
// File: MyModule.cppm
export module MyModule;  // ✅ Must match filename
```

---

## 📊 CI Status Checks

Every pull request must pass:

1. ✅ **enforce-layout** - Naming & layout validation
2. ✅ **run-tests** - Unit test suite
3. ✅ **clang-tidy-full** - Complete static analysis

View status: Pull Request → Checks tab

---

## 🎓 Best Practices

### Adding New Files

1. Follow naming convention from the start
2. Run `cmake -S . -B build` to validate
3. Run `ninja format` before committing

### Refactoring

1. Rename files to match conventions first
2. Let clang-tidy guide symbol renames
3. Use `ninja tidy-fix` for automatic fixes

### Working with Modules

```cpp
// File: Logger.cppm
export module Logger;  // Must match filename

// File: Logger.Debug.cppm
export module Logger.Debug;  // Partitions use dots
```

### Suppressing Checks (Rare)

Only when justified:

```cpp
// NOLINT(readability-identifier-naming)
void legacy_api_function() {  // External constraint
    // ...
}
```

---

## 🔥 Why This Matters

| Without Enforcement                      | With Enforcement       |
|------------------------------------------|------------------------|
| `logger.cpp`, `Logger.cpp`, `LOGGER.cpp` | `Logger.cpp` only      |
| `int myVar`, `my_var`, `MyVar` mixed     | Consistent `my_var`    |
| 10 different formatting styles           | One canonical style    |
| Merge conflicts from whitespace          | Clean diffs            |
| "Works on my machine"                    | Guaranteed consistency |

---

## 📚 References

- [CMake Enforcement](cmake/EnforceLayout.cmake)
- [Naming Conventions](docs/CODE_QUALITY.md)
- [clang-tidy Checks](https://clang.llvm.org/extra/clang-tidy/checks/list.html)
- [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)

---