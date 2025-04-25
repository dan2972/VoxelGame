# Voxel Game Engine
Another attempt at creating a voxel game inspired by Minecraft
created using C++ and OpenGL.

<details>
<summary>Table of Contents</summary>

- [Gallery](#gallery)
- [Features](#features)
- [Build](#build)

</details>

## Gallery
*An example showing the randomly generated terrain*
![Screenshot 2025-04-24 212323](https://github.com/user-attachments/assets/55bb9691-e4bf-4a47-bf81-d8936e966c61)
*An example showing basic flood fill lighting*
![Screenshot 2025-04-25 122015](https://github.com/user-attachments/assets/986f947f-ba67-49d9-8a37-b5b669dc7004)
*an example using Minecraft's textures*
![Screenshot 2025-04-25 123119](https://github.com/user-attachments/assets/9dacb7d6-ba36-4582-80e3-07c0014536fb)


## Features
- Custom OpenGL abstraction
- Infinite editable chunk-based terrain (including infinite height)
- Multithreaded chunk generation
- Smooth ambient occlusion
- Basic flood fill lighting
- Frustum Culling
- Batched Font Rendering
- Dynamic texture atlas packing/creation (resizes atlas automatically)
- Custom 3D line renderer
- Separation of logic and rendering timesteps

## Build
This project uses VCPKG and CMake.

To build, use the provided CMake Preset (vcpkg).

```bash
cmake --preset=vcpkg-release
cmake --build build
.\build\VoxelGame
```

If using VSCode on Windows, you may need to define the CMake
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
