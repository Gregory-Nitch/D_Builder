/***********************************************************************************************************************
 * LICENSE : TODO!
 *
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
std::unique_ptr<D_Map> Dungeon_Map = nullptr;

/*
========================================================================================================================
- - Main Start - -
========================================================================================================================
*/

int main(int argc, char **argv)
{
    //! TODO: Need to init below variable.
    std::filesystem::path img_dir(DEFAULT_INPUT_IMG_PATH);

    std::cout << "Welcome to D_Builder" << std::endl;

    D_Tile::load_tiles(img_dir);
    D_Tile::generate_tiles();
    Dungeon_Map = std::make_unique<D_Map>(3, 3, "fort");

    return EXIT_SUCCESS;
}
