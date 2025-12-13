#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <atomic>

typedef union D_Connections;

class D_Tile
{
public:
    D_Tile(std::filesystem::path const &path);
    ~D_Tile();
    static void load_tiles(std::filesystem::directory_entry const &dir_path);
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
    static inline std::vector<std::shared_ptr<D_Tile>> permutate(std::shared_ptr<D_Tile> permutateable);
};

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

    uint32_t mask;
};

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
