/***********************************************************************************************************************
 * LICENSE : TODO!
 *
 * @date : 2025-12-14
 * @author : Gregory Nitch
 *
 * @brief : D_Tile implementation functions. This class represents a section that can be used to construct the D_Map
 * object's display matrix.
 **********************************************************************************************************************/

/*
========================================================================================================================
- - Sysytem Includes - -
========================================================================================================================
*/
#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <exception>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/
#include "d_tile.hpp"
#include "d_builder_common.hpp"

/*
========================================================================================================================
- - Macros - -
========================================================================================================================
*/
/***********************************************************************************************************************
 * @brief Max possible permutations of a tile that is not flippable.
 **********************************************************************************************************************/
#define MAX_PERMUTATIONS (3)

/***********************************************************************************************************************
 * @brief Max possible permutations of a tile that is flippable.
 **********************************************************************************************************************/
#define MAX_FLIPPABLE_PERMUTATIONS (7)

/***********************************************************************************************************************
 * @brief Max possible number of connections a tile can have.
 **********************************************************************************************************************/
#define TILE_CONNECTION_MAX (32)

/***********************************************************************************************************************
 * @brief Max possible tokens a tile filename should have.
 **********************************************************************************************************************/
#define FILE_NAME_TOKEN_NUM (7)

/***********************************************************************************************************************
 * @brief Index of the tile name in the token vector when constructing a tile.
 **********************************************************************************************************************/
#define TILE_NAME_IDX (0)

/***********************************************************************************************************************
 * @brief Index of the tile theme in the token vector when constructing a tile.
 **********************************************************************************************************************/
#define TILE_THEME_IDX (1)

/***********************************************************************************************************************
 * @brief Index of the tile's entrance flag in the token vector when constructing a tile.
 **********************************************************************************************************************/
#define TILE_ENT_FLG_IDX (3)

/***********************************************************************************************************************
 * @brief Index of the tile's exit flag in the token vector when constructing a tile.
 **********************************************************************************************************************/
#define TILE_EXT_FLG_IDX (4)

/***********************************************************************************************************************
 * @brief Index of the tile's permutable flag in the token vector when constructing a tile.
 **********************************************************************************************************************/
#define TILE_PERM_FLG_IDX (5)

/***********************************************************************************************************************
 * @brief Index of the tile's flippable flag in the token vector when constructing a tile.
 **********************************************************************************************************************/
#define TILE_FLIP_FLG_IDX (6)

/***********************************************************************************************************************
 * @brief Initial mask given to a tile before mapping its connection to bits, also given to tiles with no connections.
 **********************************************************************************************************************/
#define CONNECTION_INIT_MASK (0x0)

/***********************************************************************************************************************
 * @brief Expected string when parsing a tile that has no connections.
 **********************************************************************************************************************/
#define NA_CONNECTION_TOKEN ("NA")

/*
========================================================================================================================
- - Class Methods - -
========================================================================================================================
*/

/**
 * @brief Constructor for a Dungeon Tile.
 * @param[in] in_path Path to the image file.
 * @warning This image file name must be in the following format:
 * name;theme;connections,with,comma,separated,values;is_entrance;is_exit;is_permutateable;is_flippable
 * For example:
 * 3WayInter0;fort;T3,T4,R3,R4,B3,B4;false;false;true;false
 * Additionally, this image is expected to permutate, it should have top connections as it will be the base image that
 * all permutations will be made from.
 * @note If there are no connections that section of the filename, it should have NA placed there.
 * @throws std::invalid_argument if in_path filename is empty, if name member ends up empty, if theme member ends up
 * empty, or if it is labeled as both an entrance and an exit.
 */
D_Tile::D_Tile(std::filesystem::path const &in_path)
{
    if (in_path.filename().generic_string().empty())
        throw std::invalid_argument(ERR_FORMAT("Empty filename in path given to D_Tile()!"));

    std::vector<std::string> file_tokens;
    std::string file_token;
    std::vector<std::string> connection_tokens;
    std::string connection_token;
    std::stringstream file_str_stream(in_path.filename().generic_string());
    char const semi_colon = ';';
    char const comma = ',';

    file_tokens.reserve(FILE_NAME_TOKEN_NUM);
    connection_tokens.reserve(TILE_CONNECTION_MAX);

    // Parse info
    while (std::getline(file_str_stream, file_token, semi_colon))
    {
        file_tokens.push_back(file_token);
    }

    std::stringstream connection_string_stream(file_tokens.at(2));
    while (std::getline(connection_string_stream, connection_token, comma))
    {
        connection_tokens.push_back(connection_token);
    }

    // Set members
    path = in_path;
    name = file_tokens.at(TILE_NAME_IDX);
    theme = file_tokens.at(TILE_THEME_IDX);
    id = id_counter.fetch_add(1);
    map_connection_tokens(connection_tokens);
    is_entrance_flag = file_tokens.at(TILE_ENT_FLG_IDX).compare("true") ? true : false;
    is_exit_flag = file_tokens.at(TILE_EXT_FLG_IDX).compare("true") ? true : false;
    is_permutateable_flag = file_tokens.at(TILE_PERM_FLG_IDX).compare("true") ? true : false;
    is_flippable_flag = file_tokens.at(TILE_FLIP_FLG_IDX).compare("true") ? true : false;

    if (name.empty())
        throw std::invalid_argument(ERR_FORMAT("Tile name found to be empty at end of D_Tile()!"));
    if (theme.empty())
        throw std::invalid_argument(ERR_FORMAT("Tile theme found to be empty at end of D_Tile!"));
    if (is_entrance_flag && is_exit_flag)
        throw std::invalid_argument(ERR_FORMAT("A tile cannot be both an entrance and an exit!"));
}

/**
 * @brief Destructor.
 */
D_Tile::~D_Tile()
{
    //! TODO: This?
}

/**
 * @brief Loads all the tiles from a given directory.
 * @param dir_path Directory path to a group of images to load.
 * @note Does not generate permutations in the global maps, if that is required call generate_tiles. Doing so will also
 * create images for the application to use.
 */
void D_Tile::load_tiles(std::filesystem::directory_entry const &dir_path)
{
    //! TODO: This
}

/**
 * @brief Generates tiles from the D_Tiles loaded in load_tiles(), this will also create permutation images of
 * permutable tiles and save them.
 * @param dir_path Directory path to save permutated D_Tile images into.
 * @note Generates permutations in the global maps.
 */
void D_Tile::generate_tiles(std::filesystem::directory_entry const &dir_path)
{
    //! TODO: This
}

/**
 * @brief Gets the name of the tile.
 * @returns Name of the tile.
 */
std::string const &D_Tile::get_name() const
{
    return name;
}

/**
 * @brief Gets the theme of the tile.
 * @returns Theme of the tile.
 */
std::string const &D_Tile::get_theme() const
{
    return theme;
}

/**
 * @brief Gets the ID of the tile.
 * @returns ID of the tile.
 */
uint64_t const D_Tile::get_id() const
{
    return id;
}

/**
 * @brief Gets the connection bit map of the tile.
 * @returns Connection bit map of the tile.
 */
D_Connections const D_Tile::get_connections() const
{
    return connections;
}

/**
 * @brief Gets the permutable flag of the tile.
 * @returns Whether or not the tile is permutable of the tile.
 * @note Tiles that were made from permutable tiles are not permutable.
 */
bool const D_Tile::is_permutateable() const
{
    return is_permutateable_flag;
}

/**
 * @brief Gets the entrance flag of the tile.
 * @returns Whether or not the tile is an entrance tile.
 */
bool const D_Tile::is_entrance() const
{
    return is_entrance_flag;
}

/**
 * @brief Gets the exit flag of the tile.
 * @returns Whether or not the tile is an exit tile.
 */
bool const D_Tile::is_exit() const
{
    return is_exit_flag;
}

/**
 * @brief Gets the flippable flag of the tile.
 * @returns Whether or not the tile is flippable.
 * @note Tiles made from flippable tiles are not set as flippable themselves after their creation.
 */
bool const D_Tile::is_flippable() const
{
    return is_flippable_flag;
}

/**
 * @brief Outputs the tile information string form.
 * @returns The tile information as a string in the form of:
 * ID:<id>,Name:<name>,Theme:<theme>,Connections:<connections,>,Entrance:<flag>,Exit:<flag>,Permutable:<flag>,Flippable:<flag>
 */
std::string const D_Tile::to_string() const
{
    std::stringstream ss;
    ss << "ID:" << id << ",Name:" << name << ",Theme:" << theme << ",Connections:";

    for (std::pair<uint32_t, std::string> pair : Connection_Bit_Mask_to_Str_Map)
    {
        if (connections.mask & pair.first)
            ss << Connection_Bit_Mask_to_Str_Map.at(pair.first) << ",";
    }

    ss << ",Entrance:";
    is_entrance_flag ? ss << "is entrance" : ss << "not entrance";
    ss << ",Exit:";
    is_exit_flag ? ss << "is exit" : ss << "not exit";
    ss << ",Permutable:";
    is_permutateable_flag ? ss << "is permutable" : ss << "not permutable";
    ss << ",Flippable:";
    is_flippable_flag ? ss << "is flippable" : ss << "not flippable";

    return ss.str();
}

/*
========================================================================================================================
- - Private Methods - -
========================================================================================================================
*/

/**
 * @brief Private constructor to build new D_Tile objects from permutaion values.
 */
D_Tile::D_Tile(std::filesystem::path permutation_path,
               std::string permutation_name,
               std::string permutation_theme,
               uint64_t permutation_id,
               D_Connections permutation_connections,
               bool permutation_is_entrance_flag,
               bool permutation_is_exit_flag,
               bool permutation_is_permutable_flag,
               bool permutation_is_flippable_flag)
{
    if (permutation_name.empty())
        throw std::invalid_argument(ERR_FORMAT("Permutating tile was given an empty name!"));
    if (permutation_theme.empty())
        throw std::invalid_argument(ERR_FORMAT("Permutating tile was given an empty theme!"));
    if (Tile_Map.contains(permutation_id))
    {
        std::string err("Permutating tile was given an id that is already in use!");
        err.append(std::format(" ID was{}", permutation_id));
        throw std::invalid_argument(ERR_FORMAT(err));
    }

    path = permutation_path;
    name = permutation_name;
    theme = permutation_theme;
    id = permutation_id;
    connections = permutation_connections;
    is_entrance_flag = permutation_is_entrance_flag;
    is_exit_flag = permutation_is_exit_flag;
    is_permutateable_flag = permutation_is_permutable_flag;
    is_flippable_flag = permutation_is_flippable_flag;
}

/**
 * @brief Maps the connection tokens used in the public constructor to bits in the connection bit map.
 * @param[in] connection_tokens Connection tokens read from the filename.
 * @throws std::invalid_argument on an empty vector.
 */
inline void D_Tile::map_connection_tokens(std::vector<std::string> connection_tokens)
{
    connections.mask = CONNECTION_INIT_MASK;
    // If we have NA, then the tile has no connections.
    for (std::string connection : connection_tokens)
        if (connection.compare(NA_CONNECTION_TOKEN))
            return;

    // Set connections with our str to bit map.
    for (auto con : connection_tokens)
    {
        if (Connection_Str_to_Bit_Mask_Map.contains(con))
            connections.mask |= Connection_Str_to_Bit_Mask_Map.at(con);
        else
        {
            std::string err("Tile has an invalid connection in its connection list!");
            err.append(to_string());
            throw std::invalid_argument(ERR_FORMAT(err));
        }
    }

    // If we have reached here and have no connections we've had an error in our input.
    std::string err("Tile has no valid connections in its connection list!");
    err.append(to_string());
    throw std::invalid_argument(ERR_FORMAT(err));
}

/**
 * @brief Creates permutations of the given D_Tile and places shared_ptr references in the global tile maps.
 * @param[in] permutable The D_Tile to permutate.
 * @returns A vector of shared_ptrs to the passed D_Tile's permutations, these will tiles will have already been loaded
 * into the global maps but are returned for easy iteration when generating their images.
 * @throws std::invalid_argument a nullptr.
 */
inline void D_Tile::permutate(std::shared_ptr<D_Tile> permutateable)
{
    //! TODO: This
}

/**
 * @brief Outputs a string to name a new generated tile.
 * @note This image filename is in the following format:
 * name;theme;connections,with,comma,separated,values;is_entrance;is_exit;is_permutateable;is_flippable
 * For example:
 * 3WayInter0;fort;T3,T4,R3,R4,B3,B4;false;false;true;false
 */
inline std::string const D_Tile::to_filename()
{
    std::stringstream ss;
    ss << name << ";" << theme << ";";

    // Build a vector of connection tokens to properly place commas or the ending semicolon.
    bool need_comma = false;
    std::vector<std::string> connection_tokens;
    connection_tokens.reserve(32);
    for (auto itr = Connection_Bit_Mask_to_Str_Map.begin(); itr != Connection_Bit_Mask_to_Str_Map.end(); itr++)
    {
        if (itr->first & connections.mask)
            connection_tokens.push_back(itr->second);
    }

    // Stream in the connections.
    for (size_t idx = 0; idx < connection_tokens.size(); idx++)
    {
        if (idx == connection_tokens.size() - 1) // Dont add a comma at the last connection, place a semicolon instead.
            ss << connection_tokens.at(idx) << ';';
        else
            ss << connection_tokens.at(idx) << ',';
    }

    // Stream the remaining attributes.
    is_entrance_flag ? ss << "true;" : ss << "false;";
    is_exit_flag ? ss << "true;" : ss << "false;";
    is_permutateable_flag ? ss << "true;" : ss << "false;";
    is_flippable_flag ? ss << "true" : ss << "false"; // Last part of the filename has no semicolon.

    return ss.str();
}

/**
 * @brief Creates an image for a D_Tile and saves it at the object's path member.
 */
void D_Tile::generate_tile_img()
{
    //! TODO: This
}
