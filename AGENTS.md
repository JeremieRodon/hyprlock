# Agent Guidelines for hyprlock

This document provides coding guidelines and standards for AI agents working on hyprlock, a GPU-accelerated screen lock for Hyprland.

## Project Overview

hyprlock is a C++23 project built with CMake that provides a GPU-accelerated screen lock utility for Hyprland using Wayland protocols, OpenGL ES, and PAM authentication.

## Build Commands

### Standard Build
```bash
# Configure (Release build)
cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -S . -B ./build

# Build
cmake --build ./build --config Release --target hyprlock -j$(nproc 2>/dev/null || getconf _NPROCESSORS_CONF)

# Install
sudo cmake --install build
```

### Debug Build
```bash
# Configure (Debug build)
cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -S . -B ./build

# Build
cmake --build ./build --config Debug --target hyprlock -j$(nproc 2>/dev/null || getconf _NPROCESSORS_CONF)
```

### Clean Build
```bash
rm -rf build/
# Then run configure and build commands above
```

## Linting and Formatting

### Format Code (clang-format)
```bash
# Format a single file
clang-format -i src/path/to/file.cpp

# Format all source files
find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

### Lint Code (clang-tidy)
```bash
# Lint a single file (requires compile_commands.json)
clang-tidy src/path/to/file.cpp

# Generate compile_commands.json (automatically done by CMakeLists.txt)
# It will be in build/compile_commands.json
```

**Note**: This project has no test suite. All testing is done manually.

## Code Style Guidelines

### Language Standard
- **C++23** is required (set in CMakeLists.txt)
- Use modern C++ features appropriately

### Formatting (from .clang-format)
- **Indentation**: 4 spaces, never tabs
- **Column limit**: 180 characters
- **Brace style**: Attach (K&R style)
  ```cpp
  void function() {
      // code
  }
  ```
- **Pointer alignment**: Left (`Type* ptr` not `Type *ptr`)
- **Line breaks**: Break ternary operators before `?`, break constructor initializers after `:`
- **Include order**: Preserve existing order (SortIncludes: false)
- **Namespace indentation**: All namespaces are indented

### Naming Conventions (from .clang-tidy)
- **Classes**: `CamelCase` with `C` prefix → `CMyClass`
- **Interfaces**: `CamelCase` with `I` prefix (ignored in check) → `IWidget`
- **Structs**: `CamelCase` with `S` prefix → `SRenderData`
- **Enums**: `camelBack` with `e` prefix → `eLogLevel`
- **Enum constants**: `UPPER_CASE` → `OUTPUT_INVALID`
- **Functions/Methods**: `camelBack` → `handleInput()`, `getFailText()`
- **Namespaces**: `CamelCase` with `N` prefix (if used)
- **Member variables**: Use `m_` prefix for class members → `m_vOutputs`, `m_bTerminate`
- **Global pointers**: Use `g_p` prefix → `g_pHyprlock`, `g_pConfigManager`

### Type Aliases (from defines.hpp)
Use project-specific smart pointer aliases:
```cpp
SP<Type>  // CSharedPointer (shared_ptr equivalent)
WP<Type>  // CWeakPointer (weak_ptr equivalent)
UP<Type>  // CUniquePointer (unique_ptr equivalent)
ASP<Type> // CAtomicSharedPointer (atomic shared_ptr)
AWP<Type> // CAtomicWeakPointer (atomic weak_ptr)
```

### Imports and Dependencies
- **Include order**: Preserve existing order in files (don't sort automatically)
- **Local includes**: Use relative paths with quotes → `#include "../../defines.hpp"`
- **System includes**: Use angle brackets → `#include <vector>`
- **Library includes**: Use angle brackets → `#include <hyprlang.hpp>`
- **Standard library**: Use C++ headers → `#include <string>` not `#include <string.h>`

### Error Handling
Use the assertion macros defined in `helpers/Log.hpp`:
```cpp
// With formatted message
RASSERT(condition, "Error: {} failed with code {}", var1, var2);

// Simple assertion
ASSERT(pointer != nullptr);
```
**Critical**: Assertions abort the program, so only use for unrecoverable errors.

### Logging
Use the `Debug::log()` function from `helpers/Log.hpp`:
```cpp
Debug::log(ERR, "Failed to initialize: {}", errorMsg);
Debug::log(WARN, "Timeout occurred after {}ms", timeout);
Debug::log(LOG, "Successfully loaded {} widgets", count);
Debug::log(TRACE, "Detailed debug info: {}", details); // Only shown with --verbose
```

Log levels: `TRACE`, `INFO`, `LOG`, `WARN`, `ERR`, `CRIT`, `NONE`

### Memory Management
- Prefer smart pointers using project aliases (`SP`, `UP`, `WP`)
- Use `makeShared<T>()` for creating shared pointers
- Raw pointers are acceptable for non-owning references (e.g., function parameters)

### Code Organization
Project structure:
```
src/
├── auth/          # Authentication implementations (PAM, fingerprint)
├── config/        # Configuration management and parsing
├── core/          # Core functionality (hyprlock, Output, Seat, Timer, EGL)
├── helpers/       # Utility functions (Color, Log, Math, AnimatedVariable)
└── renderer/      # Rendering engine
    ├── resources/ # Resource management (textures, text)
    └── widgets/   # UI widgets (Background, Label, Image, Shape, PasswordInputField)
```

### clang-tidy Compliance
The project uses **WarningsAsErrors: '*'**, meaning all clang-tidy warnings are treated as errors. Key enabled checks:
- `bugprone-*` (with some exceptions)
- `concurrency-*` (excluding mt-unsafe)
- `cppcoreguidelines-*` (with many exceptions for pragmatic C++)
- `modernize-*` (excluding trailing-return-type, auto inference, etc.)
- `performance-*`
- `readability-*` (excluding magic numbers, function size, etc.)

### Comments and Documentation
- Use `//` for single-line comments
- Use `/* */` for multi-line comments when appropriate
- Document non-obvious logic and complex algorithms
- Keep comments concise and up-to-date

### Best Practices
- Use `const` and `constexpr` where appropriate
- Prefer range-based for loops: `for (const auto& item : container)`
- Use structured bindings when helpful: `auto [key, value] = pair;`
- Use `std::format` and `std::print` for string formatting (C++23)
- Avoid magic numbers; use named constants or enums
- Check for null/validity before dereferencing pointers
- Keep functions focused and reasonably sized

## Common Patterns

### Widget Implementation
All widgets inherit from `IWidget` and implement:
```cpp
virtual void configure(const std::unordered_map<std::string, std::any>& prop, const SP<COutput>& pOutput);
virtual bool draw(const SRenderData& data);
virtual void onAssetUpdate(ResourceID id, ASP<CTexture> newAsset);
```

### Configuration Access
```cpp
static const auto VALUE = g_pConfigManager->getValue<Hyprlang::INT>("config:key");
```

### Wayland Protocol Integration
Protocol files are auto-generated in `protocols/` directory using `hyprwayland-scanner`.

## Version Information
Version is read from the `VERSION` file at build time and embedded as `HYPRLOCK_VERSION` macro.

## Important Notes
- This project has no automated tests - rely on manual testing
- All changes must pass clang-tidy checks (errors halt build)
- Format all code with clang-format before committing
- The project is Linux-specific (Wayland/Hyprland)
- Requires GPU acceleration (OpenGL ES 3.0+)
