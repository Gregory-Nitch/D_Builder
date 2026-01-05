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
#include <bit>
#include <format>

/*
========================================================================================================================
- - 3rd Party Includes - -
========================================================================================================================
*/

#include <QImage>
#include <QTransform>
#include <QString>

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
 * @brief Index of the tile connections in the token vector when constructing a tile.
 **********************************************************************************************************************/
#define TILE_CON_IDX (2)

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
 * all bits are set to zero.
 **********************************************************************************************************************/
#define CONNECTION_ZERO_MASK (0x0)

/***********************************************************************************************************************
 * @brief Expected string when parsing a tile that has no connections.
 **********************************************************************************************************************/
#define NA_CONNECTION_TOKEN "NA"

/***********************************************************************************************************************
 * @brief Number of connections that every tile will have on one side, used when rotating connections.
 **********************************************************************************************************************/
#define TILE_SIDE_CONNECTION_SIZE (8)

/***********************************************************************************************************************
 * @brief Global id counter for D_Tile objects.
 *
 * @note May be replaced later with id set by a database.
 **********************************************************************************************************************/
std::atomic<uint64_t> D_Tile::id_counter{0};

/*
========================================================================================================================
- - Class Methods - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Constructor for a Dungeon Tile.
 *
 * @param[in] in_path Path to the image file.
 *
 * @warning This image file name must be in the following format:
 * name;theme;connections,with,comma,separated,values;is_entrance;is_exit;is_permutateable;is_flippable
 * For example:
 * 3WayInter0;fort;T3,T4,R3,R4,B3,B4;false;false;true;false
 * Additionally, this image is expected to permutate, it should have top connections as it will be the base image that
 * all permutations will be made from.
 *
 * @note If there are no connections that section of the filename, it should have NA placed there.
 *
 * @throws std::invalid_argument if in_path filename is empty, if name member ends up empty, if theme member ends up
 * empty, or if it is labeled as both an entrance and an exit.
 **********************************************************************************************************************/
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

    std::stringstream connection_string_stream(file_tokens.at(TILE_CON_IDX));
    while (std::getline(connection_string_stream, connection_token, comma))
    {
        connection_tokens.push_back(connection_token);
    }

    // Remove '.jpg'
    [[maybe_unused]] std::stringstream err;
    size_t idx = file_tokens.at(TILE_FLIP_FLG_IDX).find_first_of('.');
    if (idx == std::string::npos)
    {
        err << "No file type in file path!";
        err << to_string();
        throw std::invalid_argument(ERR_FORMAT(err.str()));
    }
    file_tokens.at(TILE_FLIP_FLG_IDX) = file_tokens.at(TILE_FLIP_FLG_IDX).erase(idx);

    // Set members
    path = in_path;
    name = file_tokens.at(TILE_NAME_IDX);
    theme = file_tokens.at(TILE_THEME_IDX);
    id = id_counter.fetch_add(1);
    map_connection_tokens(connection_tokens);
    is_entrance_flag = file_tokens.at(TILE_ENT_FLG_IDX).compare("true") ? false : true;
    is_exit_flag = file_tokens.at(TILE_EXT_FLG_IDX).compare("true") ? false : true;
    is_permutateable_flag = file_tokens.at(TILE_PERM_FLG_IDX).compare("true") ? false : true;
    is_flippable_flag = file_tokens.at(TILE_FLIP_FLG_IDX).compare("true") ? false : true;

    if (name.empty())
    {
        err << "Tile name found to be empty at end of D_Tile()!:";
        err << to_string();
        throw std::invalid_argument(ERR_FORMAT(err.str()));
    }
    if (theme.empty())
    {
        err << "Tile theme found to be empty at end of D_Tile!:";
        err << to_string();
        throw std::invalid_argument(ERR_FORMAT(err.str()));
    }
    if (is_entrance() && is_exit())
    {
        err << "A tile cannot be both an entrance and an exit!:";
        err << to_string();
        throw std::invalid_argument(ERR_FORMAT(err.str()));
    }
    if (is_flippable() && !is_permutateable())
    {
        err << "A tile cannot be flippable and not be permutateable!:";
        err << to_string();
        throw std::invalid_argument(ERR_FORMAT(err.str()));
    }
}

/***********************************************************************************************************************
 * @brief Destructor.
 **********************************************************************************************************************/
D_Tile::~D_Tile()
{
    //! TODO: This?
}

/***********************************************************************************************************************
 * @brief Loads all the tiles from a given directory, places them in the global map and then loads the acutal images as
 * QImages for each tile.
 *
 * @param dir_path Directory path to a group of images to load.
 * @param loaded_path Directory path to move the loaded images too. Defaults to an empty path incase we have already
 * copied and generated tiles in the loaded directory.
 *
 * @note Does not generate permutations in the global maps, if that is required call generate_tiles. Doing so will also
 * create images for the application to use.
 **********************************************************************************************************************/
void D_Tile::load_tiles(std::filesystem::path const &dir_path, std::filesystem::path const &loaded_path)
{
    LOG_DEBUG("Loading Tiles...");
    if (dir_path.empty())
        std::invalid_argument(ERR_FORMAT("Given empty path to loading function!"));

    size_t tile_count = 0;
    std::vector<std::shared_ptr<D_Tile>> tiles;
    for (std::filesystem::directory_entry const dir_entry : std::filesystem::directory_iterator{dir_path})
    {
        if (dir_entry.is_directory())
            continue;
        tile_count++;
    }

    std::stringstream ss;
    ss << "Found ";
    ss << tile_count;
    ss << " input tiles.";
    LOG_DEBUG(ss.str());

    size_t entrance_count = 0;
    size_t exit_count = 0;
    Tile_Map.reserve(tile_count);
    tiles.reserve(tile_count);
    for (std::filesystem::directory_entry const dir_entry : std::filesystem::directory_iterator{dir_path})
    {
        if (dir_entry.is_directory())
            continue;
        std::shared_ptr<D_Tile> tile = std::make_shared<D_Tile>(dir_entry.path());
        if (!loaded_path.empty())
        {
            tile->copy_tile_img(loaded_path);
        }
        //! NOTE: We only load the tile image after we have ensured it is in the proper directory.
        tile->image = std::make_unique<QImage>(QString::fromStdString(tile->path.generic_string()));
        tiles.push_back(tile);

        if (tile->is_entrance())
            entrance_count++;
        if (tile->is_exit())
            exit_count++;
    }

    Entrance_Map.reserve(entrance_count);
    Exit_Map.reserve(exit_count);
    for (auto tile : tiles)
    {
        std::pair<uint64_t, std::shared_ptr<D_Tile>> tile_pair = {tile->id, tile};
        auto emplace_pair = Tile_Map.emplace(tile_pair);
        if (!emplace_pair.second)
            throw std::runtime_error(ERR_FORMAT("Failed placing a tile in the Tile_Map during loading!"));

        if (tile->is_entrance())
        {
            emplace_pair = Entrance_Map.emplace(tile_pair);
            if (!emplace_pair.second)
                throw std::runtime_error(ERR_FORMAT("Failed placing a tile in the Entrance_Map during loading!"));
        }

        if (tile->is_exit())
        {
            emplace_pair = Exit_Map.emplace(tile_pair);
            if (!emplace_pair.second)
                throw std::runtime_error(ERR_FORMAT("Failed placing a tile in the Exit_Map during loading!"));
        }

        LOG_DEBUG(std::format("{}:{}", "Loaded Tile", tile->to_string()));
    }
}

/***********************************************************************************************************************
 * @brief Generates tiles from the D_Tiles loaded in load_tiles(), this will also create permutation images of
 * permutable tiles and save them.
 *
 * @note Generates permutations in the global maps.
 *
 * @throws std::runtime_error if it encoutners a nullptr in the Tile_Map.
 **********************************************************************************************************************/
void D_Tile::generate_tiles()
{
    LOG_DEBUG("Generating Tiles...");

    size_t entrance_count = 0;
    size_t exit_count = 0;
    std::vector<std::shared_ptr<D_Tile>> permutations;

    // For each tile in global check for permutables
    for (auto pair : Tile_Map)
    {
        std::shared_ptr<D_Tile> tile = pair.second;
        if (nullptr != tile && tile->is_permutateable())
        {
            permutate(tile, permutations, entrance_count, exit_count);
        }
        else if (nullptr == tile)
            throw std::runtime_error(ERR_FORMAT("Found nullptr in tile global map!"));
    }

    // Generate images and load them to the global map.
    Tile_Map.reserve(Tile_Map.size() + permutations.size());
    Entrance_Map.reserve(Entrance_Map.size() + entrance_count);
    Exit_Map.reserve(Exit_Map.size() + exit_count);
    std::stringstream err;
    err << "Tile Map size:" << Tile_Map.size() << " Permutations size:" << permutations.size() << " Entrance Map size:"
        << Entrance_Map.size() << " Entrance count:" << entrance_count << " Exit Map size:" << Exit_Map.size()
        << " Exit count:" << exit_count << " [Tile]:";
    for (auto tile : permutations)
    {
        //! TODO: vvv (generate_tile_img()) Move image gen out of the loop and use multiple threads for faster processing?
        tile->generate_tile_img();
        std::pair<uint64_t, std::shared_ptr<D_Tile>> tile_pair = {tile->id, tile};
        auto emplace_pair = Tile_Map.emplace(tile_pair);
        if (!emplace_pair.second)
        {
            err << tile->to_string() << ":Failed placing a permutation in the Tile_Map during permutation!";
            throw std::runtime_error(ERR_FORMAT(err.str()));
        }

        if (tile->is_entrance())
        {
            emplace_pair = Entrance_Map.emplace(tile_pair);
            if (!emplace_pair.second)
            {
                err << tile->to_string() << ":Failed placing a permutation in the Entrance_Map during permutation!";
                throw std::runtime_error(ERR_FORMAT(err.str()));
            }
        }
        else if (tile->is_exit())
        {
            emplace_pair = Exit_Map.emplace(tile_pair);
            if (!emplace_pair.second)
            {
                err << tile->to_string() << ":Failed placing a permutation in the Exit_Map during permutation!";
                throw std::runtime_error(ERR_FORMAT(err.str()));
            }
        }

        LOG_DEBUG(std::format("{}:{}", "Permutated Tile:", tile->to_string()));
    }
}

/***********************************************************************************************************************
 * @brief Gets the name of the tile.
 *
 * @returns Name of the tile.
 **********************************************************************************************************************/
std::string const &D_Tile::get_name() const
{
    return name;
}

/***********************************************************************************************************************
 * @brief Gets the theme of the tile.
 *
 * @returns Theme of the tile.
 **********************************************************************************************************************/
std::string const &D_Tile::get_theme() const
{
    return theme;
}

/***********************************************************************************************************************
 * @brief Gets the ID of the tile.
 *
 * @returns ID of the tile.
 **********************************************************************************************************************/
uint64_t const D_Tile::get_id() const
{
    return id;
}

/***********************************************************************************************************************
 * @brief Gets the connection bit map of the tile.
 *
 * @returns Connection bit map of the tile.
 **********************************************************************************************************************/
D_Connections const D_Tile::get_connections() const
{
    return connections;
}

/***********************************************************************************************************************
 * @brief Gets the permutable flag of the tile.
 *
 * @returns Whether or not the tile is permutable of the tile.
 *
 * @note Tiles that were made from permutable tiles are not permutable.
 **********************************************************************************************************************/
bool const D_Tile::is_permutateable() const
{
    return is_permutateable_flag;
}

/***********************************************************************************************************************
 * @brief Gets the entrance flag of the tile.
 *
 * @returns Whether or not the tile is an entrance tile.
 **********************************************************************************************************************/
bool const D_Tile::is_entrance() const
{
    return is_entrance_flag;
}

/***********************************************************************************************************************
 * @brief Gets the exit flag of the tile.
 *
 * @returns Whether or not the tile is an exit tile.
 **********************************************************************************************************************/
bool const D_Tile::is_exit() const
{
    return is_exit_flag;
}

/***********************************************************************************************************************
 * @brief Gets the flippable flag of the tile.
 *
 * @returns Whether or not the tile is flippable.
 *
 * @note Tiles made from flippable tiles are not set as flippable themselves after their creation.
 **********************************************************************************************************************/
bool const D_Tile::is_flippable() const
{
    return is_flippable_flag;
}

/***********************************************************************************************************************
 * @brief Gets the flipped status of the tile, if the tile was flipped during permutation this will return true.
 *
 * @retval bool Whether or not the tile has been flipped.
 **********************************************************************************************************************/
bool const D_Tile::is_flipped() const
{
    return is_flipped_flag;
}

/***********************************************************************************************************************
 * @brief Gets the amount of rotation used to produce the permutated image. Original images will be set to Zero.
 *
 * @retval Connection_Rotations enum value to the amount the image needed to be rotated to produce the permutation.
 **********************************************************************************************************************/
Connection_Rotations const D_Tile::get_rotation_amount() const
{
    return rotation_amount;
}

/***********************************************************************************************************************
 * @brief Outputs the tile information string form.
 *
 * @returns The tile information as a string in the form of:
 * ID:<id>,Name:<name>,Theme:<theme>,Connections:<connections,>,Entrance:<flag>,Exit:<flag>,Permutable:<flag>,Flippable:<flag>
 **********************************************************************************************************************/
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
    is_entrance() ? ss << "is entrance" : ss << "not entrance";
    ss << ",Exit:";
    is_exit() ? ss << "is exit" : ss << "not exit";
    ss << ",Permutable:";
    is_permutateable() ? ss << "is permutable" : ss << "not permutable";
    ss << ",Flippable:";
    is_flippable() ? ss << "is flippable" : ss << "not flippable";
    ss << ",Flipped Tile:";
    is_flipped() ? ss << "is flipped" : ss << "is not flipped";
    ss << "Rotation:" << static_cast<uint8_t>(get_rotation_amount());

    return ss.str();
}

/*
========================================================================================================================
- - Private Methods - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @brief Private constructor to build new D_Tile objects from permutaion values.
 **********************************************************************************************************************/
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

/***********************************************************************************************************************
 * @brief Maps the connection tokens used in the public constructor to bits in the connection bit map.
 *
 * @param[in] connection_tokens Connection tokens read from the filename.
 *
 * @throws std::invalid_argument on an empty vector.
 **********************************************************************************************************************/
inline void D_Tile::map_connection_tokens(std::vector<std::string> connection_tokens)
{
    connections.mask = CONNECTION_ZERO_MASK;

    // Set connections with our str to bit map.
    for (auto con : connection_tokens)
    {
        if (Connection_Str_to_Bit_Mask_Map.contains(con))
            connections.mask |= Connection_Str_to_Bit_Mask_Map.at(con);
        else if (!con.compare(NA_CONNECTION_TOKEN))
        {
            connections.mask = CONNECTION_ZERO_MASK; // Double zero to be sure. Why not?
            return;                                  // If we have NA, then the tile has no connections.
        }
        else
        {
            std::stringstream err;
            err << "Tile has an invalid connection in its connection list![Tile]:";
            err << to_string();
            err << "[Connection]:" << con;
            throw std::invalid_argument(ERR_FORMAT(err.str()));
        }
    }

    // If we have reached here and have no connections we've had an error.
    if (!connections.mask)
    {
        std::string err("Tile has no valid connections in its connection list!:[Tile]:");
        err.append(to_string());
        throw std::invalid_argument(ERR_FORMAT(err));
    }
}

/***********************************************************************************************************************
 * @brief Creates permutations of the given D_Tile and places shared_ptr references in the global tile maps.
 *
 * @param[in] permutable The D_Tile to permutate.
 * @param[out] permutations Vector to place permutations in.
 * @param[out] entrance_count Number of current entrances being made during tile generation.
 * @param[out] exit_count Number of current exits being made during tile generation.
 *
 * @throws std::invalid_argument a nullptr.
 **********************************************************************************************************************/
inline void D_Tile::permutate(std::shared_ptr<D_Tile> permutateable,
                              std::vector<std::shared_ptr<D_Tile>> &permutations,
                              size_t &entrance_count,
                              size_t &exit_count)
{ /*! TODO:We may be able to move tile rotation processing into another function for refactoring.*/
    if (nullptr == permutateable)
        throw std::invalid_argument(ERR_FORMAT("Encountered a nullptr while trying to permutate a tile!"));

    size_t permutation_limiter = ROTATION_ARR.size();
    if (permutateable->connections.mask == rotate_connections(Connection_Rotations::One_Eighty, permutateable->connections).mask)
    {
        permutation_limiter = ROTATION_ARR.size() - 2;
    }

    std::stringstream ss;
    ss << "Permutating Tile:" << permutateable->to_string();
    LOG_DEBUG(ss.str());

    if (permutateable->is_flippable())
    {
        permutations.reserve(permutations.size() + MAX_FLIPPABLE_PERMUTATIONS);
        if (permutateable->is_entrance())
            entrance_count += MAX_FLIPPABLE_PERMUTATIONS;
        if (permutateable->is_exit())
            exit_count += MAX_FLIPPABLE_PERMUTATIONS;
    }
    else
    {
        permutations.reserve(permutations.size() + MAX_PERMUTATIONS);
        if (permutateable->is_entrance())
            entrance_count += MAX_PERMUTATIONS;
        if (permutateable->is_exit())
            exit_count += MAX_PERMUTATIONS;
    }

    // Create new tiles
    for (size_t idx = 0; idx < permutation_limiter; idx++)
    {
        D_Connections rotated_connections = rotate_connections(ROTATION_ARR[idx], permutateable->connections);
        std::shared_ptr<D_Tile> tile(new D_Tile(
            permutateable->name,
            permutateable->theme,
            id_counter.fetch_add(1),
            rotated_connections,
            permutateable->is_entrance(),
            permutateable->is_exit(),
            false, // Permutations are not permutatable,
            false  // nor are they flippable.
            ));

        tile->rotation_amount = ROTATION_ARR[idx];
        std::string filename = tile->to_filename();
        tile->path = std::filesystem::path(std::format("{}/{}", permutateable->path.parent_path().generic_string(), filename));
        tile->image = std::make_unique<QImage>(*permutateable->image);
        permutations.push_back(tile);
    }

    // If we need to flip and continue do so
    if (permutateable->is_flippable())
    {
        // Flip once
        D_Connections flipped_connections = flip_connections(permutateable->connections);
        std::shared_ptr<D_Tile> flipped(new D_Tile(
            permutateable->name,
            permutateable->theme,
            id_counter.fetch_add(1),
            flipped_connections,
            permutateable->is_entrance(),
            permutateable->is_exit(),
            false, // Permutations are not permutable
            false  //  nor are they flippable.
            ));

        flipped->is_flipped_flag = true;
        std::string filename = flipped->to_filename();
        flipped->path = std::filesystem::path(std::format("{}/{}", permutateable->path.parent_path().generic_string(), filename));
        flipped->image = std::make_unique<QImage>(*permutateable->image);
        permutations.push_back(flipped);

        // And rotate
        for (size_t idx = 0; idx < permutation_limiter; idx++)
        {
            D_Connections rotated_connections = rotate_connections(ROTATION_ARR[idx], flipped->connections);
            std::shared_ptr<D_Tile> tile(new D_Tile(
                permutateable->name,
                permutateable->theme,
                id_counter.fetch_add(1),
                rotated_connections,
                permutateable->is_entrance(),
                permutateable->is_exit(),
                false, // Permutations are not permutable
                false  //  nor are they flippable.
                ));

            tile->is_flipped_flag = true;
            tile->rotation_amount = ROTATION_ARR[idx];
            std::string filename = tile->to_filename();
            tile->path = std::filesystem::path(std::format("{}/{}", permutateable->path.parent_path().generic_string(), filename));
            tile->image = std::make_unique<QImage>(*permutateable->image);
            permutations.push_back(tile);
        }
    }
}

/***********************************************************************************************************************
 * @brief Outputs a string to name a new generated tile.
 *
 * @note This image filename is in the following format:
 * name;theme;connections,with,comma,separated,values;is_entrance;is_exit;is_permutateable;is_flippable
 * For example:
 * 3WayInter0;fort;T3,T4,R3,R4,B3,B4;false;false;true;false
 **********************************************************************************************************************/
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
    is_entrance() ? ss << "true;" : ss << "false;";
    is_exit() ? ss << "true;" : ss << "false;";
    is_permutateable() ? ss << "true;" : ss << "false;";
    is_flippable() ? ss << "true" : ss << "false"; // Last part of the filename has no semicolon.

    ss << ".jpg";

    return ss.str();
}

/***********************************************************************************************************************
 * @brief Creates an image for a D_Tile and saves it at the instances's path member.
 **********************************************************************************************************************/
void D_Tile::generate_tile_img()
{
    if (!image || image->isNull())
        throw std::invalid_argument(ERR_FORMAT("Null image reference found when generating a tile image!"));

    QTransform matrix;
    size_t degrees = 0;

    if (is_flipped())
        image->flip(Qt::Horizontal);

    switch (rotation_amount)
    {
    case Connection_Rotations::Nintey:
        degrees = 90;
        break;

    case Connection_Rotations::One_Eighty:
        degrees = 180;
        break;

    case Connection_Rotations::Two_Seventy:
        degrees = 270;
        break;

    default: // No Rotation required.
        break;
    }

    if (0 != degrees)
    {
        matrix.rotate(degrees);
        *image = image->transformed(matrix);
    }

    image->save(QString::fromStdString(path.generic_string()));
}

/***********************************************************************************************************************
 * @brief Copies an image for a D_Tile from the image's path to the passed directory, this then udpates the tiles path
 * member.
 *
 * @param[in] loaded_dir Directory to set as the new path member and move the tile image too.
 **********************************************************************************************************************/
void D_Tile::copy_tile_img(std::filesystem::path loaded_dir)
{
    std::filesystem::path new_path = loaded_dir / path.filename();

    try
    {
        std::filesystem::copy(path, new_path, std::filesystem::copy_options::overwrite_existing);
    }
    catch (std::filesystem::filesystem_error &fs_e)
    {
        std::stringstream err;
        err << "Unable to copy tile image. Filesystem error was: ";
        err << fs_e.what();
        throw std::runtime_error(ERR_FORMAT(err.str()));
    }

    path = new_path;
}

/***********************************************************************************************************************
 * @brief Rotates connections for a given connection bitmap according to a rotation enum value.
 *
 * @param[in] rotation Connection_Rotations enum denoting the amount that the connections should be rotated.
 * @param[in] to_rotate The connections to rotate.
 *
 * @retval D_Connections The connections after rotated.
 **********************************************************************************************************************/
inline D_Connections D_Tile::rotate_connections(Connection_Rotations rotation, D_Connections to_rotate)
{
    return {.mask = std::rotl(to_rotate.mask, (static_cast<uint8_t>(rotation) * TILE_SIDE_CONNECTION_SIZE))};
}

/***********************************************************************************************************************
 * @brief Flips connections for a given D_Connection union.
 *
 * @param[in] to_flip Connections to flip.
 *
 * @retval D_Connections The passed connections flipped horizontally, ie left becomes right and right becomes left, top
 * and bottom are mirrored.
 **********************************************************************************************************************/
inline D_Connections D_Tile::flip_connections(D_Connections to_flip)
{
    // Swap left and right masks
    D_Connections out = {.mask = CONNECTION_ZERO_MASK};
    out.side_masks.left = to_flip.side_masks.right;
    out.side_masks.right = to_flip.side_masks.left;

    // Top and Bottom masks need to be reversed, extract the bit with i and shift by j.
    for (size_t i = 0, j = UINT8_WIDTH - 1; i < UINT8_WIDTH; i++, j--)
    {
        if (UINT8_WIDTH < j)
            throw std::runtime_error(ERR_FORMAT("While flipping connections we were going to write outside of the uint8_t bit width!"));

        out.side_masks.top |= ((to_flip.side_masks.top >> i) & 1u) << j;
        out.side_masks.bottom |= ((to_flip.side_masks.bottom >> i) & 1u) << j;
    }

    return out;
}
