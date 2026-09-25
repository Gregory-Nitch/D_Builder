/***********************************************************************************************************************
 * @date 2026-09-25
 * @author Gregory Nitch
 *
 * @brief Graphics view implementation that constrains panning to the map scene.
 **********************************************************************************************************************/

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <algorithm>
#include <cmath>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_map_graphics_view.hpp"

/*
========================================================================================================================
- - 3rd Party Includes - -
========================================================================================================================
*/

#include <QWheelEvent>

/*
========================================================================================================================
- - Macros - -
========================================================================================================================
*/

constexpr qreal MIN_VISIBLE_MAP_PORTION = 0.15;
constexpr qreal ZOOM_FACTOR_PER_WHEEL_STEP = 1.15;

D_MapGraphicsView::D_MapGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void D_MapGraphicsView::setZoomLimits(qreal minimum_scale, qreal maximum_scale)
{
    minimum_zoom_scale = minimum_scale;
    maximum_zoom_scale = std::max(minimum_scale, maximum_scale);
}

void D_MapGraphicsView::clampToMapBounds()
{
    if (is_clamping || scene() == nullptr)
        return;

    const QRectF map_bounds = sceneRect();
    const QRectF visible_bounds = mapToScene(viewport()->rect()).boundingRect();
    if (map_bounds.isEmpty() || visible_bounds.isEmpty())
        return;

    const auto clamp_axis_center = [](qreal visible_center,
                                      qreal visible_size,
                                      qreal map_start,
                                      qreal map_size) -> qreal
    {
        const qreal map_end = map_start + map_size;
        if (visible_size <= map_size)
        {
            return std::clamp(visible_center,
                              map_start + (visible_size / 2.0),
                              map_end - (visible_size / 2.0));
        }

        const qreal required_overlap = map_size * MIN_VISIBLE_MAP_PORTION;
        return std::clamp(visible_center,
                          map_start + required_overlap - (visible_size / 2.0),
                          map_end - required_overlap + (visible_size / 2.0));
    };

    const QPointF constrained_center(
        clamp_axis_center(visible_bounds.center().x(), visible_bounds.width(), map_bounds.left(), map_bounds.width()),
        clamp_axis_center(visible_bounds.center().y(), visible_bounds.height(), map_bounds.top(), map_bounds.height()));

    if (constrained_center == visible_bounds.center())
        return;

    is_clamping = true;
    centerOn(constrained_center);
    is_clamping = false;
}

void D_MapGraphicsView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    clampToMapBounds();
}

void D_MapGraphicsView::wheelEvent(QWheelEvent *event)
{
    const qreal wheel_steps = static_cast<qreal>(event->angleDelta().y()) / 120.0;
    if (wheel_steps == 0.0 || minimum_zoom_scale <= 0.0)
    {
        event->ignore();
        return;
    }

    const qreal current_scale = transform().m11();
    const qreal requested_scale = current_scale * std::pow(ZOOM_FACTOR_PER_WHEEL_STEP, wheel_steps);
    const qreal constrained_scale = std::clamp(requested_scale, minimum_zoom_scale, maximum_zoom_scale);
    const qreal scale_factor = constrained_scale / current_scale;

    if (scale_factor != 1.0)
        scale(scale_factor, scale_factor);

    clampToMapBounds();
    event->accept();
}