# D_Builder
A desktop TTRPG dungeon builder that allows map section swaping after map generation. Custom tile sets are permited given they fit the tile requirements (see below). The following is an image of the UI with a 3x3 map of the first basic tile set.
![UI Example](ui_example.png)
The map can be dragged, zoomed in and out, and tiles can be swapped by right click along with changing map settings with the top buttons etc. Note that after changing some settings the map will need to be regenerated to reset the state.

## Planned Features
- New themes such as:
  - Fort
  - Cave
  - Bazzonad (hell/demon like)
  - and others.
- maybe SQLite later on -> for less memory usage? (not keeping all tiles in RAM all the time)

## Requirements

### 1st Party Libs
- [libcpp59](https://github.com/Gregory-Nitch/libcpp59)
*expected in user/local/ in the above build config but can be changed*

### 3rd Party Libs
- Qt 6.11.2 (I installed via the online installer)
  - *install directory can be altered in the above config*
- OpenGL (For Qt)
  - Ubuntu : `sudo apt install libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev`
- Vulkan (For Qt)
  - Ubuntu : `sudo apt install libvulkan-dev`
- Doxygen (optional)

### A Note on Large Qt Images
- Max allocation has been set to 1024 but if you need an overide you could execute with:
  - `QT_IMAGEIO_MAXALLOC=<allocation_amount_here> ./D_Builder`

## Hardware Requirements
- With a minimal tile set and after loading btop reported 1.2Gb of memory usage. For larger and multiple tile sets this will increase, expect something like 4~5Gb as a possiblity.

## Building
- `mkdir bld && cd bld`
- `cmake -S .. -B . -DCMAKE_BUILD_TYPE=<debug/release> -DCMAKE_PREFIX_PATH="$HOME/SW/Repos/Qt/6.11.2/gcc_64;/usr/local"`
- `cmake --build .`

### Testing
- `ctest`
  - Also note that numerous threads will then start generating maps until every tile in all sets in the input folder are used. If the test never stops you probably have a missing tile that is needed in your set (or there is an issue with the generation).

### Running
- Run with command (in same dir):
  - `D_Builder [no_gen]`
  - `no_gen` variable skips tile generation and just loads tiles, you can pass this as a CLI variable if you know the tiles have already been generated.
- Currently the logger just prints to std out and its level cannot be changed. (possible improvement)

### Compile Commands Symbolic Link
- `ln -s bld/compile_commands.json compile_commands.json`

## Contributing
- Contributions are welcome for things such as:
  - Windows / Apple support
  - New tile sets (would really like this as tile set creation of detailed tiles is time consuming)
  - Bug fixes, improvements, corrections, etc. 
    - though the project has basicly been finished to improvements really is ment to say performance improvements, other improvements can be preposed but aren't really being sought and performance wasn't really found to be an issue.
  - I don't really use Qt regularly so there may have been a better way to do things. Even outside of the UI I just kind of put the project together and didn't think about its design other than getting the generator working.

### Tile Requirements
You will need to supply tiles that match the current available input tiles, meaning their connections must match. Though theoritcally generation should be possible with any valid tile set that covers every possible connection within that set, other connection sets have not been tested so you may encounter errors. To ensure support make sure your tiles meet the following requirements.
- The filename must encode tile metadata using this format (you can inspect the given input tiles for examples):
  - name;theme;connections;entrance;exit;permutable;flippable.jpg
    - Where:
      - `connections` is a comma-separated list of T0-T7, R0-R7, B0-B7, or L0-L7 tokens, or `NA` for a tile with no connections. (this indicates a 8x8 tile as a requirement)
      - The remaining fields must be `true` or `false`.
      - A tile cannot be both an entrance and an exit.
      - Flippable tiles must also be permutable and must create a unique horizontal reflection.
      - Permutable tiles create unique rotations; a tile may be permutable without being flippable.
- The tile image must:
  - be the same size in pixels (recommend 512x512)
- Default tiles were made at:
  - [Inkarnate](https://inkarnate.com/). If you need to make other maps I recommend using it, its continously updated and comes with some things you can try out for free first (at least when I started using it).
*Note that since tiles are flipped and rotated to make other tiles the ground textures will also be rotated so using textures that don't stand out so much might suit tastes better. ie, coblestone may not look so good.*
- Tiles are placed in a directory containing one theme where a root directory contains the directories of themes. ie, themes/ and themes/your_theme_dir_here/ etc.
- The same input directory can be used as the default themes. (~/imgs/input/your_theme_dir_here/)
