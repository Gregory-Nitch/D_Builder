/***********************************************************************************************************************
 * LICENSE : TODO!
 *
 * @date 2025-12-14
 * @author Gregory Nitch
 *
 * @brief Common header file for the D_Builder application, here common macros, definitions and app global variables
 * are defined.
 **********************************************************************************************************************/

#pragma once

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <format>

/*
========================================================================================================================
- - Macros - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Default input image path for the application, tiles are expected to be placed in this directory.
 **********************************************************************************************************************/
#define DEFAULT_INPUT_IMG_PATH "./imgs/input"

/***********************************************************************************************************************
 * @brief Default D_Tile loaded path for the application, tiles are expected to move and save here on application start
 * and when new tiles have been generated.
 **********************************************************************************************************************/
#define DEFAULT_SECTION_IMG_LOADED_PATH "./imgs/loaded"

/***********************************************************************************************************************
 * @brief Produces a std::format from the passed error message. Adds filename, function name, line and an ERR
 * identifier.
 * @param msg Message to be formated into the string.
 **********************************************************************************************************************/
#define ERR_FORMAT(msg) \
    std::format("ERR:{}:{}:{}: {}", __FILE__, __func__, __LINE__, msg)

/***********************************************************************************************************************
 * @brief Prints the given message to std::cout along with filename, function name and line.
 * @param msg Message to be logged.
 **********************************************************************************************************************/
#define LOG_DEBUG(msg) \
    std::cout << "INF:" << __FILE__ << ":" << __func__ << ":" << __LINE__ << ":" << msg << std::endl

/*
========================================================================================================================
- - App Globals - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Global map of all D_Tiles loaded by the program, normally initialized at application start.
 **********************************************************************************************************************/
extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Tile_Map;

/***********************************************************************************************************************
 * @brief Global map of all entrance D_Tiles loaded by the program, normally initialized at application start. These are
 * shared with the Tile_Map global variable.
 **********************************************************************************************************************/
extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Entrance_Map;

/***********************************************************************************************************************
 * @brief Global map of all exit D_Tiles loaded by the program, normally initialized at application start. These are
 * shared with the Tile_Map global variable.
 **********************************************************************************************************************/
extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Exit_Map;

extern std::unique_ptr<D_Tile> Dungeon_Map;
