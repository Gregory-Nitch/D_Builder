/***********************************************************************************************************************
 * @date 2026-09-25
 * @author Gregory Nitch
 *
 * @brief Graphics view that keeps a portion of its map scene visible while panning.
 **********************************************************************************************************************/

#pragma once

/*
========================================================================================================================
- - 3rd Party Includes - -
========================================================================================================================
*/

#include <QGraphicsView>

class D_MapGraphicsView : public QGraphicsView
{
public:
    explicit D_MapGraphicsView(QWidget *parent = nullptr);
    void setZoomLimits(qreal minimum_scale, qreal maximum_scale);
    void clampToMapBounds();

protected:
    void scrollContentsBy(int dx, int dy) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    bool is_clamping = false;
    qreal minimum_zoom_scale = 0.0;
    qreal maximum_zoom_scale = 0.0;
};