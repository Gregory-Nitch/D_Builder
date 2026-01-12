/***********************************************************************************************************************
 * LICENSE : TODO!
 *
 * @date 2025-12-14
 * @author Gregory Nitch
 *
 * @brief Header file for the D_Map class, contains a list of methods and member variables, for more detailed
 * information on methods @see d_map.cpp.
 **********************************************************************************************************************/

#pragma once

/*
========================================================================================================================
- - System Inculdes - -
========================================================================================================================
*/

#include <cstdint>
#include <deque>
#include <vector>
#include <string>
#include <memory>
#include <random>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_tile.hpp"

/*
========================================================================================================================
- - Macros - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Maximum amount of neighboors any tile will have.
 **********************************************************************************************************************/
#define MAX_NEIGHBOORS (4)

/***********************************************************************************************************************
 * @brief Represents 100%.
 **********************************************************************************************************************/
#define ONE_HUNDRED_PERCENT (100)

/*
========================================================================================================================
- - Start of D_Map - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @class D_Map
 * @brief Represents a map within the applications GUI.
 *
 * @members:
 *      TODO
 **********************************************************************************************************************/
class D_Map
{
public:
    D_Map(uint8_t in_cols, uint8_t in_rows, std::string in_theme);
    ~D_Map();
    void generate();
    void generate(uint8_t in_cols, uint8_t in_rows, std::string theme);
    void save(std::string path, std::string file_name) const;
    void swap_tile(uint8_t col, uint8_t row, std::shared_ptr<D_Tile> replacement);
    std::string const to_string() const;
    std::vector<std::vector<std::shared_ptr<D_Tile>>> const &get_display_mat();
    uint8_t get_connection_chance() const;

private:
    std::vector<std::vector<std::shared_ptr<D_Tile>>> display_mat;
    std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> theme_map; /*! TODO: Why don't we load theme's into individual maps?*/
    std::deque<std::pair<uint8_t, uint8_t>> to_visit;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> distr;
    std::string theme;
    uint8_t cols;
    uint8_t rows;
    uint8_t connection_chance; // Out of 100, values over or equal to 100 yield a 100% chance of connection.

    void reset_for_generate(void);
    void start_generation_at_entrance(void);
    std::shared_ptr<D_Tile> D_Map::chose_tile_based_on_connections(std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> &tile_map,
                                                                   D_Connections valid_connections,
                                                                   D_Connections possible_connections);
    void place_nodes(void);
    void calculate_connections_and_add_visitors(std::pair<uint8_t, uint8_t> &const current_point,
                                                D_Connections &valid_connections,
                                                D_Connections &possible_connections);
    void fill_empty_tiles(void);
};
