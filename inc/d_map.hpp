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
- - Start of D_Special_Sections - -
========================================================================================================================
*/

enum class D_Special_Sections
{
    Entrance,
    Exit,
    Seeded,
};

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

private:
    std::vector<std::vector<std::shared_ptr<D_Tile>>> display_mat;
    std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> theme_map; /*! TODO: Why don't we load theme's into individual maps?*/
    std::vector<std::pair<size_t, size_t>> unseedables;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> distr;
    std::string theme;
    std::pair<size_t, size_t> entrance;
    std::pair<size_t, size_t> exit;
    uint8_t cols;
    uint8_t rows;
    bool has_exit_flag;

    void set_special_section(D_Special_Sections spec);
    bool is_reachable(std::pair<size_t, size_t> start, std::pair<size_t, size_t> dest);
    void fill_remaining_sections();
};
