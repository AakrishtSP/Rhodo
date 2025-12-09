<div align="center">

# 🎮 Rhodo Game Engine

**A modern, high-performance game engine built with C++23 and C++20 Modules**

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![CMake](https://img.shields.io/badge/CMake-3.30+-green.svg)](https://cmake.org/)
[![C++](https://img.shields.io/badge/C++-23-blue.svg)](https://isocpp.org/)
[![Modules](https://img.shields.io/badge/C%2B%2B20-Modules-brightgreen.svg)](https://en.cppreference.com/w/cpp/language/modules)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](https://github.com/AakrishtSP/Rhodo)

</div>

---

## 📋 Table of Contents

- [About](#about)
- [Features](#features)
- [Code Quality](#code-quality)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
  - [Development Setup](#development-setup)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

---

## 🎯 About

Rhodo is a professional, open-source game engine designed for **performance**, **modularity**, and **cross-platform
compatibility**. Built with modern C++23 and C++20 modules, Rhodo provides developers with a production-ready foundation
for creating high-performance games.

The engine emphasizes:

- **Zero-cost abstractions** through compile-time optimization
- **Modular architecture** with C++20 module support
- **Data-oriented design** for cache efficiency
- **CI/CD-ready** with strict code quality enforcement
- **Studio-grade tooling** (clang-tidy, clang-format, CMake)

---

## ✨ Features

### Core Infrastructure

- **🔧 C++23 with Modules** - Modern C++20 module system for fast, clean compilation
- **📦 Module-Aware Build** - First-class support for C++20 modules with BMI generation
- **🔍 Type-Safe Signals** - Zero-overhead event system with compile-time type checking
- **📝 Structured Logging** - High-performance logging with Quill backend
- **🏗️ ECS-Ready** - Entity Component System support via module interfaces
- **🌍 Cross-Platform** - Windows, Linux, macOS with unified API

### Developer Experience

- **⚡ Fast Iteration** - Module-aware incremental compilation
- **🔬 Code Quality** - Automated clang-tidy + clang-format enforcement
- **📊 Per-Directory Policies** - Engine (performance) vs Tools (safety) profiles
- **🚀 CI/CD Ready** - GitHub Actions workflow with strict gates
- **📚 Comprehensive Docs** - Architecture guides, API reference, best practices

---

## 🛡️ Code Quality

Rhodo enforces **production-grade code quality** at 4 layers:

### Layer 1: CMake (Configure Time)

- ✅ File/directory naming validation
- ✅ PascalCase module naming enforcement
- ✅ Hierarchical module structure validation

### Layer 2: clang-tidy (Build Time)

- ✅ Symbol naming (classes, variables, constants)
- ✅ Code patterns & best practices
- ✅ Per-directory profiles (engine vs tools)

### Layer 3: Git Pre-Commit (Local)

- ✅ Format validation (clang-format)
- ✅ CMake configuration check
- ✅ Trailing whitespace detection

### Layer 4: CI/CD (Remote)

- ✅ Complete validation pipeline
- ✅ Unit test suite
- ✅ Full clang-tidy analysis

**Result:** Impossible to merge code violating standards.

See [Code Quality Documentation](docs/CODE_QUALITY.md) for details.

---

## 🚀 Getting Started

### Prerequisites

- **CMake** 3.30 or later
- **C++23 Compiler**:
    - Clang 17+ (recommended)
    - GCC 13+
    - MSVC 17.4+
- **Ninja** (recommended) or Make

### Quick Build

```bash
# Clone repository
git clone https://github.com/AakrishtSP/Rhodo.git
cd Rhodo

# Configure
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Build
cmake --build build

# Run tests
ctest --test-dir build
```

### Development Setup

```bash
# Full setup with code quality enabled
cmake -S . -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DRH_ENABLE_CLANG_TIDY=ON \
  -DRH_ENABLE_CLANG_FORMAT=ON \
  -DRH_ENABLE_ASSERTS=ON

# Format all code
ninja -C build format

# Run static analysis
ninja -C build tidy

# Auto-fix issues
ninja -C build fix
```

---

## 📁 Project Structure

```
rhodo/
├── engine/                  # Engine Core (C++20 Modules)
│   ├── Rhodo.cppm          # root module
│   ├── core/               # Core systems
│   ├── debug/              # Debug assertions
│   ├── logger/             # Logging system
│   └── signals/            # Event system
├── application/            # Editor & Tools (relaxed standards)
├── tests/                  # Unit tests
├── cmake/                  # CMake modules
│   ├── ClangTidy.cmake
│   ├── ClangFormat.cmake
│   └── EnforceLayout.cmake
├── docs/                   # Documentation
│   ├── CODE_QUALITY.md
│   ├── ENFORCEMENT.md
│   └── MODULE_NAMING.md
└── CMakeLists.txt
```

---

## 📖 Documentation

- **[Code Quality Architecture](docs/CODE_QUALITY.md)** - Style profiles, check configuration
- **[Enforcement System](docs/ENFORCEMENT.md)** - Naming conventions, CI validation
- **[Module Naming](docs/MODULE_NAMING.md)** - PascalCase hierarchical modules
- **[Quick Reference](docs/ENFORCEMENT_QUICK.md)** - Commands and common patterns

---

## 🤝 Contributing

We welcome contributions! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

**Quick Start:**

1. Fork the repository
2. Create a feature branch
3. Follow [coding standards](docs/CODE_QUALITY.md)
4. Submit a pull request

**Before committing:**

```bash
ninja fix           # Auto-format and fix issues
git add -A
git commit          # Pre-commit hook validates
```

### Planned Features
- **🎨 Rendering**
  - Vulkan-based rendering pipeline
  - WebGL/WebGPU support for web builds
  - Shader management and reflection
  - Advanced lighting and materials

- **🎮 Core Engine**
  - Entity Component System (ECS)
  - Scene management and serialization
  - Resource management system
  - Layer-based architecture

- **⚙️ Physics & Math**
  - Custom math library
  - Physics engine integration
  - Collision detection and response

- **🎹 Input & Audio**
  - Input abstraction layer
  - Audio system integration
  - UI component system

- **🛠️ Tools & Editor**
  - Visual scene editor
  - Scripting integration
  - Hot reload capability
  - Profiling tools

See our [Roadmap](#roadmap) for detailed development phases.

---

## 🗺️ Roadmap

Rhodo's development is organized into 7 distinct phases:

### Phase 0: Core Infrastructure ⚙️ (In Progress)
- [x] Logging system
- [x] Signal/Event system
- [ ] Assertion system & debug macros
- [ ] Profiling hooks
- [ ] Build system & CI/CD
- [ ] Configuration system
- [ ] Filesystem abstraction
- [ ] Layer/Layer Stack

### Phase 1: Engine Core & ECS 🎯
- Application loop & frame pipeline
- High-precision cross-platform timers
- ECS integration and abstraction
- Entity/component lifecycle management
- Resource management system
- Custom memory allocators

### Phase 2: Rendering 🎨
- Renderer interface/abstraction
- Vulkan initialization
- Descriptor sets, pipelines, render passes
- GPU resource management
- Shader management
- Camera & scene graph
- Basic rendering systems
- Frame pacing

### Phase 3: Math & Physics 🔬
- Math library
- Physics integration
- Collision detection & response
- Kinematic character controller
- ECS integration with physics

### Phase 4: Input, Audio, and UI 🎹
- Input abstraction
- Audio system integration
- UI components/HUD
- Debug overlay/console

### Phase 5: Scene, Scripting, & Editor 📝
- Scene serialization
- Hot reload/asset reload
- Scripting integration
- Editor/inspector UI
- Prefabs/entity templates

### Phase 6: Web Builds 🌐
- WebGL backend integration
- WebGPU integration
- Asset pipeline for web
- Demo scene in browser

### Phase 7: Optimization 🚀
- Multithreading/job system
- Async resource loading
- Profiling tools
- Cross-platform builds
- Asset packaging/bundling

For detailed issue tracking, visit our [GitHub Issues](https://github.com/AakrishtSP/Rhodo/issues).

---

## 🚀 Getting Started

### Prerequisites

- **CMake** 3.31 or higher
- **C++17** compatible compiler:
  - GCC 7+ (Linux)
  - Clang 5+ (macOS/Linux)
  - MSVC 2017+ (Windows)
  - MinGW-w64 (Windows)
- **Git** for cloning the repository

### Building

1. **Clone the repository**
   ```bash
   git clone https://github.com/AakrishtSP/Rhodo.git
   cd Rhodo
   ```

2. **Create a build directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake**
   ```bash
   cmake ..
   ```
   
   Optional: Disable tests
   ```bash
   cmake .. -DRH_BUILD_TESTS=OFF
   ```

4. **Build the project**
   ```bash
   cmake --build .
   ```

   Or for a specific configuration:
   ```bash
   cmake --build . --config Release
   ```

### Running Examples

After building, example applications can be found in:
```
build/build/bin/<Platform>/ExampleApp/
```

Run the example:
```bash
# Linux/macOS
./build/build/bin/Linux/ExampleApp/ExampleApp

# Windows
build\build\bin\Windows\ExampleApp\ExampleApp.exe
```

### Running Tests

If tests are enabled:
```bash
ctest --test-dir build
```

Or run the test executable directly:
```bash
./build/build/bin/<Platform>/RhodoTests/RhodoTests
```

---

## 📁 Complete Project Structure

```
.
├── engine/             # Core engine library
│   ├── Rhodo.cppm     # Root module
│   ├── core/          # Core engine systems
│   ├── debug/         # Debug utilities
│   ├── logger/        # Logging system
│   ├── signals/       # Event/signal system
│   └── CMakeLists.txt
├── application/       # Editor & tools
│   ├── Application.cpp
│   └── CMakeLists.txt
├── tests/             # Unit tests
│   ├── engine/
│   ├── application/
│   └── CMakeLists.txt
├── cmake/             # Build system utilities
├── docs/              # Documentation
├── CMakeLists.txt     # Root CMake configuration
├── LICENSE            # Apache 2.0 license
├── README.md          # This file
└── CONTRIBUTING.md    # Contribution guidelines
```

---

## 📊 Development Status

| Component | Status |
|-----------|--------|
| Logger | ✅ Working |
| Signal System | ✅ Working |
| Build System | 🔄 In Progress |
| Layer System | 📋 Planned |
| ECS | 📋 Planned |
| Renderer | 📋 Planned |
| Physics | 📋 Planned |
| Audio | 📋 Planned |
| Editor | 📋 Planned |

**Legend:** ✅ Complete | 🔄 In Progress | 📋 Planned

---

## 🤝 Contributing

We welcome contributions from the community! Whether you're fixing bugs, adding features, or improving documentation, your help is appreciated.

Please read our [CONTRIBUTING.md](CONTRIBUTING.md) for:
- Code style guidelines
- Development workflow
- Pull request process
- Issue reporting guidelines

---

## 📄 License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

```
Copyright 2025 Aakrisht Sharma Paudel

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
```

---

## 📧 Contact

**Aakrisht Sharma Paudel**

- GitHub: [@AakrishtSP](https://github.com/AakrishtSP)
- Project: [https://github.com/AakrishtSP/Rhodo](https://github.com/AakrishtSP/Rhodo)

---

<div align="center">

**⭐ Star this repository if you find it interesting!**

Made with ❤️ by [@AakrishtSP](https://github.com/AakrishtSP)

</div>
