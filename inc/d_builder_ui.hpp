/***********************************************************************************************************************
 * @date 2026-09-07
 * @author Gregory Nitch
 *
 * @brief UI header, contains user interface related declarations for the UI.
 **********************************************************************************************************************/

#pragma once

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <vector>
#include <memory>

/*
========================================================================================================================
- - 3rd Party Includes - -
========================================================================================================================
*/

#include <QMainWindow>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QObject>
#include <QEvent>
#include <QtGlobal>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_builder_common.hpp"
#include "d_tile_graphics_item.hpp"

namespace Ui
{
    class MainWindow;
}

class DBuilderUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit DBuilderUI(QWidget *parent = nullptr);
    ~DBuilderUI();

    Ui::MainWindow *ui;
    QGraphicsScene *graphicsViewScene;
    std::vector<std::vector<D_TileGraphicsItem *>> tile_graphics_mat;

private:
    bool requires_map_generation = false;
    qreal maxZoomMultiplier = 8.0;
    qreal minZoom = 0.1;
    void resetGraphicsView();
    void on_tile_right_clicked(std::size_t row, std::size_t col, Qt::MouseButton button);

private slots:
    void onGenerateButtonClicked();
    void onSaveButtonClicked();
    void onLoadTileSetButtonClicked();
    void onPercentConnectionChanged();
    void onNumRowsCChanged();
    void onNumColsChanged();
    void onStyleChanged();
};