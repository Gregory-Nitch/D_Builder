#include <iostream>
#include <cstdlib>

#include "d_map.hpp"
#include "d_tile.hpp"
#include "d_builder_common.hpp"

extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Tiles_Map;
extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Entrance_Tiles;
extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Exit_Tiles;

int main(int argc, char** argv)
{
    std::cout << "Welcome to D_Builder" << std::endl;

    return EXIT_SUCCESS;
}
