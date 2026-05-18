# MAGE-Engine

**M**asterful **A**pplication **G**ame **E**ngine - a Vulkan-based C++ framework for building games and emulators from scratch.

## About

MAGE is a personal project initially made for my university and born out of a desire to actually understand what's happening under the hood, from the GPU command buffers up to the game loop, instead of reaching for a black-box engine every time. The goal is to grow it into a small, hackable foundation that I can use to build:

- 2D games (maybe 2.5D and Isometrics in the future)
- Emulators (CHIP-8 was the first target; more to come)
- Any side-project that needs a rendering surface, input, and a place to draw pixels

It's a learning project at first but a reusable library second.

## Tech / Required Tools

To build the engine you'll need:

- **Git**
- **CMake** >= 3.26
- **GCC/G++** (or any C++20-capable compiler)
- **Vulkan SDK** - graphics API
- **slangc** - Slang shader compiler (must be on `PATH`), usually comes with the Vulkan SDK installation.

Dependencies fetched automatically via CMake `FetchContent`:

- [GLFW](https://www.glfw.org/) 3.4
- [GLM](https://github.com/g-truc/glm) 1.0.1 — math
- [Dear ImGui](https://github.com/ocornut/imgui) 1.92.7 — debug UI

On Linux you'll also need the usual X11 dev packages (`Xrandr`, `Xxf86vm`, `Xi`, `pthread`, `dl`).

## Building

```sh
cmake -B build
cmake --build build
```

By default this builds the `mage` static library plus the example apps under `example/`. To consume MAGE from
another CMake project, link against `MAGE::mage` and turn off `MAGE_BUILD_EXAMPLES`.

## TODO

- [ ] Hot-reload for shaders
- [ ] Debug UI with ImGui
- [ ] Implement VMA (Vulkan Memory Allocation)
- [ ] Audio subsystem
- [ ] Asset pipeline - model loading (glTF), texture streaming
- [ ] Scene graph / proper ECS for `GameObject`
- [ ] CI build (Linux + Windows)

## Project Layout

```
src/        # engine source (library)
include/    # third-party headers fetched at configure time
shaders/    # Slang shaders, compiled to SPIR-V at build time
example/    # example apps that link against MAGE::mage
scripts/    # helper scripts (shader build, etc.)
```