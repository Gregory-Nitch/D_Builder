#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <vector>

typedef struct D_Connections
{
        uint32_t T0 : 1;
        uint32_t T1 : 1;
        uint32_t T2 : 1;
        uint32_t T3 : 1;
        uint32_t T4 : 1;
        uint32_t T5 : 1;
        uint32_t T6 : 1;
        uint32_t T7 : 1;
        uint32_t R0 : 1;
        uint32_t R1 : 1;
        uint32_t R2 : 1;
        uint32_t R3 : 1;
        uint32_t R4 : 1;
        uint32_t R5 : 1;
        uint32_t R6 : 1;
        uint32_t R7 : 1;
        uint32_t B0 : 1;
        uint32_t B1 : 1;
        uint32_t B2 : 1;
        uint32_t B3 : 1;
        uint32_t B4 : 1;
        uint32_t B5 : 1;
        uint32_t B6 : 1;
        uint32_t B7 : 1;
        uint32_t L0 : 1;
        uint32_t L1 : 1;
        uint32_t L2 : 1;
        uint32_t L3 : 1;
        uint32_t L4 : 1;
        uint32_t L5 : 1;
        uint32_t L6 : 1;
        uint32_t L7 : 1;
}D_Connections;

class D_Tile
{
public:
    D_Tile(std::string path);
    ~D_Tile();
    static void load_tiles(std::string dir_path);
    std::string const & get_name() const;
    std::string const & get_theme() const;
    uint64_t const get_id() const;
    D_Connections const get_connections() const;
    bool const is_permutateable() const;
    bool const is_entrance() const;
    bool const is_exit() const;
    bool const is_flippable() const;

private:
    std::string name;
    std::string theme;
    uint64_t id;
    D_Connections connections;
    bool is_permutateable_flag;
    bool is_entrance_flag;
    bool is_exit_flag;
    bool is_flippable_flag;

    D_Tile();
    static inline std::vector<std::shared_ptr<D_Tile>> permutate(std::shared_ptr<D_Tile> permutateable);
};
