# Voxel Game Engine
Another attempt at creating a voxel game inspired by Minecraft
created using C++ and OpenGL.

<details>
<summary>Table of Contents</summary>

- [Features](#features)
- [Build](#build)

</details>

## Features
- Custom OpenGL abstraction
- Batched Font Rendering
- Dynamic texture atlas packing/creation (resizes automatically)
- Separation of logic and rendering timesteps

## Build
This project uses VCPKG and CMake.

To build, use the provided CMake Preset (vcpkg).

```bash
cmake --preset=vcpkg
cmake --build build
.\build\VoxelGame
```

On Windows, you may need to define the CMake
variables: `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER`.

The recommended way to do this (especially if using vscode with the CMake extension) 
is to create and add a file `CMakeUserPresets.json` to the root directory, and 
activate this preset instead of the one provided.

Example `CMakeUserPresets.json` file:
```json
{
    "version": 2,
    "configurePresets": [
        {
            "name": "default",
            "inherits": "vcpkg",
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