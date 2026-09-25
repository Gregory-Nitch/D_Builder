/***********************************************************************************************************************
 * @date 2025-12-14
 * @author Gregory Nitch
 *
 * @brief Application main, starts the app and does other initializations.
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
#include <algorithm>
#include <limits>

/*
************************************************************************************************************************
- - 3rd Party Includes - -
************************************************************************************************************************
*/

#include <QApplication>
#include <QCoreApplication>
#include <QEventLoop>
#include <QImageReader>
#include <QProgressDialog>

/*
************************************************************************************************************************
- - Local Includes - -
************************************************************************************************************************
*/

#include "d_map.hpp"
#include "d_tile.hpp"
#include "d_builder_common.hpp"
#include "d_builder_ui.hpp"

/*
************************************************************************************************************************
- - Global Variable INIT - -
************************************************************************************************************************
*/

std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Tile_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Entrance_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Exit_Map = {};
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Active_Theme_Map = {};
std::unordered_map<std::string, std::filesystem::path> Loaded_Img_Dirs = {};
std::shared_ptr<D_Tile> Empty_Tile = nullptr;
std::unique_ptr<D_Map> Dungeon_Map = nullptr;
std::string Gen_Flag = NO_GENERATE_IMG_CLI_COMMAND;
libcpp59::logger Logger = {};

/*
************************************************************************************************************************
- - Main Start - -
************************************************************************************************************************
*/

/***********************************************************************************************************************
 * @brief Initializes D_Builder, loads tiles, and starts the Qt event loop.
 *
 * @param[in] argc Number of command-line arguments.
 * @param[in] argv Command-line argument values.
 *
 * @retval int Application exit code.
 **********************************************************************************************************************/
int main(int argc, char **argv)

{
    Logger.log(libcpp59::log_level::INFO, "Welcome to D_Builder.");
    QApplication app(argc, argv);
    QImageReader::setAllocationLimit(1024);

    init_img_dirs();
    QProgressDialog tile_progress;
    tile_progress.setWindowTitle("D_Builder");
    tile_progress.setCancelButton(nullptr);
    tile_progress.setWindowModality(Qt::ApplicationModal);
    tile_progress.setMinimumDuration(0);
    tile_progress.setAutoClose(false);
    tile_progress.setAutoReset(false);

    auto update_tile_progress = [&tile_progress](std::string const &phase, size_t completed, size_t total)
    {
        size_t const maximum_value = static_cast<size_t>(std::numeric_limits<int>::max());
        int const maximum = static_cast<int>(std::min(total, maximum_value));
        int const value = static_cast<int>(std::min(completed, maximum_value));
        tile_progress.setLabelText(QString::fromStdString(phase) + " (" + QString::number(completed) + " of " +
                                   QString::number(total) + ")");
        tile_progress.setRange(0, std::max(1, maximum));
        tile_progress.setValue(std::min(value, std::max(1, maximum)));
        tile_progress.show();
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    };

    if (2 == argc && !Gen_Flag.compare(argv[1])) // User skipped generation.
    {
        Logger.log(libcpp59::log_level::INFO, "Skipping tile generation.");
        D_Tile::load_tiles(DEFAULT_IMG_LOADED_ROOT_PATH, update_tile_progress);
    }
    else
    {
        D_Tile::load_tiles(DEFAULT_INPUT_IMG_PATH, update_tile_progress);
        D_Tile::generate_tiles(update_tile_progress);
    }

    if (Tile_Map.empty())
    {
        Logger.log(libcpp59::log_level::ERR, "No tiles loaded when filtering by theme! Exiting...");
        return EXIT_FAILURE;
    }
    // We are just grabing the first theme available as the default
    std::string default_theme = Tile_Map.begin()->second->get_theme();
    Active_Theme_Map = D_Tile::filter_by_theme(default_theme);
    Logger.log(libcpp59::log_level::INFO, "Default theme: " + default_theme);
    //! NOTE: the .ui file has these 3x3 and 80% settings set for default values.
    Dungeon_Map = std::make_unique<D_Map>(3, 3, 80, Active_Theme_Map);

    DBuilderUI gui;
    gui.show();
    tile_progress.close();

    return app.exec();
}
