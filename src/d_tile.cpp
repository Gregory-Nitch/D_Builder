/***********************************************************************************************************************
 * @date : 2025-12-14
 * @author : Gregory Nitch
 *
 * @brief : D_Tile implementation functions. This class represents a section that can be used to construct the D_Map
 * object's display matrix.
 **********************************************************************************************************************/

/*
************************************************************************************************************************
- - Sysytem Includes - -
************************************************************************************************************************
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
************************************************************************************************************************
- - 3rd Party Includes - -
************************************************************************************************************************
*/

#include <QImage>
#include <QTransform>
#include <QString>

/*
************************************************************************************************************************
- - Local Includes - -
************************************************************************************************************************
*/

#include "d_tile.hpp"
#include "d_builder_common.hpp"

/*
************************************************************************************************************************
- - Constants / Macros - -
************************************************************************************************************************
*/

/***********************************************************************************************************************
 * @brief Max possible permutations of a tile that is not flippable.
 **********************************************************************************************************************/
constexpr uint8_t MAX_PERMUTATIONS = 3;

/***********************************************************************************************************************
 * @brief Max possible permutations of a tile that is flippable.
 **********************************************************************************************************************/
constexpr uint8_t MAX_FLIPPABLE_PERMUTATIONS = 7;

/***********************************************************************************************************************
 * @brief Max possible number of connections a tile can have.
 **********************************************************************************************************************/
constexpr uint8_t TILE_CONNECTION_MAX = 32;

/***********************************************************************************************************************
 * @brief Max possible tokens a tile filename should have.
 **********************************************************************************************************************/
constexpr uint8_t FILE_NAME_TOKEN_NUM = 7;

/***********************************************************************************************************************
 * @brief Index of the tile name in the token vector when constructing a tile.
 **********************************************************************************************************************/
constexpr uint8_t TILE_NAME_IDX = 0;

/***********************************************************************************************************************
 * @brief Index of the tile theme in the token vector when constructing a tile.
 **********************************************************************************************************************/
constexpr uint8_t TILE_THEME_IDX = 1;

/***********************************************************************************************************************
 * @brief Index of the tile connections in the token vector when constructing a tile.
 **********************************************************************************************************************/
constexpr uint8_t TILE_CON_IDX = 2;

/***********************************************************************************************************************
 * @brief Index of the tile's entrance flag in the token vector when constructing a tile.
 **********************************************************************************************************************/
constexpr uint8_t TILE_ENT_FLG_IDX = 3;

/***********************************************************************************************************************
 * @brief Index of the tile's exit flag in the token vector when constructing a tile.
 **********************************************************************************************************************/
constexpr uint8_t TILE_EXT_FLG_IDX = 4;

/***********************************************************************************************************************
 * @brief Index of the tile's permutable flag in the token vector when constructing a tile.
 **********************************************************************************************************************/
constexpr uint8_t TILE_PERM_FLG_IDX = 5;

/***********************************************************************************************************************
 * @brief Index of the tile's flippable flag in the token vector when constructing a tile.
 **********************************************************************************************************************/
constexpr uint8_t TILE_FLIP_FLG_IDX = 6;

/***********************************************************************************************************************
 * @brief Expected string when parsing a tile that has no connections.
 **********************************************************************************************************************/
constexpr char const *NA_CONNECTION_TOKEN = "NA";

/***********************************************************************************************************************
 * @brief Number of connections that every tile will have on one side, used when rotating connections.
 **********************************************************************************************************************/
constexpr uint8_t TILE_SIDE_CONNECTION_SIZE = 8;

/***********************************************************************************************************************
 * @brief Global id counter for D_Tile objects.
 *
 * @note May be replaced later with id set by a database.
 **********************************************************************************************************************/
std::atomic<uint64_t> D_Tile::id_counter{0};

D_Tile::D_Tile(std::filesystem::path const &in_path)
{
    if (in_path.filename().generic_string().empty())
    {
        Logger.log(libcpp59::log_level::ERR, "Empty filename in path given to D_Tile()!");
        throw std::invalid_argument("Empty filename in path given to D_Tile()!");
    }

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
        Logger.log(libcpp59::log_level::ERR, err.str());
        throw std::invalid_argument("No file type in file path!");
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
        Logger.log(libcpp59::log_level::ERR, err.str());
        throw std::invalid_argument("Tile name found to be empty at end of D_Tile()!");
    }
    if (theme.empty())
    {
        err << "Tile theme found to be empty at end of D_Tile!:";
        err << to_string();
        Logger.log(libcpp59::log_level::ERR, err.str());
        throw std::invalid_argument("Tile theme found to be empty at end of D_Tile()!");
    }
    if (is_entrance() && is_exit())
    {
        err << "A tile cannot be both an entrance and an exit!:";
        err << to_string();
        Logger.log(libcpp59::log_level::ERR, err.str());
        throw std::invalid_argument("A tile cannot be both an entrance and an exit!");
    }
    if (is_flippable() && !is_permutateable())
    {
        err << "A tile cannot be flippable and not be permutateable!:";
        err << to_string();
        Logger.log(libcpp59::log_level::ERR, err.str());
        throw std::invalid_argument("A tile cannot be flippable and not be permutateable!");
    }
}

D_Tile::~D_Tile()
{
    //! TODO: This?
}

void D_Tile::load_tiles(std::filesystem::path const &dir_path, Progress_Callback const &progress_callback)
{
    Logger.log(libcpp59::log_level::INFO, "Loading Tiles...");
    size_t total_tile_count = 0;
    for (auto &&dir_set : Loaded_Img_Dirs)
    {
        std::filesystem::path const sub_dir_path = std::filesystem::path(dir_path) / dir_set.first;
        for (std::filesystem::directory_entry const &dir_entry : std::filesystem::directory_iterator{sub_dir_path})
        {
            if (!dir_entry.is_directory())
                total_tile_count++;
        }
    }

    size_t loaded_tile_count = 0;
    if (progress_callback)
        progress_callback("Loading tiles", loaded_tile_count, total_tile_count);

    for (auto &&dir_set : Loaded_Img_Dirs)
    {
        std::filesystem::path const sub_dir_path = std::filesystem::path(dir_path) / dir_set.first;
        std::filesystem::path const loaded_path = dir_set.second;
        if (sub_dir_path.empty())
        {
            Logger.log(libcpp59::log_level::ERR, "Given empty path to loading function!");
            throw std::invalid_argument("Given empty path to loading function!");
        }

        size_t tile_count = 0;
        std::vector<std::shared_ptr<D_Tile>> tiles;
        for (std::filesystem::directory_entry const &dir_entry : std::filesystem::directory_iterator{sub_dir_path})
        {
            if (dir_entry.is_directory())
                continue;
            tile_count++;
        }

        std::stringstream ss;
        ss << "Found ";
        ss << tile_count;
        ss << " input tiles.";
        Logger.log(libcpp59::log_level::DEBUG, ss.str());

        size_t entrance_count = 0;
        size_t exit_count = 0;
        Tile_Map.reserve(tile_count);
        tiles.reserve(tile_count);
        for (std::filesystem::directory_entry const &dir_entry : std::filesystem::directory_iterator{sub_dir_path})
        {
            if (dir_entry.is_directory())
                continue;
            std::shared_ptr<D_Tile> tile = std::make_shared<D_Tile>(dir_entry.path());
            if (!loaded_path.empty())
            {
                tile->copy_tile_img(loaded_path);
            }
            //! NOTE: We only load the tile image after we have ensured it is in the proper directory.
            tile->image = std::make_shared<QImage>(QString::fromStdString(tile->path.generic_string()));
            tiles.push_back(tile);
            loaded_tile_count++;
            if (progress_callback)
                progress_callback("Loading tiles", loaded_tile_count, total_tile_count);

            if (tile->is_entrance())
                entrance_count++;
            if (tile->is_exit())
                exit_count++;
            if (!tile->get_connections().mask)
                Empty_Tile = tile;
        }

        Entrance_Map.reserve(entrance_count);
        Exit_Map.reserve(exit_count);
        for (auto tile : tiles)
        {
            std::pair<uint64_t, std::shared_ptr<D_Tile>> tile_pair = {tile->id, tile};
            auto emplace_pair = Tile_Map.emplace(tile_pair);
            if (!emplace_pair.second)
            {
                Logger.log(libcpp59::log_level::ERR, "Failed placing a tile in the Tile_Map during loading!");
                throw std::runtime_error("Failed placing a tile in the Tile_Map during loading!");
            }

            if (tile->is_entrance())
            {
                emplace_pair = Entrance_Map.emplace(tile_pair);
                if (!emplace_pair.second)
                {
                    Logger.log(libcpp59::log_level::ERR, "Failed placing a tile in the Entrance_Map during loading!");
                    throw std::runtime_error("Failed placing a tile in the Entrance_Map during loading!");
                }
            }

            if (tile->is_exit())
            {
                emplace_pair = Exit_Map.emplace(tile_pair);
                if (!emplace_pair.second)
                {
                    Logger.log(libcpp59::log_level::ERR, "Failed placing a tile in the Exit_Map during loading!");
                    throw std::runtime_error("Failed placing a tile in the Exit_Map during loading!");
                }
            }

            Logger.log(libcpp59::log_level::DEBUG, std::format("Loaded Tile:{}", tile->to_string()));
        }
    }
}

void D_Tile::generate_tiles(Progress_Callback const &progress_callback)
{
    Logger.log(libcpp59::log_level::INFO, "Generating Tiles...");

    size_t entrance_count = 0;
    size_t exit_count = 0;
    std::vector<std::shared_ptr<D_Tile>> permutations;
    size_t processed_tile_count = 0;
    size_t const source_tile_count = Tile_Map.size();

    if (progress_callback)
        progress_callback("Preparing tile permutations", processed_tile_count, source_tile_count);

    // For each tile in global check for permutables
    for (auto pair : Tile_Map)
    {
        std::shared_ptr<D_Tile> tile = pair.second;
        if (nullptr != tile && tile->is_permutateable())
        {
            permutate(tile, permutations, entrance_count, exit_count);
        }
        else if (nullptr == tile)
        {
            Logger.log(libcpp59::log_level::ERR, "Found nullptr in tile global map!");
            throw std::runtime_error("Found nullptr in tile global map!");
        }

        processed_tile_count++;
        if (progress_callback)
            progress_callback("Preparing tile permutations", processed_tile_count, source_tile_count);
    }

    // Generate images and load them to the global map.
    Tile_Map.reserve(Tile_Map.size() + permutations.size());
    Entrance_Map.reserve(Entrance_Map.size() + entrance_count);
    Exit_Map.reserve(Exit_Map.size() + exit_count);
    std::stringstream err;
    err << "Tile Map size:" << Tile_Map.size() << " Permutations size:" << permutations.size() << " Entrance Map size:"
        << Entrance_Map.size() << " Entrance count:" << entrance_count << " Exit Map size:" << Exit_Map.size()
        << " Exit count:" << exit_count << " [Tile]:";
    size_t generated_tile_count = 0;
    if (progress_callback)
        progress_callback("Writing generated tile images", generated_tile_count, permutations.size());

    for (auto tile : permutations)
    {
        //! IMPROVEMENT: vvv (generate_tile_img()) Move image gen out of the loop and use multiple threads for faster processing?
        tile->generate_tile_img();
        std::pair<uint64_t, std::shared_ptr<D_Tile>> tile_pair = {tile->id, tile};
        auto emplace_pair = Tile_Map.emplace(tile_pair);
        if (!emplace_pair.second)
        {
            err << tile->to_string() << ":Failed placing a permutation in the Tile_Map during permutation!";
            Logger.log(libcpp59::log_level::ERR, err.str());
            throw std::runtime_error(err.str());
        }

        if (tile->is_entrance())
        {
            emplace_pair = Entrance_Map.emplace(tile_pair);
            if (!emplace_pair.second)
            {
                err << tile->to_string() << ":Failed placing a permutation in the Entrance_Map during permutation!";
                Logger.log(libcpp59::log_level::ERR, err.str());
                throw std::runtime_error(err.str());
            }
        }
        else if (tile->is_exit())
        {
            emplace_pair = Exit_Map.emplace(tile_pair);
            if (!emplace_pair.second)
            {
                err << tile->to_string() << ":Failed placing a permutation in the Exit_Map during permutation!";
                Logger.log(libcpp59::log_level::ERR, err.str());
                throw std::runtime_error(err.str());
            }
        }

        Logger.log(libcpp59::log_level::DEBUG, std::format("Permutated Tile:{}", tile->to_string()));
        generated_tile_count++;
        if (progress_callback)
            progress_callback("Writing generated tile images", generated_tile_count, permutations.size());
    }
}

std::string const &D_Tile::get_name() const
{
    return name;
}

std::string const &D_Tile::get_theme() const
{
    return theme;
}

uint64_t D_Tile::get_id() const
{
    return id;
}

D_Connections D_Tile::get_connections() const
{
    return connections;
}

std::shared_ptr<QImage> const &D_Tile::get_image()
{
    return image;
}

bool D_Tile::is_permutateable() const
{
    return is_permutateable_flag;
}

bool D_Tile::is_entrance() const
{
    return is_entrance_flag;
}

bool D_Tile::is_exit() const
{
    return is_exit_flag;
}

bool D_Tile::is_flippable() const
{
    return is_flippable_flag;
}

bool D_Tile::is_flipped() const
{
    return is_flipped_flag;
}

Connection_Rotations D_Tile::get_rotation_amount() const
{
    return rotation_amount;
}

std::string const D_Tile::to_string() const
{
    std::stringstream ss;
    ss << "ID:" << id << ",Name:" << name << ",Theme:" << theme << ",Connections:";
    ss << connections_to_string();
    ss << ",Connections as Mask:" << connections.mask;
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
    ss << "Rotation:" << static_cast<int>(get_rotation_amount());

    return ss.str();
}

std::string const D_Tile::connections_to_string() const
{
    std::stringstream ss;

    for (std::pair<uint32_t, std::string> pair : Connection_Bit_Mask_to_Str_Map)
    {
        if (connections.mask & pair.first)
            ss << Connection_Bit_Mask_to_Str_Map.at(pair.first) << ",";
    }

    return ss.str();
}

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
    {
        Logger.log(libcpp59::log_level::ERR, "Permutating tile was given an empty name!");
        throw std::invalid_argument("Permutating tile was given an empty name!");
    }
    if (permutation_theme.empty())
    {
        Logger.log(libcpp59::log_level::ERR, "Permutating tile was given an empty theme!");
        throw std::invalid_argument("Permutating tile was given an empty theme!");
    }
    if (Tile_Map.contains(permutation_id))
    {
        Logger.log(libcpp59::log_level::ERR, std::format("Permutating tile was given an id that is already in use! ID was{}", permutation_id));
        throw std::invalid_argument("Permutating tile was given an id that is already in use!");
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
            Logger.log(libcpp59::log_level::ERR, err.str());
            throw std::invalid_argument("Tile has an invalid connection in its connection list!");
        }
    }

    // If we have reached here and have no connections we've had an error.
    if (!connections.mask)
    {
        Logger.log(libcpp59::log_level::ERR, std::format("Tile has no valid connections in its connection list!:[Tile]:{}", to_string()));
        throw std::invalid_argument("Tile has no valid connections in its connection list!");
    }
}

inline void D_Tile::permutate(std::shared_ptr<D_Tile> permutateable,
                              std::vector<std::shared_ptr<D_Tile>> &permutations,
                              size_t &entrance_count,
                              size_t &exit_count)
{ /*! IMPROVEMENT:We may be able to move tile rotation processing into another function for refactoring.*/
    if (nullptr == permutateable)
    {
        Logger.log(libcpp59::log_level::ERR, "Encountered a nullptr while trying to permutate a tile!");
        throw std::invalid_argument("Encountered a nullptr while trying to permutate a tile!");
    }

    size_t permutation_limiter = ROTATION_ARR.size();
    if (permutateable->connections.mask == rotate_connections(Connection_Rotations::One_Eighty, permutateable->connections).mask)
    {
        permutation_limiter = ROTATION_ARR.size() - 2;
    }

    Logger.log(libcpp59::log_level::DEBUG, std::format("Permutating Tile:{}", permutateable->to_string()));

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
        tile->image = std::make_shared<QImage>(*permutateable->image);
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
        std::string flipped_filename = flipped->to_filename();
        flipped->path = std::filesystem::path(std::format("{}/{}",
                                                          permutateable->path.parent_path().generic_string(),
                                                          flipped_filename));
        flipped->image = std::make_shared<QImage>(*permutateable->image);
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
            tile->image = std::make_shared<QImage>(*permutateable->image);
            permutations.push_back(tile);
        }
    }
}

inline std::string const D_Tile::to_filename()
{
    std::stringstream ss;
    ss << name << ";" << theme << ";";

    // Build a vector of connection tokens to properly place commas or the ending semicolon.
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

bool D_Tile::generate_tile_img()
{
    if (!image || image->isNull())
    {

        Logger.log(libcpp59::log_level::ERR, std::format("Null image reference found when generating a tile image! Tile path: {}", path.generic_string()));
        throw std::invalid_argument("Null image reference found when generating a tile image!");
    }

    QTransform matrix;
    double degrees = 0.0;

    if (is_flipped())
        image->flip(Qt::Horizontal);

    switch (rotation_amount)
    {
    case Connection_Rotations::Nintey:
        degrees = 90.0;
        break;

    case Connection_Rotations::One_Eighty:
        degrees = 180.0;
        break;

    case Connection_Rotations::Two_Seventy:
        degrees = 270.0;
        break;

    default: // No Rotation required.
        break;
    }

    if (0 < degrees)
    {
        matrix.rotate(degrees);
        *image = image->transformed(matrix);
    }

    return image->save(QString::fromStdString(path.generic_string()), "JPG", DEFAULT_OUTPUT_QUALITY);
}

void D_Tile::copy_tile_img(std::filesystem::path loaded_dir)
{
    std::filesystem::path new_path = loaded_dir / path.filename();
    if (std::filesystem::exists(new_path) &&
        std::filesystem::equivalent(new_path, path))
    {
        Logger.log(libcpp59::log_level::DEBUG, std::format("Tile image already exists at path and is equivalent: {} -> Skipping copy...", new_path.generic_string()));
        return;
    }

    try
    {
        std::filesystem::copy(path, new_path, std::filesystem::copy_options::overwrite_existing);
    }
    catch (std::filesystem::filesystem_error const &fs_e)
    {
        std::stringstream err;
        err << "Unable to copy tile image. Filesystem error was: ";
        err << fs_e.what();
        Logger.log(libcpp59::log_level::ERR, err.str());
        throw;
    }

    path = new_path;
}

inline D_Connections D_Tile::rotate_connections(Connection_Rotations rotation, D_Connections to_rotate)
{
    return {.mask = std::rotl(to_rotate.mask, (static_cast<uint8_t>(rotation) * TILE_SIDE_CONNECTION_SIZE))};
}

inline D_Connections D_Tile::flip_connections(D_Connections to_flip)
{
    D_Connections out = {.mask = CONNECTION_ZERO_MASK};

    // Reverse top and bottom
    out.side_masks.top = reverse_8bits(to_flip.side_masks.top);
    out.side_masks.bottom = reverse_8bits(to_flip.side_masks.bottom);
    // Right becomes Left AND reverses
    out.side_masks.left = reverse_8bits(to_flip.side_masks.right);
    // Left becomes Right AND reverses
    out.side_masks.right = reverse_8bits(to_flip.side_masks.left);

    return out;
}

std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> D_Tile::filter_by_theme(std::string const &theme)
{
    std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> filtered_tiles;
    std::size_t reserve_size = 0;

    for (auto const &pair : Tile_Map)
        if (pair.second->get_theme() == theme)
            ++reserve_size;

    filtered_tiles.reserve(reserve_size);
    for (auto const &pair : Tile_Map)
        if (pair.second->get_theme() == theme)
            filtered_tiles.emplace(pair.first, pair.second);

    return filtered_tiles;
}
