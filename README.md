# D_Builder
A desktop TTRPG dungeon builder that allows map section swaping after map generation. Custom tile sets are permited given they fit the tile requirements (see below).

# Planned Features
- New themes such as:
  - Fort
  - Cave
  - Bazzonad
  - and others.

# Requirements

## 1st Party Libs
- [libcpp59](https://github.com/Gregory-Nitch/libcpp59)
*expected in user/local/ in the above build config but can be changed*

## 3rd Party Libs
- Qt 6.11.2
  - *install directory can be altered in the above config*
- OpenGL (For Qt)
  - Ubuntu : `sudo apt install libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev`
- Vulkan (For Qt)
  - Ubuntu : `sudo apt install libvulkan-dev`
- Doxygen
- maybe SQLite later on -> for less memory usage?

## A Note on Large Qt Images
- Max allocation has been set to 1024 but if you need an overide you could execute with:
  - `QT_IMAGEIO_MAXALLOC=<allocation_amount_here> ./D_Builder`

## Hardware Requirements
- With a minimal tile set and after loading btop reported 1.9Gb of memory usage. For larger and multiple tile sets this will increase, expect something like 4~5Gb as a possiblity.

# Building
- `mkdir bld && cd bld`
- `cmake -S .. -B . -DCMAKE_BUILD_TYPE=<debug/release> -DCMAKE_PREFIX_PATH="$HOME/SW/Repos/Qt/6.11.2/gcc_64;/usr/local"`
- `cmake --build .`

## Testing
- `ctest`
  - Also note that numerous threads will then start generating maps until every tile in all sets in the input folder are used. If the test never stops you probably have a missing tile that is needed in your set (or there is an issue with the generation).

## Compile Commands Symbolic Link
- `ln -s bld/compile_commands.json compile_commands.json`

# Contributing
- Contributions are welcome for things such as:
  - Windows / Apple support
  - New tile sets (would really like this as tile set creation of detailed tiles is time consuming)
  - Bug fixes, improvements, etc. (though the project has basicly been finished to improvements really is ment to say performance improvements, other improvements can be preposed but aren't really being sought)

## Tile Requirements
You will need to supply tiles that match the current available input tiles, meaning their connections must match. Though theoritcally generation should be possible with any valid tile set that covers every possible connection within that set, other connection sets have not been tested so you may encounter errors. To ensure support make sure your tiles meet the following requirements.
- The filename must encode tile metadata using this format (you can inspect the given input tiles for examples):
  - name;theme;connections;entrance;exit;permutable;flippable.jpg
    - Where:
      - `connections` is a comma-separated list of T0-T7, R0-R7, B0-B7, or L0-L7 tokens, or `NA` for a tile with no connections.
      - The remaining fields must be `true` or `false`.
      - A tile cannot be both an entrance and an exit.
      - Flippable tiles must also be permutable and must create a unique horizontal reflection.
      - Permutable tiles create unique rotations; a tile may be permutable without being flippable.
- The tile image must:
  - be a jpg
  - be 512x512 pixels
- Default tiles were made at:
  - [Inkarnate](https://inkarnate.com/). If you need to make other maps I recommend using it, its continously updated and comes with some things you can try out for free first (at least when I started using it).
*Note that since tiles are flipped and rotated to make other tiles the ground textures will also be rotated so using textures that don't stand out so much might suit tastes better. ie, coblestone may not look so good.*
