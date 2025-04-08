# Voxel Game Engine
Another attempt at creating a voxel game inspired by Minecraft
created using C++ and OpenGL (May switch to vulkan in future rewrite).

## Build
This project uses VCPKG and CMake.

To build, simply use the defined CMake Preset (vcpkg) and build.

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

Alternatively, you can add the variables to the cmake build command.