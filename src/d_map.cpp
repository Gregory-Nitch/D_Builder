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
#include <sstream>
#include <memory>
#include <format>
#include <unordered_map>

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

/***********************************************************************************************************************
 * @brief Constructor for D_Map, creates the D_Map object and calls the generate function to create a design.
 *
 * @param[TODO] TODO TODO.
 * @param[TODO] TODO TODO.
 *
 * @retval TODO TODO.
 **********************************************************************************************************************/
D_Map::D_Map(uint8_t in_cols,
             uint8_t in_rows,
             uint8_t in_con_chance,
             std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> &usable_tiles = Tile_Map)
{
    if (in_cols > MAX_MAP_SIZE ||
        in_rows > MAX_MAP_SIZE ||
        in_cols < MIN_MAP_SIZE ||
        in_rows < MIN_MAP_SIZE)
    {
        throw std::invalid_argument(ERR_FORMAT("Invalid sizes given to D_Map: Sizes must be between 2-20 inclusive!"));
    }
    if (!usable_tiles)
    {
        throw std::invalid_argument(ERR_FORMAT("Usable tiles not given to the D_Map during construction!"));
    }

    cols = in_cols;
    rows = in_rows;
    connection_chance = in_con_chance;
    theme_map = usable_tiles;
    /* Get random number for seed, Init the generator, Define the range. We set (0 to in_cols) since we will be finding
     * the x of the entrance tile first but it will need to be reset later.
     */
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, in_cols - 1);

    generate();
}

D_Map::~D_Map()
{
    //! TODO: this
}

void D_Map::generate()
{
    reset_for_generate();
    start_generation_at_entrance();
    place_nodes();
    fill_empty_tiles();
}

void D_Map::generate(uint8_t in_cols,
                     uint8_t in_rows,
                     uint8_t in_con_chance,
                     std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> &usable_tiles = Tile_Map)
{
    if (in_cols > MAX_MAP_SIZE ||
        in_rows > MAX_MAP_SIZE ||
        in_cols < MIN_MAP_SIZE ||
        in_rows < MIN_MAP_SIZE)
    {
        throw std::invalid_argument(ERR_FORMAT("Invalid sizes given to D_Map::generate(): Sizes must be between 2-20 inclusive!"));
    }
    if (!usable_tiles)
    {
        throw std::invalid_argument(ERR_FORMAT("Usable tiles were empty when calling D_Map::generate(params)!"));
    }

    cols = in_cols;
    rows = in_rows;
    connection_chance = in_con_chance;
    theme_map = usable_tiles;
    generate();
}

void D_Map::save(std::string path, std::string file_name) const
{
    //! TODO: this
}

void D_Map::swap_tile(uint8_t col, uint8_t row, std::shared_ptr<D_Tile> replacement)
{
    display_mat.at(col).at(row) = replacement;
}

std::string const D_Map::to_string() const
{
    std::stringstream ss;
    ss << "\n\n- - - D_Map Info: - - -\n";
    ss << "\tTheme: " << theme << "\n";
    ss << "\tColumns: " << cols << "\n";
    ss << "\tRows: " << rows << "\n";
    ss << "\tConnection Chance: " << connection_chance << "\n";
    ss << "- - - Connections - - -\n";

    for (uint8_t row = 0; row < display_mat.at(0).size(); row++)
    {
        ss << "Row[" << row << "]:";
        for (uint8_t col = 0; col < display_mat.size(); col++)
        {
            ss << "[" << display_mat.at(col).at(row)->connections_to_string() << "]";
        }
        ss << "\n";
    }

    ss << "- - - - - - - - - - - -\n\n";
    return ss.str();
}

std::vector<std::vector<std::shared_ptr<D_Tile>>> const &D_Map::get_display_mat()
{
    return display_mat;
}

uint8_t D_Map::get_connection_chance() const
{
    return connection_chance;
}

/*
========================================================================================================================
- - Private Functions - -
========================================================================================================================
*/

void D_Map::reset_for_generate()
{
    display_mat.clear();
    to_visit.clear();
    display_mat.resize(cols);
    for (size_t col = 0; col < cols; col++)
    {
        display_mat.at(col).resize(rows, {nullptr});
    }
    to_visit.reserve(cols * rows);
}

void D_Map::start_generation_at_entrance()
{
    uint8_t ent_col, ent_row;
    D_Connections valid_connections = CONNECTION_FULL_MASK;

    distr.param(0, cols - 1);
    ent_col = distr(gen);
    distr.param(0, rows - 1);
    ent_row = distr(gen);

    if (0 == ent_col)
    {
        valid_connections.side_masks.left ^= CONNECTION_SIDE_MASK;
    }
    else if (cols - 1 == ent_col)
    {
        valid_connections.side_masks.right ^= CONNECTION_SIDE_MASK;
    }

    if (0 == ent_row)
    {
        valid_connections.side_masks.top ^= CONNECTION_SIDE_MASK;
    }
    else if (rows - 1 == ent_row)
    {
        valid_connections.side_masks.bottom ^= CONNECTION_SIDE_MASK;
    }

    std::shared_ptr<D_Tile> chosen_tile = chose_tile_based_on_connections(Entrance_Map, valid_connections);
    swap_tile(ent_col, ent_row, chosen_tile);

    valid_connections = chosen_tile->get_connections();
    if (valid_connections.side_masks.top)
        to_visit.push_back({col, row - 1});
    if (valid_connections.side_masks.right)
        to_visit.push_back({col + 1, row});
    if (valid_connections.side_masks.bottom)
        to_visit.push_back({col, row + 1});
    if (valid_connections.side_masks.left)
        to_visit.push_back({col - 1, row});
}

std::shared_ptr<D_Tile> D_Map::chose_tile_based_on_connections(std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> &tile_map,
                                                               D_Connections valid_connections,
                                                               D_Connections possible_connections = CONNECTION_ZERO_MASK)
{
    std::vector<std::shared_ptr<D_Tile>> tile_canidates = {};
    tile_canidates.reserve(tile_map.size());

    if (possible_connections)
    {
        for (auto tile_pair : tile_map)
        {
            // Check that there are connections in the possible directions.
            D_Connections tile_connections = tile_pair.second->get_connections();
            if (!possible_connections.side_masks.top &&
                tile_connections.side_masks.top)
                continue;
            if (!possible_connections.side_masks.bottom &&
                tile_connections.side_masks.bottom)
                continue;
            if (!possible_connections.side_masks.right &&
                tile_connections.side_masks.right)
                continue;
            if (!possible_connections.side_masks.left &&
                tile_connections.side_masks.left)
                continue;

            // Check that if we have valid connections in a direction that they match the canidate.
            //! NOTE: We can't use the full mask because there are connections that have not been set.
            if (valid_connections.side_masks.top &&
                valid_connections.side_masks.top != tile_connections.side_masks.top)
                continue;
            if (valid_connections.side_masks.bottom &&
                valid_connections.side_masks.bottom != tile_connections.side_masks.bottom)
                continue;
            if (valid_connections.side_masks.right &&
                valid_connections.side_masks.right != tile_connections.side_masks.right)
                continue;
            if (valid_connections.side_masks.left &&
                valid_connections.side_masks.left != tile_connections.side_masks.left)
                continue;

            // All condtions for the canidate have been met, add it to the list to choose from
            tile_canidates.push_back(tile_pair.second);
        }

        distr.param(0, tile_canidates.size() - 1);
        return tile_canidates.at(distr(gen));
    }

    // Else we only have valid connections and should match on that.
    for (auto tile_pair : tile_map)
    {
        if (valid_connections.mask == tile_pair.second->get_connections().mask)
            tile_canidates.push_back(tile_pair.second);
    }

    if (tile_canidates.empty())
    {
        std::stringstream err;
        err << "Whilst filtering canidates for an entrance we could not fit a tile that met requirements!"
            << " Valid connections were = int_mask:[" << valid_connections.mask << "]";
        throw std::runtime_error(ERR_FORMAT(err.str()));
    }

    distr.param(0, tile_canidates.size() - 1);
    return tile_canidates.at(distr(gen));
}

void D_Map::place_nodes()
{
    while (!to_visit.empty())
    {
        std::pair<uint8_t, uint8_t> current = to_visit.pop_front();
        D_Connections valid_connections = CONNECTION_ZERO_MASK;
        D_Connections possible_connections = CONNECTION_ZERO_MASK;
        calculate_connections_and_add_visitors(current, valid_connections, possible_connections);
        std::shared_ptr<D_Tile> chosen_tile = chose_tile_based_on_connections(theme_map,
                                                                              valid_connections,
                                                                              possible_connections);
    }
}

void D_Map::calculate_connections_and_add_visitors(std::pair<uint8_t, uint8_t> &const current_point,
                                                   D_Connections &valid_connections,
                                                   D_Connections &possible_connections)
{
    std::shared_ptr<D_Tile> n_tile = nullptr;
    distr.param(0, ONE_HUNDRED_PERCENT);

    // Check up
    if (0 != current_point.first &&
        (n_tile = display_mat.at(current_point.first - 1).at(current_point.second)) &&
        n_tile->get_connections().side_masks.bottom)
    {
        valid_connections.side_masks.top = n_tile->get_connections().side_masks.bottom;
        n_tile = nullptr;
    }
    else if (0 != current_point.first &&
             !(n_tile = display_mat.at(current_point.first - 1).at(current_point.second)) &&
             distr(gen) > connection_chance)
    {
        possible_connections.side_masks.top = CONNECTION_SIDE_MASK;
        to_visit.push_back({current_point.first - 1, current_point.second})
    }
    else
    {
        valid_connections.side_masks.top = CONNECTION_ZERO_MASK;
    }

    // Check down
    if (cols - 1 != current_point.first &&
        (n_tile = display_mat.at(current_point.first + 1).at(current_point.second)) &&
        n_tile->get_connections().side_masks.top)
    {
        valid_connections.side_masks.bottom = n_tile->get_connections().side_masks.top;
        n_tile = nullptr;
    }
    else if (cols - 1 != current_point.first &&
             !(n_tile = display_mat.at(current_point.first + 1).at(current_point.second)) &&
             distr(gen) > connection_chance)
    {
        possible_connections.side_masks.bottom = CONNECTION_SIDE_MASK;
        to_visit.push_back({current_point.first + 1, current_point.second})
    }
    else
    {
        valid_connections.side_masks.bottom = CONNECTION_ZERO_MASK;
    }

    // Check left
    if (0 != current_point.second &&
        (n_tile = display_mat.at(current_point.first).at(current_point.second - 1)) &&
        n_tile->get_connections().side_masks.right)
    {
        valid_connections.side_masks.left = n_tile->get_connections().side_masks.right;
        n_tile = nullptr;
    }
    else if (0 != current_point.second &&
             !(n_tile = display_mat.at(current_point.first).at(current_point.second - 1)) &&
             distr(gen) > connection_chance)
    {
        possible_connections.side_masks.left = CONNECTION_SIDE_MASK;
        to_visit.push_back({current_point.first, current_point.second - 1})
    }
    else
    {
        valid_connections.side_masks.left = CONNECTION_ZERO_MASK;
    }

    // Check right
    if (rows - 1 != current_point.second &&
        (n_tile = display_mat.at(current_point.first).at(current_point.second + 1)) &&
        n_tile->get_connections().side_masks.left)
    {
        valid_connections.side_masks.right = n_tile->get_connections().side_masks.left;
        n_tile = nullptr;
    }
    else if (rows - 1 != current_point.second &&
             !(n_tile = display_mat.at(current_point.first).at(current_point.second + 1)) &&
             distr(gen) > connection_chance)
    {
        possible_connections.side_masks.right = CONNECTION_SIDE_MASK;
        to_visit.push_back({current_point.first, current_point.second + 1})
    }
    else
    {
        valid_connections.side_masks.right = CONNECTION_ZERO_MASK;
    }
}

void D_Map::fill_empty_tiles()
{
    for (size_t col = 0; col < cols; col++)
    {
        for (size_t row = 0; row < rows; row++)
        {
            std::shared_ptr<D_Tile> tile = display_mat.at(col).at(row);
            if (!tile)
            {
                display_mat.at(col).at(row) = Empty_Tile;
            }
        }
    }
}
