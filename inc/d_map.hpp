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

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_tile.hpp"

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
    std::vector<std::vector<std::shared_ptr<D_Tile>>> const & get_display_mat();

private:
    std::vector<std::vector<std::shared_ptr<D_Tile>>> display_mat;
    std::vector<std::pair<size_t, size_t>> unseedables;
    std::string theme;
    uint8_t cols;
    uint8_t rows;
    bool has_exit_flag;
};

