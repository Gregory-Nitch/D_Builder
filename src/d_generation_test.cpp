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
    for (size_t i = 0; i < TEST_ITERATION_COUNT; i++)
    {
        Dungeon_Map->generate();
        std::string file_name = std::format("{}Size-3x3_count{}.jpg", DEFAULT_TEST_OUTPUT_IMG_PATH, i);
        if (!Dungeon_Map->save(file_name))
            throw std::runtime_error(ERR_FORMAT("Failed saving map!"));
        LOG_DEBUG(std::format("Map generated, filename = {}", file_name));
    }

    for (size_t i = 0; i < TEST_ITERATION_COUNT; i++)
    {
        Dungeon_Map->generate(5, 5, 50, Tile_Map);
        std::string file_name = std::format("{}Size-5x5_count{}.jpg", DEFAULT_TEST_OUTPUT_IMG_PATH, i);
        if (!Dungeon_Map->save(file_name))
            throw std::runtime_error(ERR_FORMAT("Failed saving map!"));
        LOG_DEBUG(std::format("Map generated, filename = {}", file_name));
    }

    for (size_t i = 0; i < TEST_ITERATION_COUNT; i++)
    {
        Dungeon_Map->generate(10, 10, 50, Tile_Map);
        std::string file_name = std::format("{}Size-10x10_count{}.jpg", DEFAULT_TEST_OUTPUT_IMG_PATH, i);
        if (!Dungeon_Map->save(file_name))
            throw std::runtime_error(ERR_FORMAT("Failed saving map!"));
        LOG_DEBUG(std::format("Map generated, filename = {}", file_name));
    }

    for (size_t i = 0; i < TEST_ITERATION_COUNT; i++)
    {
        Dungeon_Map->generate(15, 15, 50, Tile_Map);
        std::string file_name = std::format("{}Size-15x15_count{}.jpg", DEFAULT_TEST_OUTPUT_IMG_PATH, i);
        if (!Dungeon_Map->save(file_name))
            throw std::runtime_error(ERR_FORMAT("Failed saving map!"));
        LOG_DEBUG(std::format("Map generated, filename = {}", file_name));
    }

    for (size_t i = 0; i < TEST_ITERATION_COUNT; i++)
    {
        Dungeon_Map->generate(20, 20, 50, Tile_Map);
        std::string file_name = std::format("{}Size-20x20_count{}.jpg", DEFAULT_TEST_OUTPUT_IMG_PATH, i);
        if (!Dungeon_Map->save(file_name))
            throw std::runtime_error(ERR_FORMAT("Failed saving map!"));
        LOG_DEBUG(std::format("Map generated, filename = {}", file_name));
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

    Dungeon_Map = std::make_unique<D_Map>(3, 3, 50, Tile_Map);

    test_generations();

    return EXIT_SUCCESS;
}
