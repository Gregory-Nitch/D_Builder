
#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include "inc/d_map.hpp"

#define MAX_MAP_SIZE (20)

D_Map::D_Map(uint8_t cols, uint8_t rows)
{
    //! TODO: this
}

D_Map::~D_Map()
{
    //! TODO: this
}

bool const D_Map::has_exit()
{
    return has_exit_flag;
}

void D_Map::generate()
{
    //! TODO: this
}

void D_Map::save(std::string path, std::string file_name)
{
    //! TODO: this
}

void D_Map::swap_tile(uint8_t col, uint8_t row, std::shared_ptr<D_Tile> replacement)
{
    //! TODO: this
}
