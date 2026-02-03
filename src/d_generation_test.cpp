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
#include <mutex>
#include <thread>

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
std::atomic<uint64_t> G = 0;
uint64_t G_MAX = UINT64_MAX;

struct Lockable_Map
{
    std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> data;
    mutable std::mutex mtx;

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return data.size();
    }

    void reserve(size_t s)
    {
        std::lock_guard<std::mutex> lock(mtx);
        data.reserve(s);
    }

    bool emplace(uint64_t id, std::shared_ptr<D_Tile> tile)
    {
        std::lock_guard<std::mutex> lock(mtx);
        return data.emplace(id, tile).second;
    }
};

Lockable_Map Used_Tiles;

/*
========================================================================================================================
- - Main Start - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Iterates through maps of varying sizes and outputs the designs to a folder.
 **********************************************************************************************************************/
void test_generations(size_t t_number)
{
    LOG_DEBUG(std::format("Starting thread[{}]", t_number));
    D_Map d_map(5, 5, 80, Tile_Map);
    while (Used_Tiles.size() < Tile_Map.size() && G < G_MAX)
    {
        d_map.generate();
        uint64_t current_g = G.fetch_add(1);
        std::string file_name = std::format("{}Size-10x10_G{}.jpg", DEFAULT_TEST_OUTPUT_IMG_PATH, current_g);
        if (!d_map.save(file_name))
            throw std::runtime_error(ERR_FORMAT("Failed saving map!"));
        LOG_DEBUG(std::format("Map generated, filename = {}", file_name));
        for (auto &&col : d_map.get_display_mat())
        {
            for (auto &&tile : col)
            {
                Used_Tiles.emplace(tile->get_id(), tile);
            }
        }
    }
    LOG_DEBUG(std::format("Ending thread[{}]", t_number));
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    std::cout << "- - - - Start D_Builder TEST - - - -" << std::endl;

    if (argc == 2)
    {
        try
        {
            G_MAX = std::stoull(argv[1]);
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << ERR_FORMAT("Invalid given as generation amount.") << std::endl;
            return EXIT_FAILURE;
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << ERR_FORMAT("Given number is to large for size_t.") << std::endl;
            return EXIT_FAILURE;
        }
    }

    init_img_dirs();
    std::filesystem::path img_dir(DEFAULT_INPUT_IMG_PATH);
    std::filesystem::path loaded_dir(DEFAULT_SECTION_IMG_LOADED_PATH);

    D_Tile::load_tiles(img_dir, loaded_dir);
    D_Tile::generate_tiles();

    Used_Tiles.reserve(Tile_Map.size());

    // Start up some threads to run generations
    unsigned int t = std::thread::hardware_concurrency();
    if (t == 0)
    {
        LOG_DEBUG("Unable to detect available thread count, defaulting to 4.");
        t = 4;
    }
    else
    {
        LOG_DEBUG(std::format("Assuming {} available threads.", t));
    }

    LOG_DEBUG("Launching generation threads...");
    std::vector<std::thread> thread_pool;
    for (size_t i = 0; i < 1; i++)
    {
        thread_pool.emplace_back(test_generations, i);
    }

    LOG_DEBUG("Awaiting generation termination...");
    for (auto &thread : thread_pool)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    LOG_DEBUG(std::format("Generation threads rejoined. {}/{} Tiles Used", Used_Tiles.size(), Tile_Map.size()));

    return EXIT_SUCCESS;
}
