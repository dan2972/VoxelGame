# Voxel Game Engine
Another attempt at creating a voxel game inspired by Minecraft
created using C++ and OpenGL.

<details>
<summary>Table of Contents</summary>

- [Gallery](#gallery)
- [Features](#features)
- [Build](#build)
- [Controls](#controls)
- [Notes](#notes)

</details>

## Gallery

![Screenshot 2025-04-24 212323](https://github.com/user-attachments/assets/55bb9691-e4bf-4a47-bf81-d8936e966c61)
*Example showing the randomly generated terrain*

![Screenshot 2025-04-25 122015](https://github.com/user-attachments/assets/986f947f-ba67-49d9-8a37-b5b669dc7004)
*Example showing basic flood fill lighting*

![Screenshot 2025-04-25 123119](https://github.com/user-attachments/assets/9dacb7d6-ba36-4582-80e3-07c0014536fb)
*Example using Minecraft's textures*


## Features
- Custom OpenGL abstraction
- Infinite editable chunk-based terrain (including infinite height)
- Multithreaded chunk and mesh generation
- Smooth ambient occlusion
- Smooth flood fill lighting
- Frustum Culling
- Batched Font Rendering
- Dynamic texture atlas packing/creation (resizes atlas automatically)
- Custom 3D line renderer

## Build
This project uses VCPKG and CMake.

To build, use the provided CMake Preset. It is highly recommended to build in release mode.

```bash
cmake --preset=vcpkg-release
cmake --build build
.\build\VoxelGame
```

If using VSCode on Windows, you may need to define the CMake
variables: `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER`.

The recommended way to do this (especially if using vscode with the CMake extension) 
is to create and add a file `CMakeUserPresets.json` to the project root directory, and 
activate this preset instead of the one provided.

Example `CMakeUserPresets.json` file:
```json
{
    "version": 2,
    "configurePresets": [
        {
            "name": "default",
            "inherits": "vcpkg-release",
            "environment": {
                "VCPKG_ROOT": "C:/vcpkg"
            },
            "cacheVariables": {
                "CMAKE_C_COMPILER": "cl.exe",
                "CMAKE_CXX_COMPILER": "cl.exe"
            }
        }
    ]
}
```

Alternatively, you can add the variables to the CMake build command.

## Controls

- Use W, A, S, D and the mouse to move/look around
- Use space to fly up, shift to fly down, and hold ctrl while moving to speed up
- Press ESC to unlock the mouse
- Open the debug IMGUI menu to adjust or play around with some settings

## Notes

***This engine was created as a way to test my knowledge and skills in creating a relatively complex
program using OpenGL. As of the time of writing, I do not have any professional experience in working
on such programs. Consequently, there are likely many suboptimal design choices. However, I may attempt
to recreate this work using Vulkan. Hopefully, this experience could guide me to design a more
scalable program. Below are some additional notes.***

- Currently I use a set seed of 0. You can change this in `terrain_generator.cpp`.
- The multithreading system isn't perfect. There may be some unintended data race conditions, leading to a crash.
- This project targets OpenGL 3.3 because I sometimes use my mac to develop. This also means no
optimizations such as bindless rendering or vertex pulling. I may explore these if I decide to reimplement
this project in Vulkan.
- Currently, the Use Smooth Lighting option in the debug menu does not do anything.