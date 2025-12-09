# Rhodo Module Naming Convention

## ✅ Final Schema (PascalCase Hierarchical)

All modules follow strict PascalCase naming with hierarchical structure:

### Primary Modules

| Module          | File           | Purpose                    |
|-----------------|----------------|----------------------------|
| `Rhodo`         | `Rhodo.cppm`   | root engine module         |
| `Rhodo.Core`    | `Core.cppm`    | Core engine systems        |
| `Rhodo.Debug`   | `Debug.cppm`   | Debug assertions & logging |
| `Rhodo.Logger`  | `Logger.cppm`  | Logging system             |
| `Rhodo.Signals` | `Signals.cppm` | Signal/event system        |

### Module Partitions (Implementation Details)

| Module                           | File                    | Purpose                   |
|----------------------------------|-------------------------|---------------------------|
| `Rhodo.Logger:Structures`        | `Structures.cppm`       | Logger data structures    |
| `Rhodo.Logger:Impl`              | `LoggerImpl.cppm`       | Logger implementation     |
| `Rhodo.Signals:Signal`           | `Signal.cppm`           | Signal implementation     |
| `Rhodo.Signals:SignalHub`        | `SignalHub.cppm`        | Signal hub implementation |
| `Rhodo.Signals:ScopedConnection` | `ScopedConnection.cppm` | Connection RAII wrapper   |

---

## 🎯 Naming Rules

### ✅ DO

- **PascalCase everywhere**: `Logger`, `SignalHub`, `ScopedConnection`
- **Hierarchical with dots**: `Rhodo.Logger`, `Rhodo.Signals`
- **Partitions with colons**: `Rhodo.Logger:Impl`, `Rhodo.Signals:ScopedConnection`
- **Descriptive names**: `ScopedConnection` (not just `Scoped` or `Connection`)
- **Max 3 levels deep**: `Rhodo.Module.SubModule`

### ❌ DON'T

- **lowercase**: `rhodo`, `logger`, `signal`
- **snake_case**: `rhodo_logger`, `signal_hub`
- **Mix cases**: `Rhodo.logger`, `rhodo.Logger`
- **Non-descriptive partitions**: `:Impl` without context
- **Too deep**: `Rhodo.Logger.File.Stream:Util`

---

## 📋 Enforcement

CMake validates:

- ✅ Module declarations use `Rhodo.*` namespace
- ✅ Filenames match partition/module names in PascalCase
- ✅ No mixing of cases
- ✅ Partition filenames are PascalCase

**Enable/disable:**

```bash
cmake -DRH_ENFORCE_MODULE_NAME_MATCH=ON|OFF
```

---

## 🔍 Examples

### Correct

```cpp
// File: Logger.cppm
export module Rhodo.Logger;
export namespace rhodo::logger { ... }

// File: SignalHub.cppm
export module Rhodo.Signals:SignalHub;
module Rhodo.Signals:SignalHub;

// File: ScopedConnection.cppm
export module Rhodo.Signals:ScopedConnection;
```

### Incorrect

```cpp
// ❌ File: logger.cppm
export module rhodo.logger;

// ❌ File: signal_hub.cppm
export module Rhodo.Signals:SignalHub;

// ❌ File: Connection.cppm declaring
export module Rhodo.Signals:ScopedConnection;
```

---

## 🏗️ Current Structure

```
Rhodo/
├── Rhodo.cppm                    (export module Rhodo;)
├── Core/
│   └── Core.cppm                 (export module Rhodo.Core;)
├── Debug/
│   └── Debug.cppm                (export module Rhodo.Debug;)
├── Logger/
│   ├── Logger.cppm               (export module Rhodo.Logger;)
│   ├── Structures.cppm           (export module Rhodo.Logger:Structures;)
│   └── LoggerImpl.cppm            (export module Rhodo.Logger:Impl;)
└── Signals/
    ├── Signals.cppm              (export module Rhodo.Signals;)
    ├── Signal.cppm               (export module Rhodo.Signals:Signal;)
    ├── SignalHub.cppm            (export module Rhodo.Signals:SignalHub;)
    └── ScopedConnection.cppm     (export module Rhodo.Signals:ScopedConnection;)
```

---

## 🔑 Key Points

1. **Filenames MUST match module declarations** (PascalCase)
2. **Partitions can be multi-word**: `ScopedConnection`, not just `Scoped`
3. **All modules start with `Rhodo`** - it's the ABI root
4. **Enforcement is strict** - CMake fails if violated
5. **Partitions are private** - only public API exported from main module

---

**Last Updated:** December 9, 2025
