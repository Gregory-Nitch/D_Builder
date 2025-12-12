#include <iostream>
#include <cstdlib>

#include "d_map.hpp"
#include "d_tile.hpp"
#include "d_builder_common.hpp"

std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Tile_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Entrance_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Exit_Map = {};

int main(int argc, char** argv)
{
    std::cout << "Welcome to D_Builder" << std::endl;

    D_Tile::load_tiles(std::string(DEFAULT_PATH));

    return EXIT_SUCCESS;
}
