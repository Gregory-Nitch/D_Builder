/***********************************************************************************************************************
 * LICENSE : TODO!
 *
 * @date 2025-12-14
 * @author Gregory Nitch
 *
 * @brief Implementation file of the D_Map class, contains for detailed information on its methods.
 **********************************************************************************************************************/

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <format>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_map.hpp"

/*
========================================================================================================================
- - Macros - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Max map size in both width and height, ie. 20x20.
 **********************************************************************************************************************/
#define MAX_MAP_SIZE (20)

/*
========================================================================================================================
- - Class Methods - -
========================================================================================================================
*/

D_Map::D_Map(uint8_t in_cols, uint8_t in_rows, std::string in_theme)
{
    cols = in_cols;
    rows = in_rows;
    theme = in_theme;
    generate();
}

D_Map::~D_Map()
{
    //! TODO: this
}

void D_Map::generate()
{
    //! TODO: this
    // resize our display matrix
    // get a new map of tiles filtered by theme
    // randomly pick an entrance
    // protect entrance
    // randomly decide if we want an exit
    // if has exit, randomly pick it
    // test if the exit is reachable
    //   if not error
    // protect exit
    // randomly pick seeds and protect them
    // randomly fill in the map connecting to every tile unless its a seed with no connections.
    // test that every tile is reachable
    //   if not error
}

void D_Map::generate(uint8_t in_cols, uint8_t in_rows, std::string in_theme)
{
    cols = in_cols;
    rows = in_rows;
    theme = in_theme;
    generate();
}

void D_Map::save(std::string path, std::string file_name) const
{
    //! TODO: this
}

void D_Map::swap_tile(uint8_t col, uint8_t row, std::shared_ptr<D_Tile> replacement)
{
    //! TODO: this
}

std::string const D_Map::to_string() const
{
    //! TODO: this
    return "";
}

std::vector<std::vector<std::shared_ptr<D_Tile>>> const &D_Map::get_display_mat()
{
    return display_mat;
}
