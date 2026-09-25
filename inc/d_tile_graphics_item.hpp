/***********************************************************************************************************************
 * @date 2026-09-22
 * @author Gregory Nitch
 *
 * @brief Tile graphics item header, contains the declaration of the tile graphics item class.
 **********************************************************************************************************************/

#pragma once

/*
************************************************************************************************************************
- - System Includes - -
************************************************************************************************************************
*/

/*
************************************************************************************************************************
- - Local Includes - -
************************************************************************************************************************
*/

#include "d_tile.hpp"

/*
************************************************************************************************************************
- - 3rd Party Includes - -
************************************************************************************************************************
*/

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

class D_TileGraphicsItem : public QGraphicsPixmapItem
{
public:
    /***********************************************************************************************************************
     * @brief Creates a graphics item for a tile at the supplied map coordinates.
     *
     * @param[in] tile Tile whose image is displayed.
     * @param[in] row Row of the tile in the map.
     * @param[in] col Column of the tile in the map.
     * @param[in] clickHandler Callback invoked when the item is clicked.
     * @param[in] parent Parent graphics item, if any.
     **********************************************************************************************************************/
    D_TileGraphicsItem(std::shared_ptr<D_Tile> const &tile, std::size_t row, std::size_t col, std::function<void(std::size_t row, std::size_t col, Qt::MouseButton)> clickHandler, QGraphicsItem *parent = nullptr);
    std::size_t row;
    std::size_t col;
    std::function<void(std::size_t row, std::size_t col, Qt::MouseButton)> clickHandler;

protected:
    /***********************************************************************************************************************
     * @brief Invokes the tile click handler for a mouse press.
     *
     * @param[in] event Mouse press event.
     **********************************************************************************************************************/
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /***********************************************************************************************************************
     * @brief Paints the tile and its hover outline when applicable.
     *
     * @param[in] painter Painter used to draw the item.
     * @param[in] option Item style options.
     * @param[in] widget Optional widget being painted into.
     **********************************************************************************************************************/
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    /***********************************************************************************************************************
     * @brief Enables the hover outline when the cursor enters the tile.
     *
     * @param[in] event Hover-enter event.
     **********************************************************************************************************************/
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    /***********************************************************************************************************************
     * @brief Disables the hover outline when the cursor leaves the tile.
     *
     * @param[in] event Hover-leave event.
     **********************************************************************************************************************/
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    bool is_hovered = false;
};
