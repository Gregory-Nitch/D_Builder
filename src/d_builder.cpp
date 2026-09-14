/***********************************************************************************************************************
 * @date 2025-12-14
 * @author Gregory Nitch
 *
 * @brief Application main, starts the app and does other initializations.
 **********************************************************************************************************************/

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <string>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_map.hpp"
#include "d_tile.hpp"
#include "d_builder_common.hpp"

/*
========================================================================================================================
- - Global Variable INIT - -
========================================================================================================================
*/

std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Tile_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Entrance_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Exit_Map = {};
std::unordered_map<std::string, std::filesystem::path> Loaded_Img_Dirs = {};
std::shared_ptr<D_Tile> Empty_Tile = nullptr;
std::unique_ptr<D_Map> Dungeon_Map = nullptr;
std::string Gen_Flag = GENERATE_IMG_CLI_COMMAND;

/*
========================================================================================================================
- - Main Start - -
========================================================================================================================
*/

int main(int argc, char **argv)

{
    std::cout << "Welcome to D_Builder" << std::endl;

    init_img_dirs();

    std::filesystem::path img_dir(DEFAULT_INPUT_IMG_PATH);             //! TODO: Need to handle subdirectory tile sets based on theme
    std::filesystem::path loaded_dir(DEFAULT_SECTION_IMG_LOADED_PATH); //! TODO: Need to place loaded tiles in subdirectories based on theme

    if (2 == argc && !Gen_Flag.compare(argv[1]))
    {
        D_Tile::load_tiles();
        D_Tile::generate_tiles();
    }
    else // Only loading required.
    {
        LOG_DEBUG("Skipping tile generation...");
        D_Tile::load_tiles();
    }

    Dungeon_Map = std::make_unique<D_Map>(3, 3, 50, Tile_Map);

    return EXIT_SUCCESS;
}
