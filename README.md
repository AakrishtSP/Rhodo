<div align="center">

# 🎮 Rhodo Game Engine

**A modern, cross-platform game engine built with C++17**

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![CMake](https://img.shields.io/badge/CMake-3.31+-green.svg)](https://cmake.org/)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](https://github.com/AakrishtSP/Rhodo)

</div>

---

## 📋 Table of Contents

- [About](#about)
- [Features](#features)
- [Roadmap](#roadmap)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
  - [Running Examples](#running-examples)
- [Project Structure](#project-structure)
- [Development Status](#development-status)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

---

## 🎯 About

Rhodo is an ambitious, open-source game engine designed for flexibility, performance, and cross-platform compatibility. Built from the ground up with modern C++ practices, Rhodo aims to provide developers with a robust foundation for creating 2D and 3D games that run on desktop platforms and the web.

The engine is currently in **active development**, following a phased approach to ensure each component is properly architected and tested.

---

## ✨ Features

### Current Features
- **🔧 Core Infrastructure**
  - Cross-platform support (Windows, Linux, macOS)
  - Flexible logging system with multiple severity levels
  - Type-safe signal/event system for decoupled communication
  - Precompiled header support for faster compilation
  - CMake-based build system

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

## 📁 Project Structure

```
Rhodo/
├── Rhodo/              # Core engine library
│   ├── include/        # Public headers
│   │   └── Rhodo/
│   │       ├── Core/   # Core engine systems
│   │       └── Signals/ # Event/signal system
│   └── src/            # Implementation files
│       └── Core/
├── Examples/           # Example applications
│   └── src/
├── Tests/              # Unit tests
├── CMakeLists.txt      # Root CMake configuration
├── LICENSE             # Apache 2.0 license
├── README.md           # This file
└── CONTRIBUTING.md     # Contribution guidelines
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
