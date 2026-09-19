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
std::unordered_map<std::string, std::filesystem::path> Loaded_Img_Dirs = {};
std::shared_ptr<D_Tile> Empty_Tile = nullptr;
std::unique_ptr<D_Map> Dungeon_Map = nullptr;
std::string Gen_Flag = GENERATE_IMG_CLI_COMMAND;
libcpp59::logger Logger = {};
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
    Logger.log(libcpp59::log_level::DEBUG, std::format("Starting thread[{}]", t_number));

    D_Map d_map(5, 5, 80, Tile_Map);
    while (Used_Tiles.size() < Tile_Map.size() && G < G_MAX)
    {
        d_map.generate();
        uint64_t current_g = G.fetch_add(1);
        std::string file_name = std::format("{}Size-10x10_G{}.jpg", DEFAULT_TEST_OUTPUT_IMG_PATH, current_g);

        if (!d_map.save(file_name))
            Logger.log(libcpp59::log_level::ERR, "Failed saving map!");

        Logger.log(libcpp59::log_level::DEBUG, std::format("Map generated, filename = {}", file_name));
        for (auto &&col : d_map.get_display_mat())
        {
            for (auto &&tile : col)
            {
                Used_Tiles.emplace(tile->get_id(), tile);
            }
        }
    }
    Logger.log(libcpp59::log_level::DEBUG, std::format("Ending thread[{}]", t_number));
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    Logger.set_log_level(libcpp59::log_level::DEBUG);
    Logger.log(libcpp59::log_level::INFO, "- - - - Start D_Builder TEST - - - -");
    if (argc == 2)
    {
        try
        {
            G_MAX = std::stoull(argv[1]);
        }
        catch (const std::invalid_argument &e)
        {
            Logger.log(libcpp59::log_level::ERR, std::format("Invalid value given as generation amount: {}", argv[1]));
            return EXIT_FAILURE;
        }
        catch (const std::out_of_range &e)
        {
            Logger.log(libcpp59::log_level::ERR, std::format("Given number is too large for size_t: {}", argv[1]));
            return EXIT_FAILURE;
        }
    }

    init_img_dirs();
    std::filesystem::create_directories(DEFAULT_TEST_OUTPUT_IMG_PATH);

    D_Tile::load_tiles();
    D_Tile::generate_tiles();

    Used_Tiles.reserve(Tile_Map.size());

    // Start up some threads to run generations
    unsigned int t = std::thread::hardware_concurrency();
    if (t == 0)
    {
        Logger.log(libcpp59::log_level::DEBUG, "Unable to detect available thread count, defaulting to 4.");
        t = 4;
    }
    else
    {
        Logger.log(libcpp59::log_level::DEBUG, std::format("Assuming {} available threads.", t));
    }

    Logger.log(libcpp59::log_level::INFO, "Launching generation threads...");
    std::vector<std::thread> thread_pool;
    for (size_t i = 0; i < t; i++)
    {
        thread_pool.emplace_back(test_generations, i);
    }

    Logger.log(libcpp59::log_level::INFO, "Awaiting generation termination...");
    for (auto &thread : thread_pool)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    Logger.log(libcpp59::log_level::DEBUG, std::format("Generation threads rejoined. {}/{} Tiles Used", Used_Tiles.size(), Tile_Map.size()));

    return EXIT_SUCCESS;
}
