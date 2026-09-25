/***********************************************************************************************************************
 * @date 2026-09-22
 * @author Gregory Nitch
 *
 * @brief Tile graphics item source file, contains the implementation of the tile graphics item class.
 **********************************************************************************************************************/

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_tile_graphics_item.hpp"

D_TileGraphicsItem::D_TileGraphicsItem(std::shared_ptr<D_Tile> const &tile, std::size_t row, std::size_t col, QGraphicsItem *parent)
    : QGraphicsPixmapItem(QPixmap::fromImage(*tile->get_image()), parent),
      row(row),
      col(col)
{
}