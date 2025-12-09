# Rhodo Code Quality Architecture

## 📁 Hierarchical Style System

Rhodo uses **per-directory code quality profiles** to balance performance and safety:

```
/
├── .clang-tidy          # Baseline (all checks enabled)
├── .clang-format        # Baseline (Google style, 80 cols)
│
├── engine/              # ENGINE CORE
│   ├── .clang-tidy      # Performance-first LLVM style
│   └── .clang-format    # Relaxed (100 cols, SIMD-friendly)
│
├── application/         # TOOLS & EDITOR
│   ├── .clang-tidy      # Safety-first Google style
│   └── .clang-format    # Strict (80 cols, defensive)
│
└── tests/               # UNIT TESTS
    └── .clang-tidy      # Balanced (no magic number checks)
```

---

## 🎯 Philosophy

### 🔥 **Engine Core (`engine/`)** — Performance-First

**Goal:** Zero-cost abstractions, cache-friendly, deterministic, data-oriented.

**Enabled Checks:**

- ✅ `bugprone-*` — Catch real bugs
- ✅ `performance-*` — Optimize hot paths
- ✅ `llvm-*` — Modern C++ patterns
- ✅ `modernize-*` (selective) — Smart pointers only when beneficial
- ✅ `portability-*` — Cross-platform safety

**Disabled Checks:**

- ❌ `cppcoreguidelines-pro-bounds-pointer-arithmetic` — Needed for allocators
- ❌ `cppcoreguidelines-pro-type-reinterpret-cast` — Needed for SIMD/serialization
- ❌ `modernize-use-trailing-return-type` — Readability preference
- ❌ `modernize-avoid-c-arrays` — Cache-line alignment use cases
- ❌ `google-runtime-references` — Engine APIs need non-const refs

**Formatting:**

- 100-column limit (SIMD intrinsics)
- Consecutive alignment for data tables
- 3-space comment padding

**Best For:**

- ECS systems
- Memory allocators
- Render loops
- Physics engines
- Job schedulers
- Asset loading

---

### 🛡️ **Tools/Editor (`application/`)** — Safety-First

**Goal:** Readable, maintainable, safe, defensive.

**Enabled Checks:**

- ✅ `google-*` — Google C++ Style Guide
- ✅ `cppcoreguidelines-*` — C++ Core Guidelines
- ✅ `hicpp-*` — High Integrity C++
- ✅ `readability-*` — Clarity over performance
- ✅ `cert-*` — Security hardening

**Formatting:**

- 80-column limit (strict)
- Conservative short function rules
- Standard Google alignment

**Best For:**

- Asset importers
- Level editors
- Profilers
- Debug visualizers
- Build tools

---

### ⚖️ **Tests (`Tests/`)** — Balanced

**Goal:** Readable tests without noise.

**Disabled:**

- ❌ Magic number warnings (tests are full of literals)
- ❌ Function size limits (test cases can be long)

---

## 🚀 Usage

### Run All Targets

```bash
ninja tidy           # Parallel clang-tidy on all code
ninja tidy-fix       # Auto-fix all issues
ninja format         # Format all code
ninja format-check   # CI validation
ninja fix            # Nuclear button (tidy-fix + format)
```

### Per-Directory Targets

```bash
ninja tidy-Rhodo          # Engine core only
ninja tidy-Application    # Tools only
ninja format-Rhodo        # Format engine
ninja format-Application  # Format tools
```

### Manual Check (Respects Hierarchy)

```bash
# Engine file uses Rhodo/.clang-tidy automatically
clang-tidy -p build Rhodo/Core/Memory.cpp

# Tool file uses Application/.clang-tidy automatically
clang-tidy -p build Application/Editor.cpp
```

---

## 🔧 Customization

### Add Engine-Specific Suppressions

Edit `Rhodo/.clang-tidy` and uncomment/add patterns:

```yaml
Checks: >
  ...,
  -cppcoreguidelines-pro-bounds-pointer-arithmetic,  # Custom allocators
  -cppcoreguidelines-owning-memory,                  # Manual lifetime mgmt
  -google-runtime-references                         # Engine APIs
```

### Add Per-Module Overrides

Create module-specific configs:

```bash
Rhodo/Renderer/.clang-tidy    # Vulkan-specific rules
Rhodo/Physics/.clang-tidy     # SIMD/alignment rules
Application/Editor/.clang-tidy # Qt-specific rules
```

---

## 📊 Check Summary

| Category               | Engine Core | Tools/Editor | Tests       |
|------------------------|-------------|--------------|-------------|
| **Primary Style**      | LLVM        | Google       | Balanced    |
| **Performance Checks** | ✅ Strict    | ⚠️ Moderate  | ✅ Strict    |
| **Safety Checks**      | ⚠️ Relaxed  | ✅ Strict     | ⚠️ Moderate |
| **Pointer Arithmetic** | ✅ Allowed   | ❌ Forbidden  | ⚠️ Warn     |
| **Function Size**      | 150 lines   | 80 lines     | Unlimited   |
| **Column Limit**       | 100         | 80           | 80          |
| **Magic Numbers**      | ⚠️ Warn     | ❌ Error      | ✅ Allowed   |

---

## 🎓 Guidelines

### When to Suppress Warnings

**In Engine Code:**

```cpp
// NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
// Justified: Custom allocator bump pointer arithmetic
void* Allocate(size_t size) {
  void* ptr = current_;
  current_ += size;  // Required for arena allocation
  return ptr;
}
```

**In Tools Code:**

```cpp
// Avoid suppressions — refactor instead
// BAD:
auto ptr = reinterpret_cast<Foo*>(data);  // NOLINT

// GOOD:
auto ptr = std::bit_cast<Foo*>(data);
```

### Architecture-Specific Relaxations

For future platform-specific code:

```
Rhodo/Platform/
├── .clang-tidy          # Add SIMD/intrinsic suppressions
└── SIMD/
    └── .clang-tidy      # Maximum relaxation for AVX/NEON
```

---

## 🔥 Why This Works

1. **Engine developers** don't fight false positives in hot paths
2. **Tools developers** get safety nets and clear errors
3. **CI stays clean** — each domain enforces its own correctness
4. **Onboarding is easier** — new devs see appropriate warnings
5. **Performance audits are obvious** — check diffs in `Rhodo/`

---

## 📚 References

- [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [clang-tidy Checks List](https://clang.llvm.org/extra/clang-tidy/checks/list.html)

---

**Last Updated:** December 9, 2025
**Maintained By:** Rhodo Engine Team
