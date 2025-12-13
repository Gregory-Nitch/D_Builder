
#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <exception>

#include "d_tile.hpp"
#include "d_builder_common.hpp"

#define MAX_PERMUTATIONS (3)
#define MAX_FLIPPABLE_PERMUTATIONS (7)
#define TILE_CONNECTION_MAX (32)
#define FILE_NAME_TOKEN_NUM (7)
#define TILE_NAME_IDX (0)
#define TILE_THEME_IDX (1)
#define TILE_ENT_FLG_IDX (3)
#define TILE_EXT_FLG_IDX (4)
#define TILE_PERM_FLG_IDX (5)
#define TILE_FLIP_FLG_IDX (6)
#define CONNECTION_INIT_MASK (0x0)

/**
 * @brief Constructor for a Dungeon Tile.
 * @param[in] path Path to the image file.
 * @note This image file name must be in the following format:
 * name;theme;connections,with,comma,separated,values;is_entrance;is_exit;is_permutateable;is_flippable
 * For example:
 * 3WayInter0;fort;T3,T4,R3,R4,B3,B4;false;false;true;false
 * Additionally, this image is expected to permutate, it should have top connections as it will be the base image that
 * all permutations will be made from.
 */
D_Tile::D_Tile(std::filesystem::path const &path)
{
    std::vector<std::string> file_tokens;
    std::string file_token;
    std::vector<std::string> connection_tokens;
    std::string connection_token;
    std::stringstream file_str_stream(path.filename().generic_string());
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
    name = file_tokens.at(TILE_NAME_IDX);
    theme = file_tokens.at(TILE_THEME_IDX);
    id = id_counter.fetch_add(1);
    map_connection_tokens(connection_tokens);
    is_entrance_flag = file_tokens.at(TILE_ENT_FLG_IDX).compare("true") ? true : false;
    is_exit_flag = file_tokens.at(TILE_EXT_FLG_IDX).compare("true") ? true : false;
    is_permutateable_flag = file_tokens.at(TILE_PERM_FLG_IDX).compare("true") ? true : false;
    is_flippable_flag = file_tokens.at(TILE_FLIP_FLG_IDX).compare("true") ? true : false;
}

D_Tile::~D_Tile()
{
    //! TODO: This
}

void D_Tile::load_tiles(std::filesystem::directory_entry const &dir_path)
{
    //! TODO: This
}

std::string const &D_Tile::get_name() const
{
    return name;
}

std::string const &D_Tile::get_theme() const
{
    return theme;
}

uint64_t const D_Tile::get_id() const
{
    return id;
}

D_Connections const D_Tile::get_connections() const
{
    return connections;
}

bool const D_Tile::is_permutateable() const
{
    return is_permutateable_flag;
}

bool const D_Tile::is_entrance() const
{
    return is_entrance_flag;
}

bool const D_Tile::is_exit() const
{
    return is_exit_flag;
}

bool const D_Tile::is_flippable() const
{
    return is_flippable_flag;
}

std::string const D_Tile::to_string() const
{
    std::stringstream ss;
    ss << "ID:" << id << ",Name:" << name << ",Theme:" << theme;

    for (std::pair<uint32_t, std::string> pair : Connection_Bit_Mask_to_Str_Map)
    {
        if (connections.mask & pair.first)
            ss << Connection_Bit_Mask_to_Str_Map.at(pair.first) << ",";
    }

    ss << ",Entrance Flag:";
    is_entrance_flag ? ss << "is entrance" : ss << "not entrance";
    ss << ",Exit Flag:";
    is_exit_flag ? ss << "is exit" : ss << "not exit";
    ss << ",Permutable Flag:";
    is_permutateable_flag ? ss << "is permutable" : ss << "not permutable";
    ss << ",Flippable Flag:";
    is_flippable_flag ? ss << "is flippable" : ss << "not flippable";

    return ss.str();
}

/**
 * @brief Private constructor to build new D_Tile objects from permutaion values.
 */
D_Tile::D_Tile(std::string permutation_name,
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

    name = permutation_name;
    theme = permutation_theme;
    id = permutation_id;
    connections = permutation_connections;
    is_entrance_flag = permutation_is_entrance_flag;
    is_exit_flag = permutation_is_exit_flag;
    is_permutateable_flag = permutation_is_permutable_flag;
    is_flippable_flag = permutation_is_flippable_flag;
}

inline void D_Tile::map_connection_tokens(std::vector<std::string> connection_tokens)
{
    connections.mask = CONNECTION_INIT_MASK;

    // Set connections with our str to bit map, expect to skip this with an empty token vector.
    for (auto con : connection_tokens)
    {
        if (Connection_Str_to_Bit_Mask_Map.contains(con))
            connections.mask &= Connection_Str_to_Bit_Mask_Map.at(con);
        else
        {
            std::string err("Tile has an invalid connection in its connection list!");
            err.append(to_string());
            throw std::invalid_argument(ERR_FORMAT(err));
        }
    }
}

inline std::vector<std::shared_ptr<D_Tile>> D_Tile::permutate(std::shared_ptr<D_Tile> permutateable)
{
    //! TODO: This
}
