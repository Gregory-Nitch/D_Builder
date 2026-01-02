/***********************************************************************************************************************
 * LICENSE : TODO!
 *
 * @date : 2025-12-14
 * @author : Gregory Nitch
 *
 * @brief : Header for the D_Tile class, it represents a section that can be used to construct the D_Map display matrix.
 * For documentation for each function @see d_tile.cpp.
 **********************************************************************************************************************/

#pragma once

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <array>
#include <filesystem>
#include <atomic>

/*
========================================================================================================================
- - Start of Connection_Rotations Enum - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @enum Connection_Rotations
 * @brief Represents the different rotation degrees when permutating a tile.
 *
 * @remarks Values:
 *      Nintey = 1, // Start from one for bit shifting operations
 *      One_Eighty = 2,
 *      Two_Seventy = 3,
 **********************************************************************************************************************/
enum class Connection_Rotations
{
    Nintey = 1,
    One_Eighty = 2,
    Two_Seventy = 3,
};

/***********************************************************************************************************************
 * @brief An array of the Connection_Rotation enums used for iteration.
 **********************************************************************************************************************/
constexpr std::array<Connection_Rotations, 3> ROTATION_ARR = {
    Connection_Rotations::Nintey,
    Connection_Rotations::One_Eighty,
    Connection_Rotations::Two_Seventy};

/*
========================================================================================================================
- - Start of D_Connection Union - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @union D_Connections
 * @brief Represents connections that each tile can have, in a uint32_t.
 *
 * @members :
 *      @public bits = Each indivitual unit32_t bit. T0-L7 (clockwise)
 *      @public side_masks = each side as a uint8_mask.
 *      @public mask = The entire uint32_t to use as a complete mask over the bits.
 **********************************************************************************************************************/
union D_Connections
{
    struct
    {
        // Top bits 0-7
        uint32_t T0 : 1;
        uint32_t T1 : 1;
        uint32_t T2 : 1;
        uint32_t T3 : 1;
        uint32_t T4 : 1;
        uint32_t T5 : 1;
        uint32_t T6 : 1;
        uint32_t T7 : 1;

        // Right bits 8-15
        uint32_t R0 : 1;
        uint32_t R1 : 1;
        uint32_t R2 : 1;
        uint32_t R3 : 1;
        uint32_t R4 : 1;
        uint32_t R5 : 1;
        uint32_t R6 : 1;
        uint32_t R7 : 1;

        // Bottom bits 16-23
        uint32_t B0 : 1;
        uint32_t B1 : 1;
        uint32_t B2 : 1;
        uint32_t B3 : 1;
        uint32_t B4 : 1;
        uint32_t B5 : 1;
        uint32_t B6 : 1;
        uint32_t B7 : 1;

        // Left bits 24-31
        uint32_t L0 : 1;
        uint32_t L1 : 1;
        uint32_t L2 : 1;
        uint32_t L3 : 1;
        uint32_t L4 : 1;
        uint32_t L5 : 1;
        uint32_t L6 : 1;
        uint32_t L7 : 1;
    } bits;

    struct
    {
        uint8_t top;
        uint8_t right;
        uint8_t bottom;
        uint8_t left;
    } side_masks;

    uint32_t mask;
};

/***********************************************************************************************************************
 * @brief Map of valid connection strings to bits in the D_Connection union.
 **********************************************************************************************************************/
std::unordered_map<std::string, uint32_t> const Connection_Str_to_Bit_Mask_Map = {
    // Top bits 0-7
    {"T0", 1},
    {"T1", (1 << 1)},
    {"T2", (1 << 2)},
    {"T3", (1 << 3)},
    {"T4", (1 << 4)},
    {"T5", (1 << 5)},
    {"T6", (1 << 6)},
    {"T7", (1 << 7)},
    // Right bits 8-15
    {"R0", (1 << 8)},
    {"R1", (1 << 9)},
    {"R2", (1 << 10)},
    {"R3", (1 << 11)},
    {"R4", (1 << 12)},
    {"R5", (1 << 13)},
    {"R6", (1 << 14)},
    {"R7", (1 << 15)},
    // Bottom bits 16-23
    {"B0", (1 << 16)},
    {"B1", (1 << 17)},
    {"B2", (1 << 18)},
    {"B3", (1 << 19)},
    {"B4", (1 << 20)},
    {"B5", (1 << 21)},
    {"B6", (1 << 22)},
    {"B7", (1 << 23)},
    // Left bits 24-31
    {"L0", (1 << 24)},
    {"L1", (1 << 25)},
    {"L2", (1 << 26)},
    {"L3", (1 << 27)},
    {"L4", (1 << 28)},
    {"L5", (1 << 29)},
    {"L6", (1 << 30)},
    {"L7", (1 << 31)},
};

/***********************************************************************************************************************
 * @brief Map of valid bits to connection strings in the D_Connection union.
 **********************************************************************************************************************/
std::unordered_map<uint32_t, std::string> const Connection_Bit_Mask_to_Str_Map = {
    // Top bits 0-7
    {1, "T0"},
    {(1 << 1), "T1"},
    {(1 << 2), "T2"},
    {(1 << 3), "T3"},
    {(1 << 4), "T4"},
    {(1 << 5), "T5"},
    {(1 << 6), "T6"},
    {(1 << 7), "T7"},
    // Right bits 8-15
    {(1 << 8), "R0"},
    {(1 << 9), "R1"},
    {(1 << 10), "R2"},
    {(1 << 11), "R3"},
    {(1 << 12), "R4"},
    {(1 << 13), "R5"},
    {(1 << 14), "R6"},
    {(1 << 15), "R7"},
    // Bottom bits 16-23
    {(1 << 16), "B0"},
    {(1 << 17), "B1"},
    {(1 << 18), "B2"},
    {(1 << 19), "B3"},
    {(1 << 20), "B4"},
    {(1 << 21), "B5"},
    {(1 << 22), "B6"},
    {(1 << 23), "B7"},
    // Left bits 24-31
    {(1 << 24), "L0"},
    {(1 << 25), "L1"},
    {(1 << 26), "L2"},
    {(1 << 27), "L3"},
    {(1 << 28), "L4"},
    {(1 << 29), "L5"},
    {(1 << 30), "L6"},
    {(1 << 31), "L7"},
};

/*
========================================================================================================================
- - Start of D_Tile Class - -
========================================================================================================================
*/

/***********************************************************************************************************************
 * @class D_Tile
 * @brief Represents a possbile section in the D_Map object.
 *
 * @members :
 *      @private std::filesystem::path path = Path to the actual image.
 *      @private std::string name = Name of the section.
 *      @private std::string theme = Theme of the section.
 *      @private uint64_t id = ID of the section.
 *      @private D_Connections connections = Connection bit map of the tile connections.
 *      @private bool is_permutateable_flag = Whether or not the tile is permutable.
 *      @private bool is_entrance_flag = Whether or not the tile is an entrance.
 *      @private bool is_exit_flag = Whether or not the tile is an exit.
 *      @private bool is_flippable_flag = Whether or not the tile is flippable.
 *
 *      //! NOTE: May be replaced later with id set by a database.
 *      @private static std::atomic<uint64_t> id_counter = Static class varible used to assign IDs to loaded and generate tiles.
 **********************************************************************************************************************/
class D_Tile
{
public:
    D_Tile(std::filesystem::path const &path);
    ~D_Tile();
    static void load_tiles(std::filesystem::path const &dir_path, std::filesystem::path const &loaded_path = "");
    static void generate_tiles();
    std::string const &get_name() const;
    std::string const &get_theme() const;
    uint64_t const get_id() const;
    D_Connections const get_connections() const;
    bool const is_permutateable() const;
    bool const is_entrance() const;
    bool const is_exit() const;
    bool const is_flippable() const;
    std::string const to_string() const;

private:
    std::filesystem::path path;
    std::string name;
    std::string theme;
    uint64_t id;
    D_Connections connections;
    bool is_permutateable_flag;
    bool is_entrance_flag;
    bool is_exit_flag;
    bool is_flippable_flag;

    //! NOTE: May be replaced later with id set by a database.
    static std::atomic<uint64_t> id_counter;

    D_Tile(std::string permutation_name,
           std::string permutation_theme,
           uint64_t permutation_id,
           D_Connections permutation_connections,
           bool permutation_is_entrance_flag,
           bool permutation_is_exit_flag,
           bool permutation_is_permutable_flag,
           bool permutation_is_flippable_flag);
    inline void map_connection_tokens(std::vector<std::string> connection_tokens);
    static inline void permutate(std::shared_ptr<D_Tile> permutateable,
                                 std::vector<std::shared_ptr<D_Tile>> &permutations,
                                 size_t &entrance_count,
                                 size_t &exit_count);
    inline std::string const to_filename();
    void generate_tile_img();
    void copy_tile_img(std::filesystem::path loaded_dir);
    static inline D_Connections rotate_connections(Connection_Rotations rotation, D_Connections to_rotate);
    static inline D_Connections flip_connections(D_Connections to_flip);
};
