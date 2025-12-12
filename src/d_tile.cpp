
#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "d_tile.hpp"
#include "d_builder_common.hpp"

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

void D_Tile::load_tiles(std::string dir_path)
{
    //! TODO: This
}

std::string const & D_Tile::get_name() const
{
    //! TODO: This
}

std::string const & D_Tile::get_theme() const
{
    //! TODO: This
}

uint64_t const D_Tile::get_id() const
{
    //! TODO: This
}

D_Connections const D_Tile::get_connections() const
{
    //! TODO: This
}

bool const D_Tile::is_permutateable() const
{
    //! TODO: This
}

bool const D_Tile::is_entrance() const
{
    //! TODO: This
}

bool const D_Tile::is_exit() const
{
    //! TODO: This
}

bool const D_Tile::is_flippable() const
{
    //! TODO: This
}

D_Tile::D_Tile()
{
    //! TODO: This
}

inline std::vector<std::shared_ptr<D_Tile>> D_Tile::permutate(std::shared_ptr<D_Tile> permutateable)
{
    //! TODO: This
}
