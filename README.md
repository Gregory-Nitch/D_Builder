# D_Builder
Desktop TTRPG dungeon builder

# Building
- `mkdir bld && cd bld`
- `cmake -CMAKE_BUILD_TYPE=<debug/release> ..`
- `cmake --build .`

## Compile Commands Symbolic Link
- `ln -s bld/compile_commands.json compile_commands.json`

## Planned Features
- Map section image loading.
- Map image section swaping.
- 'Random' dungeon generation.
- Map merged images saving.

## 3rd Party Libs
- Qt 6.11.1
- OpenGL (For Qt)
- - Ubuntu : `sudo apt install libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev`
- Vulkan (For Qt)
- - Ubuntu : `sudo apt install libvulkan-dev`
- maybe SQLite later on
