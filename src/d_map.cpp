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
#include "d_builder_common.hpp"

/*
========================================================================================================================
- - Macros - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Max map size in both width and height, ie. 20x20.
 **********************************************************************************************************************/
#define MAX_MAP_SIZE (20)

/***********************************************************************************************************************
 * @brief Min map size in both width and height, ie. 2x2.
 **********************************************************************************************************************/
#define MIN_MAP_SIZE (2)

/*
========================================================================================================================
- - Class Methods - -
========================================================================================================================
*/
D_Map::D_Map(uint8_t in_cols, uint8_t in_rows, std::string in_theme)
{
    if (in_cols > MAX_MAP_SIZE ||
        in_rows > MAX_MAP_SIZE ||
        in_cols < MIN_MAP_SIZE ||
        in_rows < MIN_MAP_SIZE)
    {
        throw std::invalid_argument(ERR_FORMAT("Invalid sizes given to D_Map: Sizes must be between 2-20 inclusive!"));
    }
    else if (in_theme.empty())
    {
        throw std::invalid_argument(ERR_FORMAT("Empty theme string given as argument for D_Map theme!"));
    }

    cols = in_cols;
    rows = in_rows;
    theme = in_theme;
    // Get random number for seed, Init the generator, Define the range.
    // We set (0 to 1) since we will be checking if we want an exit first most likely. But we reset this later.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 1);

    generate();
}

D_Map::~D_Map()
{
    //! TODO: this
}

void D_Map::generate()
{
    // Reset our display matrix
    display_mat.clear();
    display_mat.resize(cols);
    for (size_t col = 0; col < cols; col++)
    {
        display_mat.at(col).resize(rows, {nullptr});
    }

    // Get a new map of tiles filtered by theme. TODO: Note that we only have one theme currently so we are using Tile_map.
    theme_map = Tile_Map;

    // Empty our unseedable sections.
    unseedables.clear();
    unseedables.reserve(cols * rows);

    // randomly pick an entrance and protect it.
    set_special_section(D_Special_Sections::Entrance);

    // randomly decide if we want an exit, set it, protect it, and test if it is reachable. Throw on a bad design.
    distr.param(std::uniform_int_distribution<int>::param_type(0, 1));
    if (distr(gen))
    {
        set_special_section(D_Special_Sections::Exit);
        if (!is_reachable(entrance, exit))
        {
            std::stringstream err;
            err << "Map design failure! Exit could not be reached!" << to_string();
            std::runtime_error(ERR_FORMAT(err.str()));
        }
    }

    // randomly pick seeds and protect them
    size_t seeds = cols * rows / 5; // 20% seed target.
    for (size_t seed = 0; seed < seeds; seed++)
        set_special_section(D_Special_Sections::Seeded);

    // randomly fill in the map connecting to every tile unless its a seed with no connections.
    fill_remaining_sections();

    // Check if we can reach every tile (unless its empty, or the entrance)
    for (size_t col = 0; col < cols; col++)
    {
        for (size_t row = 0; row < rows; row++)
        {
            if (std::pair<size_t, size_t>{col, row} != entrance &&
                0 != display_mat.at(col).at(row)->get_connections().mask &&
                !is_reachable(entrance, {col, row}))
            {
                std::stringstream err;
                err << "Map design failure! Section at [col:" << col << ",row:" << row << "] could not be reached!"
                    << to_string();
                std::runtime_error(ERR_FORMAT(err.str()));
            }
        }
    }
}

void D_Map::generate(uint8_t in_cols, uint8_t in_rows, std::string in_theme)
{
    if (in_cols > MAX_MAP_SIZE ||
        in_rows > MAX_MAP_SIZE ||
        in_cols < MIN_MAP_SIZE ||
        in_rows < MIN_MAP_SIZE)
    {
        throw std::invalid_argument(ERR_FORMAT("Invalid sizes given to D_Map::generate(): Sizes must be between 2-20 inclusive!"));
    }
    else if (in_theme.empty())
    {
        throw std::invalid_argument(ERR_FORMAT("Empty theme string given as argument for D_Map::generate() theme!"));
    }

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

/*
========================================================================================================================
- - Private Functions - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Selects a section in the map randomly and sets it as the entrance, exit or as a seeded tile. It then protects
 * the section by adding it and all the neighboring tiles to the unseedeables vector.
 *
 * @param[in] spec Type of section to set in the display mat.
 **********************************************************************************************************************/
void D_Map::set_special_section(D_Special_Sections spec)
{
    //! TODO: This
}

/***********************************************************************************************************************
 * @brief Tests if a given point of on the map is reachable from a given start point (usually the entrance).
 *
 * @param[in] start Point to start from.
 * @param[in] dest The point we want to test if we can get to.
 *
 * @retval bool If we were able to reach the destination tile.
 **********************************************************************************************************************/
bool D_Map::is_reachable(std::pair<size_t, size_t> start, std::pair<size_t, size_t> dest)
{
    //! TODO: this
}

/***********************************************************************************************************************
 * @brief Fills in the remaining sections of the map with tiles that match the required connections.
 **********************************************************************************************************************/
void fill_remaining_sections()
{
    //! TODO: this
}
