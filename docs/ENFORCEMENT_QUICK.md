# Rhodo Enforcement Quick Reference

## 🚀 Quick Commands

```bash
# Configure with enforcement
cmake -S . -B build -G Ninja -DRH_ENFORCE_NAMING=ON

# Build (with clang-tidy)
ninja -C build

# Format all code
ninja -C build format

# Check formatting (CI-safe)
ninja -C build format-check

# Run clang-tidy
ninja -C build tidy

# Auto-fix clang-tidy issues
ninja -C build tidy-fix

# Nuclear button (fix everything)
ninja -C build fix
```

---

## 📁 Naming Rules (Quick)

| Location       | Files                   | Example                           |
|----------------|-------------------------|-----------------------------------|
| `engine/`      | **CamelCase**           | `Logger.cpp`, `EntityManager.hpp` |
| `Application/` | CamelCase or snake_case | `Editor.cpp`, `main_window.cpp`   |
| `Tests/`       | Flexible                | `LoggerTests.cpp`, `test_ecs.cpp` |
| `cmake/`       | **CamelCase.cmake**     | `ClangTidy.cmake`                 |

---

## 💻 C++ Symbol Naming

```cpp
// ✅ CORRECT
class EntityManager {              // CamelCase
public:
    void AddComponent();            // CamelCase
    int GetCount() const;           // CamelCase

    int entity_count;               // lower_case
    static constexpr int kMaxSize = 100;  // kCamelCase

private:
    int buffer_size_;               // lower_case_
};

// ❌ WRONG
class entity_manager {              // Wrong case
public:
    void add_component();           // Wrong case
    int m_count;                    // Wrong prefix
    int EntityCount;                // Wrong case
};
```

---

## 🔧 Enforcement Flags

```bash
# Enable/disable naming enforcement
-DRH_ENFORCE_NAMING=ON|OFF         # Default: ON

# Enable/disable clang-tidy
-DRH_ENABLE_CLANG_TIDY=ON|OFF      # Default: ON

# Enable/disable clang-format
-DRH_ENABLE_CLANG_FORMAT=ON|OFF    # Default: ON

# Strict module name matching
-DRH_ENFORCE_MODULE_NAME_MATCH=ON  # Default: OFF
```

---

## 🐛 Common Fixes

### CMake Configuration Fails

```bash
# Error: Invalid engine file name
mv Rhodo/logger.cpp Rhodo/Logger.cpp

# Reconfigure
cmake -S . -B build
```

### clang-tidy Build Fails

```bash
# Fix automatically
ninja tidy-fix

# Or fix manually based on errors
```

### Formatting Fails

```bash
# Auto-format
ninja format

# Stage changes
git add -u
```

### Pre-Commit Hook Blocks

```bash
# Fix issues first
ninja fix

# Then commit
git commit -m "Message"

# Emergency bypass (NOT RECOMMENDED - CI will still fail)
git commit --no-verify -m "Message"
```

---

## 📊 Enforcement Layers

1. **CMake** → Validates file/directory naming
2. **clang-tidy** → Validates C++ symbol naming
3. **Pre-commit** → Validates formatting + layout
4. **CI** → Validates everything (authoritative)

---

## 🎯 Target Matrix

| Target               | Engine | Tools | Tests |
|----------------------|--------|-------|-------|
| `format-Rhodo`       | ✅      | ❌     | ❌     |
| `format-Application` | ❌      | ✅     | ❌     |
| `format`             | ✅      | ✅     | ✅     |
| `tidy-Rhodo`         | ✅      | ❌     | ❌     |
| `tidy-Application`   | ❌      | ✅     | ❌     |
| `tidy`               | ✅      | ✅     | ✅     |
| `fix`                | ✅      | ✅     | ✅     |

---

## 🔍 Pre-Commit Checks

The hook validates:

- ✅ Layout & naming
- ✅ Code formatting
- ✅ CMake configuration
- ⚠️ Debug prints (warning)
- ⚠️ TODOs without issue refs (warning)
- ❌ Trailing whitespace (error)

---

## 📝 Quick Test

```bash
# Create test file
echo "class bad_class {};" > Rhodo/TestFile.cpp

# Try to configure - should fail
cmake -S . -B build

# Fix naming
mv Rhodo/TestFile.cpp Rhodo/test_file.cpp  # Still wrong (snake_case)
mv Rhodo/test_file.cpp Rhodo/TestFile.cpp  # Correct (CamelCase)

# Fix class name
echo "class TestClass {};" > Rhodo/TestFile.cpp

# Should pass now
cmake -S . -B build
```

---

## 🆘 Emergency Overrides

```bash
# Disable ALL enforcement (dev mode only)
cmake -S . -B build \
  -DRH_ENFORCE_NAMING=OFF \
  -DRH_ENABLE_CLANG_TIDY=OFF \
  -DRH_ENABLE_CLANG_FORMAT=OFF

# Bypass pre-commit
git commit --no-verify

# ⚠️ WARNING: CI will still enforce everything
```

---

## 📖 Full Documentation

- [Complete Enforcement Guide](ENFORCEMENT.md)
- [Code Quality Architecture](CODE_QUALITY.md)
- [Contributing Guidelines](../CONTRIBUTING.md)

---

**Last Updated:** December 9, 2025
