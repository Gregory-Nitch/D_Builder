/***********************************************************************************************************************
 * @date 2026-01-17
 * @author Gregory Nitch
 *
 * @brief Application tests.
 **********************************************************************************************************************/

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <format>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_map.hpp"
#include "d_tile.hpp"
#include "d_builder_common.hpp"

/*
========================================================================================================================
- - Global Variable INIT - -
========================================================================================================================
*/

std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Tile_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Entrance_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Exit_Map = {};
std::shared_ptr<D_Tile> Empty_Tile = nullptr;
std::unique_ptr<D_Map> Dungeon_Map = nullptr;
std::string Gen_Flag = GENERATE_IMG_CLI_COMMAND;

/*
========================================================================================================================
- - Main Start - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Iterates through maps of varying sizes and outputs the designs to a folder.
 **********************************************************************************************************************/
void test_generations()
{
    std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> used_tiles;
    used_tiles.reserve(Tile_Map.size());
    size_t i = 0;
    while (used_tiles.size() < Tile_Map.size())
    {
        Dungeon_Map->generate();
        std::string file_name = std::format("{}Size-10x10_count{}.jpg", DEFAULT_TEST_OUTPUT_IMG_PATH, i);
        if (!Dungeon_Map->save(file_name))
            throw std::runtime_error(ERR_FORMAT("Failed saving map!"));
        i++;
        LOG_DEBUG(std::format("Map generated, filename = {}", file_name));
        for (auto &&col : Dungeon_Map->get_display_mat())
        {
            for (auto &&tile : col)
            {
                if (!used_tiles.contains(tile->get_id()))
                {
                    auto emplace_pair = used_tiles.emplace(tile->get_id(), tile.get());
                    if (!emplace_pair.second)
                    {
                        throw std::runtime_error(ERR_FORMAT("Failed emplacement into used map during map generations."));
                    }
                }
            }
        }
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    std::cout << "- - - - Start D_Builder TEST - - - -" << std::endl;

    init_img_dirs();
    std::filesystem::path img_dir(DEFAULT_INPUT_IMG_PATH);
    std::filesystem::path loaded_dir(DEFAULT_SECTION_IMG_LOADED_PATH);

    D_Tile::load_tiles(img_dir, loaded_dir);
    D_Tile::generate_tiles();

    Dungeon_Map = std::make_unique<D_Map>(10, 10, 80, Tile_Map);

    test_generations();

    return EXIT_SUCCESS;
}
