/***********************************************************************************************************************
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
#include <iostream>
#include <sstream>
#include <memory>
#include <format>
#include <unordered_map>
#include <algorithm>

/*
========================================================================================================================
- - 3rd Party Includes - -
========================================================================================================================
*/

#include <QImage>
#include <QPainter>
#include <QString>

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
 * @param[in] in_cols The width of the map.
 * @param[in] in_rows The height of the map.
 * @param[in] in_con_chance Percentage chance for tiles to connect to each other during generation.
 * @param[in] usable_tiles Map of tiles to use during generation, defaults to the default map of tiles.
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
    if (usable_tiles.empty())
    {
        throw std::invalid_argument(ERR_FORMAT("Usable tiles not given to the D_Map during construction!"));
    }

    cols = in_cols;
    rows = in_rows;
    connection_chance = in_con_chance;
    theme_map = usable_tiles;
    gen.seed(rd());

    generate();
}

D_Map::~D_Map()
{
    //! TODO: this
}

/***********************************************************************************************************************
 * @brief Generates a new map design for the map using the currently set settings and tile map.
 **********************************************************************************************************************/
void D_Map::generate()
{
    LOG_DEBUG("Generate Start...");
    reset_for_generate();
    LOG_DEBUG("Map Reset...");
    start_generation_at_entrance();
    LOG_DEBUG("Entrance Placed...");
    place_nodes();
    LOG_DEBUG("Node Placement complete...");
    fill_empty_tiles();
    LOG_DEBUG("Filled empty tiles...");
    LOG_DEBUG(to_string());
}

/***********************************************************************************************************************
 * @brief Generates a new map design for the map using the passed settings and tile map.
 *
 * @param[in] in_cols New width of the map.
 * @param[in] in_rows New height of the map.
 * @param[in] in_con_chance New percentage chance of connections when generating designs.
 * @param[in] usable_tiles New map of tiles to use during generation.
 **********************************************************************************************************************/
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
    if (usable_tiles.empty())
    {
        throw std::invalid_argument(ERR_FORMAT("Usable tiles were empty when calling D_Map::generate(params)!"));
    }

    cols = in_cols;
    rows = in_rows;
    connection_chance = in_con_chance;
    theme_map = usable_tiles;
    generate();
}

/***********************************************************************************************************************
 * @brief Saves the current map design as an image to the given file name (and path).
 *
 * @param[in] file_name File name to use when saving the map.
 *
 * @retval bool Wether or not the save was succesful.
 **********************************************************************************************************************/
bool D_Map::save(std::string file_name) const
{
    size_t out_height = 0;
    size_t out_width = 0;

    for (const auto &tile : display_mat.at(0))
        out_height += tile->get_image()->height();

    for (const auto &col : display_mat)
        out_width += col.at(0)->get_image()->width();

    QImage result(static_cast<int>(out_width), static_cast<int>(out_height), QImage::Format_ARGB32);
    result.fill(Qt::transparent);
    QPainter painter(&result);
    size_t current_y = 0;

    for (size_t row = 0; row < static_cast<size_t>(rows); row++)
    {
        size_t current_x = 0;
        size_t row_height = 0;
        for (size_t col = 0; col < static_cast<size_t>(cols); col++)
        {
            std::shared_ptr<QImage> image = display_mat.at(col).at(row)->get_image();
            painter.drawImage(static_cast<int>(current_x),
                              static_cast<int>(current_y),
                              *image);
            current_x += image->width();
            row_height = std::max(row_height, static_cast<size_t>(image->height()));
        }
        current_y += row_height;
    }
    painter.end();

    return result.save(QString::fromStdString(file_name), "JPG", DEFAULT_OUTPUT_QUALITY);
}

/***********************************************************************************************************************
 * @brief Swaps the tile at the given point in the map display matrix.
 *
 * @param[in] col X coordinate in the map.
 * @param[in] row Y coordinate in the map.
 **********************************************************************************************************************/
void D_Map::swap_tile(uint8_t col, uint8_t row, std::shared_ptr<D_Tile> replacement)
{
    display_mat.at(col).at(row) = replacement;
}

/***********************************************************************************************************************
 * @brief Returns the D_Map with its settings and current design as a string.
 *
 * @retval std::string The map in a stringified form.
 **********************************************************************************************************************/
std::string const D_Map::to_string() const
{
    std::stringstream ss;
    ss << "\n\n- - - D_Map Info: - - -\n";
    ss << "\tTheme: " << theme << "\n";
    ss << "\tColumns: " << static_cast<int>(cols) << "\n";
    ss << "\tRows: " << static_cast<int>(rows) << "\n";
    ss << "\tConnection Chance: " << static_cast<int>(connection_chance) << "\n";
    ss << "- - - Connections - - -\n";

    for (size_t row = 0; row < static_cast<size_t>(rows); row++)
    {
        ss << "Row[" << row << "]:";
        for (size_t col = 0; col < static_cast<size_t>(cols); col++)
        {
            std::shared_ptr<D_Tile> tile = display_mat.at(col).at(row);
            if (tile)
                ss << "[" << tile->connections_to_string() << "]";
            else
                ss << "[null]";
        }
        ss << "\n";
    }

    ss << "- - - - - - - - - - - -\n\n";
    return ss.str();
}

/***********************************************************************************************************************
 * @brief Returns the display matrix of a map.
 *
 * @retval std::vector<std::vector<std::shared_ptr<D_Tile>>> A 2D vector of the map in [col][row] form.
 **********************************************************************************************************************/
std::vector<std::vector<std::shared_ptr<D_Tile>>> const &D_Map::get_display_mat()
{
    return display_mat;
}

/***********************************************************************************************************************
 * @brief Returns the connection chance set for the map.
 *
 * @retval uint8_t The set connection chance between tiles during generation.
 **********************************************************************************************************************/
uint8_t D_Map::get_connection_chance() const
{
    return connection_chance;
}

/*
========================================================================================================================
- - Private Functions - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Resets the data structures used to generate the map design, should be called before any other generation
 * processing.
 **********************************************************************************************************************/
void D_Map::reset_for_generate()
{
    display_mat.clear();
    to_visit.clear();
    display_mat.resize(cols);
    for (size_t col = 0; col < cols; col++)
    {
        display_mat.at(col).resize(rows, {nullptr});
    }
}

/***********************************************************************************************************************
 * @brief Starts the map generation by randomly placing an entrance in the display matrix and primes the to visit queue
 * with whatever tiles will be connected to that entrance.
 **********************************************************************************************************************/
void D_Map::start_generation_at_entrance()
{
    uint8_t ent_col, ent_row;
    D_Connections possible_connections = {.mask = CONNECTION_FULL_MASK};

    distr.param(std::uniform_int_distribution<unsigned long>::param_type(0, cols - 1));
    ent_col = static_cast<uint8_t>(distr(gen));
    distr.param(std::uniform_int_distribution<unsigned long>::param_type(0, rows - 1));
    ent_row = static_cast<uint8_t>(distr(gen));

    if (0 == ent_col)
    {
        possible_connections.side_masks.left ^= CONNECTION_SIDE_MASK;
    }
    else if (cols - 1 == ent_col)
    {
        possible_connections.side_masks.right ^= CONNECTION_SIDE_MASK;
    }

    if (0 == ent_row)
    {
        possible_connections.side_masks.top ^= CONNECTION_SIDE_MASK;
    }
    else if (rows - 1 == ent_row)
    {
        possible_connections.side_masks.bottom ^= CONNECTION_SIDE_MASK;
    }

    // Filter tiles that have connections outside of possible
    std::vector<std::shared_ptr<D_Tile>> tile_canidates = {};
    tile_canidates.reserve(Entrance_Map.size());
    for (auto &&tile_pair : Entrance_Map)
    {
        if ((tile_pair.second->get_connections().mask & ~possible_connections.mask) != 0)
        {
            continue;
        }
        tile_canidates.push_back(tile_pair.second);
    }

    if (tile_canidates.empty())
    {
        std::stringstream err;
        err << "Whilst filtering canidates for an entrance we could not find a tile that met requirements!"
            << " Possible connections were = int_mask:[" << possible_connections.mask << "]"
            << to_string();
        throw std::runtime_error(ERR_FORMAT(err.str()));
    }

    distr.param(std::uniform_int_distribution<unsigned long>::param_type(0, tile_canidates.size() - 1UL));
    std::shared_ptr<D_Tile> chosen_tile = tile_canidates.at(distr(gen));
    swap_tile(ent_col, ent_row, chosen_tile);

    D_Connections chosen_connections = chosen_tile->get_connections();
    for (size_t i = 0; i < MAX_NEIGHBOORS; i++)
    {
        uint8_t n_col = ent_col + static_cast<uint8_t>(TILE_NEIGHBOOR_OFFSETS[i].first);
        uint8_t n_row = ent_row + static_cast<uint8_t>(TILE_NEIGHBOOR_OFFSETS[i].second);
        if (n_col >= cols || n_row >= rows) // ie out of map bounds
        {
            continue;
        }
        else if (chosen_connections.sides[i])
        {
            to_visit.push_back({n_col, n_row});
        }
    }
}

/***********************************************************************************************************************
 * @brief Returns a pointer to a tile that meets the valid connection requirements set and that does not use connections
 * that are not present in the possible connection mask. If possible connections are not present a tile is selected
 * which matches its valid connections only.
 *
 * @param[in] required_connections Connections that need to be present.
 * @param[in] possible_connections Connections that may be present.
 *
 * @retval std::shared_ptr<D_Tile> A tile which meets the passed connection requirements, ie, all valid connections are
 * met and any set of possible connections may be met.
 **********************************************************************************************************************/
std::shared_ptr<D_Tile> D_Map::chose_tile_based_on_connections(D_Connections required_connections,
                                                               D_Connections possible_connections = {.mask = CONNECTION_ZERO_MASK},
                                                               std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> const &tile_map = Tile_Map)
{
    std::vector<std::shared_ptr<D_Tile>> tile_canidates = {};
    tile_canidates.reserve(tile_map.size());

    if (possible_connections.mask)
    {
        for (auto tile_pair : tile_map)
        {
            D_Connections tile_connections = tile_pair.second->get_connections();

            // Check required connections
            if ((tile_connections.mask & required_connections.mask) != required_connections.mask)
            {
                LOG_DEBUG("Tile missing required connections.");
                continue;
            }

            // Check for any connection outside of the complete connection mask, ie not in required, or not in possible.
            uint32_t complete_mask = required_connections.mask | possible_connections.mask;
            if ((tile_connections.mask & ~complete_mask) != 0)
            {
                LOG_DEBUG("Tile has connections outside of required and possible.");
                continue;
            }

            // Ensure that we have a connection in the valid possible directions
            bool missing_required_direction = false;
            for (int i = 0; i < MAX_NEIGHBOORS; ++i)
            {
                if (possible_connections.sides[i] && (possible_connections.sides[i] & tile_connections.sides[i]) == 0)
                {
                    missing_required_direction = true;
                    break;
                }
            }
            if (missing_required_direction)
            {
                LOG_DEBUG("Tile doesn't have connections to connect in one of the possible directions.");
                LOG_DEBUG(tile_pair.second->to_string());
                continue;
            }

            // All condtions for the canidate have been met, add it to the list to choose from
            tile_canidates.push_back(tile_pair.second);
        }
    }
    else
    {
        // Else we only have valid connections and should match on that.
        for (auto tile_pair : tile_map)
        {
            if (required_connections.mask == tile_pair.second->get_connections().mask)
                tile_canidates.push_back(tile_pair.second);
        }
    }

    if (tile_canidates.empty())
    {
        std::stringstream err;
        err << "Whilst filtering canidates we could not find a tile that met requirements!"
            << " Required connections were = int_mask:[" << required_connections.mask << "]"
            << " Possible connections were = int_mask:[" << possible_connections.mask << "]"
            << to_string();
        throw std::runtime_error(ERR_FORMAT(err.str()));
    }

    distr.param(std::uniform_int_distribution<unsigned long>::param_type(0, tile_canidates.size() - 1UL));
    return tile_canidates.at(distr(gen));
}

/***********************************************************************************************************************
 * @brief Iterates through the to visit queue and gets a tile for each visiting point in the map based on neighboors and
 * possible connections, while also placing new points in the queue. Works as the main generation loop for map
 * generation.
 **********************************************************************************************************************/
void D_Map::place_nodes()
{
    while (!to_visit.empty())
    {
        std::pair<uint8_t, uint8_t> current = to_visit.front();
        LOG_DEBUG(std::format("Visiting col:{} row:{}", current.first, current.second));
        to_visit.pop_front();
        D_Connections required_connections = {.mask = CONNECTION_ZERO_MASK};
        D_Connections possible_connections = {.mask = CONNECTION_ZERO_MASK};
        calculate_connections_and_add_visitors(current, required_connections, possible_connections);
        std::shared_ptr<D_Tile> chosen_tile = chose_tile_based_on_connections(required_connections,
                                                                              possible_connections);
        swap_tile(current.first, current.second, chosen_tile);
    }
}

/***********************************************************************************************************************
 * @brief Gets the required and possible connections for a point while also randomly selecting directions to connect in
 * where applicable. When an empty direction has been to choosen to connect to place it in the to visit list.
 *
 * @param[in] current_point The current tile, ie the tile we are currently at.
 * @param[inout] required_connections Connection mask to fill with connections we need to connect to.
 * @param[inout] connections Connection mask to maybe match connections with.
 **********************************************************************************************************************/
void D_Map::calculate_connections_and_add_visitors(std::pair<uint8_t, uint8_t> const &current_point,
                                                   D_Connections &required_connections,
                                                   D_Connections &possible_connections)
{
    distr.param(std::uniform_int_distribution<unsigned long>::param_type(0, ONE_HUNDRED_PERCENT));

    uint8_t current_col = current_point.first;
    uint8_t current_row = current_point.second;
    for (size_t i = 0; i < MAX_NEIGHBOORS; i++)
    {
        uint8_t n_col = current_col + static_cast<uint8_t>(TILE_NEIGHBOOR_OFFSETS[i].first);
        uint8_t n_row = current_row + static_cast<uint8_t>(TILE_NEIGHBOOR_OFFSETS[i].second);
        if (n_col >= cols || n_row >= rows) // ie out of map bounds
        {
            LOG_DEBUG(std::format("Out of bounds col:{} row:{}.", n_col, n_row));
            continue;
        }

        std::shared_ptr<D_Tile> n_tile = display_mat.at(n_col).at(n_row);
        if (n_tile) // Neighboor already set with connections
        {
            // Get our neighboors connections and reverse them
            uint8_t n_con_idx = TILE_NEIGHBOOR_SIDE_IDX_MIRRORS[i];
            required_connections.sides[i] = reverse_8bits(n_tile->get_connections().sides[n_con_idx]);
            LOG_DEBUG(std::format("Found set neighboor col:{} row:{}.", n_col, n_row));
        }
        else if (distr(gen) <= connection_chance) // Give a chance to possibly connect in that direction
        {
            possible_connections.sides[i] = CONNECTION_SIDE_MASK;
            std::pair<uint8_t, uint8_t> n_pair = {n_col, n_row};
            bool in_visit = false;
            for (auto &&pair : to_visit)
            {
                if (pair == n_pair)
                {
                    LOG_DEBUG(std::format("col:{} row:{} already in to visit.", n_pair.first, n_pair.second));
                    in_visit = true;
                    break;
                }
            }

            if (!in_visit)
            {
                to_visit.push_back(n_pair);
                LOG_DEBUG(std::format("Added col:{} row:{} to visit.", n_pair.first, n_pair.second));
            }
        }
        else // Don't connect
        {
            required_connections.sides[i] = CONNECTION_ZERO_MASK;
            LOG_DEBUG(std::format("Skipping neighboor at col:{} row:{}.", n_col, n_row));
        }
    }
    std::stringstream ss;
    ss << "Setting connections, possible mask = [" << possible_connections.mask << "], required mask = ["
       << required_connections.mask << "]";
    LOG_DEBUG(ss.str());
}

/***********************************************************************************************************************
 * @brief Iterates through the entire display matrix, when a nullptr is found we place an empty tile there which has
 * the backgound image texture for the map and no connections.
 **********************************************************************************************************************/
void D_Map::fill_empty_tiles()
{
    if (!Empty_Tile)
    {
        throw std::runtime_error(ERR_FORMAT("Empty Tile was null!"));
    }

    for (uint8_t col = 0; col < cols; col++)
    {
        for (uint8_t row = 0; row < rows; row++)
        {
            std::shared_ptr<D_Tile> tile = display_mat.at(col).at(row);
            if (!tile)
            {
                swap_tile(col, row, Empty_Tile);
            }
        }
    }
}
