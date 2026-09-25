/***********************************************************************************************************************
 * @date 2026-09-25
 * @author Gregory Nitch
 *
 * @brief Graphics view that keeps a portion of its map scene visible while panning.
 **********************************************************************************************************************/

#pragma once

/*
************************************************************************************************************************
- - 3rd Party Includes - -
************************************************************************************************************************
*/

#include <QGraphicsView>

class D_MapGraphicsView : public QGraphicsView
{
public:
    /***********************************************************************************************************************
     * @brief Creates a graphics view that keeps part of its map scene visible while panning.
     *
     * @param[in] parent Parent widget, if any.
     **********************************************************************************************************************/
    explicit D_MapGraphicsView(QWidget *parent = nullptr);

    /***********************************************************************************************************************
     * @brief Sets the minimum and maximum allowed zoom scales.
     *
     * @param[in] minimum_scale Lowest permitted scale.
     * @param[in] maximum_scale Highest permitted scale.
     **********************************************************************************************************************/
    void setZoomLimits(qreal minimum_scale, qreal maximum_scale);

    /***********************************************************************************************************************
     * @brief Recenters the view so that a portion of the map remains visible.
     **********************************************************************************************************************/
    void clampToMapBounds();

protected:
    /***********************************************************************************************************************
     * @brief Clamps the view after its scroll position changes.
     *
     * @param[in] dx Horizontal scroll offset.
     * @param[in] dy Vertical scroll offset.
     **********************************************************************************************************************/
    void scrollContentsBy(int dx, int dy) override;

    /***********************************************************************************************************************
     * @brief Zooms within configured limits and clamps the resulting view bounds.
     *
     * @param[in] event Wheel event that requested the zoom.
     **********************************************************************************************************************/
    void wheelEvent(QWheelEvent *event) override;

private:
    bool is_clamping = false;
    qreal minimum_zoom_scale = 0.0;
    qreal maximum_zoom_scale = 0.0;
};