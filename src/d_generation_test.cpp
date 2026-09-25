/***********************************************************************************************************************
 * @date 2026-01-17
 * @author Gregory Nitch
 *
 * @brief Application tests.
 **********************************************************************************************************************/

/*
************************************************************************************************************************
- - System Includes - -
************************************************************************************************************************
*/

#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <format>
#include <algorithm>
#include <cctype>
#include <mutex>
#include <thread>

/*
************************************************************************************************************************
- - Local Includes - -
************************************************************************************************************************
*/

#include "d_map.hpp"
#include "d_tile.hpp"
#include "d_builder_common.hpp"

/*
************************************************************************************************************************
- - Global Variable INIT - -
************************************************************************************************************************
*/

std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Tile_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Entrance_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Exit_Map = {};
std::unordered_map<std::string, std::filesystem::path> Loaded_Img_Dirs = {};
std::shared_ptr<D_Tile> Empty_Tile = nullptr;
std::unique_ptr<D_Map> Dungeon_Map = nullptr;
std::string Gen_Flag = GENERATE_IMG_CLI_COMMAND;
libcpp59::logger Logger = {};
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

/*
************************************************************************************************************************
- - Main Start - -
************************************************************************************************************************
*/

/***********************************************************************************************************************
 * @brief Iterates through generated maps and saves their designs to the test-output directory.
 *
 * @param[in] t_number Identifier for the generation worker thread.
 * @param[in] theme Name of the theme being tested.
 * @param[in] theme_tiles Tiles available for the theme being tested.
 * @param[in,out] used_tiles Tiles encountered during this theme's generations.
 * @param[in,out] generation_count Number of generations completed for this theme.
 **********************************************************************************************************************/
void test_generations(size_t t_number,
                      std::string const &theme,
                      std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> &theme_tiles,
                      Lockable_Map &used_tiles,
                      std::atomic<uint64_t> &generation_count)
{
    Logger.log(libcpp59::log_level::DEBUG, std::format("Starting thread[{}] for theme {}", t_number, theme));

    D_Map d_map(5, 5, 80, theme_tiles);
    while (used_tiles.size() < theme_tiles.size())
    {
        uint64_t current_g = generation_count.fetch_add(1);
        if (current_g >= G_MAX)
            break;

        d_map.generate();
        std::string file_name = std::format("{}{}_Size-10x10_G{}.jpg", DEFAULT_TEST_OUTPUT_IMG_PATH, theme, current_g);

        if (!d_map.save(file_name))
            Logger.log(libcpp59::log_level::ERR, "Failed saving map!");

        Logger.log(libcpp59::log_level::DEBUG, std::format("Map generated, filename = {}", file_name));
        for (auto &&col : d_map.get_display_mat())
        {
            for (auto &&tile : col)
            {
                used_tiles.emplace(tile->get_id(), tile);
            }
        }
    }
    Logger.log(libcpp59::log_level::DEBUG, std::format("Ending thread[{}] for theme {}", t_number, theme));
}

/***********************************************************************************************************************
 * @brief Initializes test data, runs concurrent map-generation tests, and reports the result.
 *
 * @param[in] argc Number of command-line arguments.
 * @param[in] argv Command-line argument values. The optional first argument sets the generation limit.
 *
 * @retval int Test process exit code.
 **********************************************************************************************************************/
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

    for (auto const &[theme, loaded_directory] : Loaded_Img_Dirs)
    {
        std::string theme_name = theme;
        std::transform(theme_name.begin(),
                       theme_name.end(),
                       theme_name.begin(),
                       [](unsigned char character)
                       { return static_cast<char>(std::tolower(character)); });
        std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> theme_tiles = D_Tile::filter_by_theme(theme_name);
        if (theme_tiles.empty())
        {
            Logger.log(libcpp59::log_level::ERR, std::format("No tiles found for theme {}.", theme));
            continue;
        }

        Lockable_Map used_tiles;
        used_tiles.reserve(theme_tiles.size());
        std::atomic<uint64_t> generation_count = 0;

        Logger.log(libcpp59::log_level::INFO, std::format("Launching generation threads for theme {}...", theme));
        std::vector<std::thread> thread_pool;
        for (size_t i = 0; i < t; i++)
        {
            thread_pool.emplace_back(test_generations,
                                     i,
                                     std::cref(theme),
                                     std::ref(theme_tiles),
                                     std::ref(used_tiles),
                                     std::ref(generation_count));
        }

        Logger.log(libcpp59::log_level::INFO, std::format("Awaiting generation termination for theme {}...", theme));
        for (auto &thread : thread_pool)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
        Logger.log(libcpp59::log_level::DEBUG,
                   std::format("Generation threads rejoined for theme {}. {}/{} Tiles Used",
                               theme,
                               used_tiles.size(),
                               theme_tiles.size()));
    }

    return EXIT_SUCCESS;
}
