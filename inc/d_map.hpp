#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include "d_tile.hpp"

class D_Map
{
public:
    D_Map(uint8_t cols, uint8_t rows);
    ~D_Map();
    bool const has_exit() const;
    void generate();
    void save(std::string path, std::string file_name) const;
    void swap_tile(uint8_t col, uint8_t row, std::shared_ptr<D_Tile> replacement);

private:
    std::vector<std::vector<std::shared_ptr<D_Tile>>> display_mat;
    std::string theme;
    uint8_t cols;
    uint8_t rows;
    bool has_exit_flag;
};
