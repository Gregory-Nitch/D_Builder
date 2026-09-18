# D_Builder
Desktop TTRPG dungeon builder

# Building
- `mkdir bld && cd bld`
- `cmake -S .. -B . -DCMAKE_BUILD_TYPE=<debug/release> -DCMAKE_PREFIX_PATH="$HOME/Repos/Qt/6.11.1/gcc_64;/usr/local"`
- `cmake --build .`

## Testing
- `ctest`

## Compile Commands Symbolic Link
- `ln -s bld/compile_commands.json compile_commands.json`

## Planned Features
- Map section image loading.
- Map image section swaping.
- 'Random' dungeon generation.
- Map merged images saving.

## 1st Party Libs
- [libcpp59](https://github.com/Gregory-Nitch/libcpp59)
*expected in user/local/ in the above build config but can be changed*

## 3rd Party Libs
- Qt 6.11.1
  - *install directory can be altered in the above config*
- OpenGL (For Qt)
  - Ubuntu : `sudo apt install libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev`
- Vulkan (For Qt)
  - Ubuntu : `sudo apt install libvulkan-dev`
- maybe SQLite later on
