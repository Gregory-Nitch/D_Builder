/***********************************************************************************************************************
 * @date 2026-09-22
 * @author Gregory Nitch
 *
 * @brief Tile graphics item source file, contains the implementation of the tile graphics item class.
 **********************************************************************************************************************/

/*
************************************************************************************************************************
- - Local Includes - -
************************************************************************************************************************
*/

#include "d_tile_graphics_item.hpp"

D_TileGraphicsItem::D_TileGraphicsItem(std::shared_ptr<D_Tile> const &tile, std::size_t row, std::size_t col, std::function<void(std::size_t row, std::size_t col, Qt::MouseButton)> clickHandler, QGraphicsItem *parent)
    : QGraphicsPixmapItem(QPixmap::fromImage(*tile->get_image()), parent),
      row(row),
      col(col),
      clickHandler(clickHandler)
{
    setAcceptHoverEvents(true);
}

void D_TileGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (clickHandler)
        clickHandler(row, col, event->button());

    QGraphicsPixmapItem::mousePressEvent(event);
}

void D_TileGraphicsItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);

    if (!is_hovered)
        return;

    QPen outlinePen(Qt::red);
    outlinePen.setWidth(3);
    painter->setPen(outlinePen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(boundingRect().adjusted(1.5, 1.5, -1.5, -1.5));
}

void D_TileGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    is_hovered = true;
    update();
    QGraphicsPixmapItem::hoverEnterEvent(event);
}

void D_TileGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    is_hovered = false;
    update();
    QGraphicsPixmapItem::hoverLeaveEvent(event);
}