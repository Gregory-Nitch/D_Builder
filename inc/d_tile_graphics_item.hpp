/***********************************************************************************************************************
 * @date 2026-09-22
 * @author Gregory Nitch
 *
 * @brief Tile graphics item header, contains the declaration of the tile graphics item class.
 **********************************************************************************************************************/

#pragma once

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_tile.hpp"

/*
========================================================================================================================
- - 3rd Party Includes - -
========================================================================================================================
*/

#include <QGraphicsPixmapItem>

class D_TileGraphicsItem : public QGraphicsPixmapItem
{
public:
    D_TileGraphicsItem(std::shared_ptr<D_Tile> const &tile, std::size_t row, std::size_t col, std::function<void(std::size_t row, std::size_t col, Qt::MouseButton)> clickHandler, QGraphicsItem *parent = nullptr);
    std::size_t row;
    std::size_t col;
    std::function<void(std::size_t row, std::size_t col, Qt::MouseButton)> clickHandler;
};