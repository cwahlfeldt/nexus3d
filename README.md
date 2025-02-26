# Nexus3D Game Engine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Nexus3D is a modern, cross-platform 3D game engine built in C that leverages SDL3, SDL3 GPU API, cglm for math operations, and flecs for Entity Component System architecture.

## Features

- **Modern Graphics**: Uses SDL3's new GPU API for efficient, cross-platform rendering
- **Entity Component System**: Built on flecs for exceptional performance and scalability
- **Math & Physics**: Utilizes cglm for optimized vector and matrix operations
- **Complete Core Systems**:
  - Renderer with PBR material support
  - Input management (keyboard, mouse, gamepad)
  - Audio system
  - Physics system
  - Resource management
  - Easy-to-use camera system
- **Modular Design**: Clean, separation of concerns with a focus on maintainability
- **Performance-Focused**: Optimized for real-time applications and games

## Building

### Prerequisites

Nexus3D has the following dependencies:
- SDL3 (with GPU API support)
- cglm
- flecs

The build system assumes these libraries are available in the `lib` directory.

### Compilation

```bash
# Clone the repository
git clone https://github.com/yourusername/nexus3d.git
cd nexus3d

# Create lib directory and place dependencies there
mkdir -p lib

# Build everything (static library, shared library, and examples)
make

# Build only the static library
make static

# Build only the shared library
make shared

# Build with debug symbols
make debug

# Install (default to /usr/local)
sudo make install

# Run examples
make run-examples
```

## Project Structure

```
nexus3d/
├── include/          # Public API headers
│   └── nexus3d/      # Main include directory
│       ├── core/     # Core engine functionality
│       ├── renderer/ # Rendering system
│       ├── ecs/      # Entity Component System
│       ├── input/    # Input handling
│       ├── audio/    # Audio system
│       ├── physics/  # Physics system
│       ├── math/     # Math utilities
│       └── utils/    # Utility functions
├── src/              # Source implementation
│   ├── core/         # Core implementation
│   ├── renderer/     # Renderer implementation
│   ├── ecs/          # ECS implementation
│   ├── input/        # Input implementation
│   ├── audio/        # Audio implementation
│   ├── physics/      # Physics implementation
│   ├── math/         # Math implementation
│   └── utils/        # Utilities implementation
├── examples/         # Example applications
├── build/            # Build output directory
└── lib/              # External dependencies
```

## Getting Started

Here's a simple example to create a window and initialize the engine:

```c
#include "nexus3d/nexus3d.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Initialize the engine
    if (!nexus3d_init()) {
        fprintf(stderr, "Failed to initialize Nexus3D Engine!\n");
        return EXIT_FAILURE;
    }
    
    // Print version information
    printf("Nexus3D Engine v%s\n", nexus3d_get_version());
    
    // Main game loop
    while (nexus_engine_is_running()) {
        // Update engine (processes input, physics, rendering, etc.)
        nexus_engine_update();
    }
    
    // Shutdown engine
    nexus3d_shutdown();
    
    return EXIT_SUCCESS;
}
```

For more complex examples, check the `examples/` directory.

## Documentation

Detailed documentation is available in the `docs/` directory. Key topics include:

- Engine Overview
- Renderer System
- Entity Component System
- Physics System
- Audio System
- Input System
- Math Utilities
- Resource Management

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- SDL3 Team for their amazing multimedia library
- cglm for the highly optimized graphics math library
- SanderMertens for creating flecs, an impressive ECS library
