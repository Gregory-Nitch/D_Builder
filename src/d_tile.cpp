
#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>

#include "inc/d_tile.hpp"

#define MAX_PERMUTATIONS (3)
#define MAX_FLIPPABLE_PERMUTATIONS (7)

D_Tile::D_Tile(std::string path)
{
    //! TODO: This
}

D_Tile::~D_Tile()
{
    //! TODO: This
}

void D_Tile::load_tiles(std::string dir_path,
                        std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> tiles,
                        std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> entrances,
                        std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> exits)
{
    //! TODO: This
}

std::string const &const D_Tile::get_name()
{
    //! TODO: This
}

std::string const &const D_Tile::get_theme()
{
    //! TODO: This
}

uint64_t const D_Tile::get_id()
{
    //! TODO: This
}

connections const D_Tile::get_connections()
{
    //! TODO: This
}

bool const D_Tile::is_permutateable()
{
    //! TODO: This
}

bool const D_Tile::is_entrance()
{
    //! TODO: This
}

bool const D_Tile::is_exit()
{
    //! TODO: This
}

bool const D_Tile::is_flippable()
{
    //! TODO: This
}

inline std::shared_ptr<D_Tile> D_Tile::permutate(std::shared_ptr<D_Tile> permutateable)
{
    //! TODO: This
}
