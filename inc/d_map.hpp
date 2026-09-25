/***********************************************************************************************************************
 * @date 2025-12-14
 * @author Gregory Nitch
 *
 * @brief Header file for the D_Map class, contains a list of methods and member variables, for more detailed
 * information on methods @see d_map.cpp.
 **********************************************************************************************************************/

#pragma once

/*
************************************************************************************************************************
- - System Inculdes - -
************************************************************************************************************************
*/

#include <cstdint>
#include <deque>
#include <vector>
#include <string>
#include <memory>
#include <random>

/*
************************************************************************************************************************
- - Local Includes - -
************************************************************************************************************************
*/

#include "d_tile.hpp"
#include "d_builder_common.hpp"

/*
************************************************************************************************************************
- - Constants / Macros - -
************************************************************************************************************************
*/

/***********************************************************************************************************************
 * @brief Maximum amount of neighboors any tile will have.
 **********************************************************************************************************************/
constexpr uint8_t MAX_NEIGHBOORS = 4;

/***********************************************************************************************************************
 * @brief Represents 100%.
 **********************************************************************************************************************/
constexpr uint8_t ONE_HUNDRED_PERCENT = 100;

/*
************************************************************************************************************************
- - Globals - -
************************************************************************************************************************
*/

/***********************************************************************************************************************
 * @brief Neighboor offsets listed in a clockwise rotation for iteration during map generation.
 **********************************************************************************************************************/
constexpr std::array<std::pair<int8_t, int8_t>, 4> TILE_NEIGHBOOR_OFFSETS =
    {{
        // {col, row}
        {0, -1}, // Top neighboor
        {1, 0},  // Right
        {0, 1},  // Bottom
        {-1, 0}, // Left
    }};

/***********************************************************************************************************************
 * @brief Corner neighboor offsets listed in a clockwise rotation, starting from Top Right for iteration during map
 * generation. These are used when searching for possible corner connections.
 **********************************************************************************************************************/
constexpr std::array<std::pair<int8_t, int8_t>, 4> TILE_CORNER_NEIGHBOOR_OFFSETS =
    {{
        // {col, row}
        {1, -1},  // Top right neighboor
        {1, 1},   // Bottom right
        {-1, 1},  // Bottom Left
        {-1, -1}, // Top left
    }};

/***********************************************************************************************************************
 * @brief An array of indexs which will produce the neighbors opposing side index in the D_Connections union sides array.
 *
 * @warning This does NOT produce the side mask, it must be used to index into the sides array. @see D_Connections
 **********************************************************************************************************************/
constexpr std::array<uint8_t, 4> TILE_NEIGHBOOR_SIDE_IDX_MIRRORS =
    {
        2, // Current top idx becomes neighboor bottom idx, etc.
        3, // right = left
        0, // bottom = top
        1, // left = right
};

/*
************************************************************************************************************************
- - Start of D_Map - -
************************************************************************************************************************
*/

/***********************************************************************************************************************
 * @brief Represents a map within the applications GUI.
 *
 * @members:
 *      @private std::vector<std::vector<std::shared_ptr<D_Tile>>> display_mat = matrix of tiles that make up the actual
 *               map. ([col][row])
 *      @private std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> theme_map = map of tiles to use during generation.
 *      @private std::deque<std::pair<uint8_t, uint8_t>> to_visit = points in the map which need to be visited and have
 *               a tile assigned to them
 *      @private std::random_device rd = random device used for number generation.
 *      @private std::mt19937 gen = random number generator.
 *      @private std::uniform_int_distribution<unsigned long> distr = random number distrobution object.
 *      @private std::string theme = theme of the map.
 *      @private uint8_t cols = width of the map.
 *      @private uint8_t rows = height of the map.
 *      @private uint8_t connection_chance = chances that a tile will connection in a possible (ie, empty) direction.
 **********************************************************************************************************************/
class D_Map
{
public:
    /***********************************************************************************************************************
     * @brief Creates a map and generates its initial design.
     *
     * @param[in] in_cols The width of the map.
     * @param[in] in_rows The height of the map.
     * @param[in] in_con_chance Percentage chance for tiles to connect during generation.
     * @param[in] usable_tiles Map of tiles to use during generation.
     **********************************************************************************************************************/
    D_Map(uint8_t in_cols,
          uint8_t in_rows,
          uint8_t in_con_chance,
          std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> &usable_tiles);

    /***********************************************************************************************************************
     * @brief Destroys the map.
     **********************************************************************************************************************/
    ~D_Map();

    /***********************************************************************************************************************
     * @brief Generates a new map design using the current settings and tile map.
     **********************************************************************************************************************/
    void generate();

    /***********************************************************************************************************************
     * @brief Generates a new map design using the supplied settings and tile map.
     *
     * @param[in] in_cols New width of the map.
     * @param[in] in_rows New height of the map.
     * @param[in] in_con_chance New percentage chance of connections during generation.
     * @param[in] usable_tiles New map of tiles to use during generation.
     **********************************************************************************************************************/
    void generate(uint8_t in_cols,
                  uint8_t in_rows,
                  uint8_t in_con_chance,
                  std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> &usable_tiles);

    /***********************************************************************************************************************
     * @brief Saves the current map design as an image at the given path.
     *
     * @param[in] file_name File name to use when saving the map.
     *
     * @retval bool Whether the save was successful.
     **********************************************************************************************************************/
    bool save(std::string file_name) const;

    /***********************************************************************************************************************
     * @brief Replaces the tile at a point in the map display matrix.
     *
     * @param[in] col X coordinate in the map.
     * @param[in] row Y coordinate in the map.
     * @param[in] replacement Tile to place at the given coordinates.
     **********************************************************************************************************************/
    void swap_tile(uint8_t col, uint8_t row, std::shared_ptr<D_Tile> replacement);

    /***********************************************************************************************************************
     * @brief Returns the map settings and current design as a string.
     *
     * @retval std::string The map in a stringified form.
     **********************************************************************************************************************/
    std::string const to_string() const;

    /***********************************************************************************************************************
     * @brief Returns the map display matrix in [column][row] form.
     *
     * @retval std::vector<std::vector<std::shared_ptr<D_Tile>>> The display matrix.
     **********************************************************************************************************************/
    std::vector<std::vector<std::shared_ptr<D_Tile>>> const &get_display_mat();

    /***********************************************************************************************************************
     * @brief Returns the connection chance set for the map.
     *
     * @retval uint8_t The configured connection chance between tiles during generation.
     **********************************************************************************************************************/
    uint8_t get_connection_chance() const;

private:
    std::vector<std::vector<std::shared_ptr<D_Tile>>> display_mat;
    std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> theme_map;
    std::deque<std::pair<uint8_t, uint8_t>> to_visit;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<unsigned long> distr;
    std::string theme;
    uint8_t cols;
    uint8_t rows;
    uint8_t connection_chance; // Out of 100, values over or equal to 100 yield a 100% chance of connection.

    /***********************************************************************************************************************
     * @brief Resets the data structures used to generate a map design.
     **********************************************************************************************************************/
    void reset_for_generate(void);

    /***********************************************************************************************************************
     * @brief Places a random entrance and seeds the generation queue with its connected neighbours.
     **********************************************************************************************************************/
    void start_generation_at_entrance(void);

    /***********************************************************************************************************************
     * @brief Chooses a tile that satisfies required and possible connection masks.
     *
     * @param[in] required_connections Connections that must be present.
     * @param[in] possible_connections Connections that may be present.
     * @param[in] tile_map Tile map from which to choose.
     *
     * @retval std::shared_ptr<D_Tile> A tile that meets the connection requirements.
     **********************************************************************************************************************/
    std::shared_ptr<D_Tile> chose_tile_based_on_connections(D_Connections valid_connections,
                                                            D_Connections possible_connections,
                                                            std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> const &tile_map);

    /***********************************************************************************************************************
     * @brief Processes queued map positions and assigns a tile to each one.
     **********************************************************************************************************************/
    void place_nodes(void);

    /***********************************************************************************************************************
     * @brief Determines a point's required and possible connections and queues connected neighbours.
     *
     * @param[in] current_point Current map coordinate.
     * @param[inout] valid_connections Connection mask to fill with required connections.
     * @param[inout] possible_connections Connection mask to fill with possible connections.
     **********************************************************************************************************************/
    void calculate_connections_and_add_visitors(std::pair<uint8_t, uint8_t> const &current_point,
                                                D_Connections &valid_connections,
                                                D_Connections &possible_connections);

    /***********************************************************************************************************************
     * @brief Fills unassigned display-matrix positions with the empty tile.
     **********************************************************************************************************************/
    void fill_empty_tiles(void);
};
