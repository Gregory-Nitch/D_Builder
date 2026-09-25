/***********************************************************************************************************************
 * @date 2026-09-07
 * @author Gregory Nitch
 *
 * @brief UI header, contains user interface related declarations for the UI.
 **********************************************************************************************************************/

#pragma once

/*
************************************************************************************************************************
- - System Includes - -
************************************************************************************************************************
*/

#include <vector>
#include <memory>

/*
************************************************************************************************************************
- - 3rd Party Includes - -
************************************************************************************************************************
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
#include <QWidget>
#include <QtGlobal>

/*
************************************************************************************************************************
- - Local Includes - -
************************************************************************************************************************
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
    /***********************************************************************************************************************
     * @brief Creates the main application window and initializes its map scene.
     *
     * @param[in] parent Parent widget, if any.
     **********************************************************************************************************************/
    explicit DBuilderUI(QWidget *parent = nullptr);

    /***********************************************************************************************************************
     * @brief Destroys the main application window.
     **********************************************************************************************************************/
    ~DBuilderUI();

    Ui::MainWindow *ui;
    QGraphicsScene *graphicsViewScene;
    std::vector<std::vector<D_TileGraphicsItem *>> tile_graphics_mat;

protected:
    /***********************************************************************************************************************
     * @brief Tracks graphics-view viewport resizing to keep the generation overlay aligned.
     *
     * @param[in] watched Object that received the event.
     * @param[in] event Event being filtered.
     *
     * @retval bool Whether the event was handled.
     **********************************************************************************************************************/
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    std::function<void(std::size_t row, std::size_t col, Qt::MouseButton)> clickHandler;
    QWidget *mapInteractionOverlay = nullptr;
    bool requires_map_generation = false;
    qreal maxZoomMultiplier = 8.0;
    qreal minZoom = 0.1;

    /***********************************************************************************************************************
     * @brief Sets whether a map must be generated before it can be interacted with or saved.
     *
     * @param[in] required Whether map generation is required.
     **********************************************************************************************************************/
    void setMapGenerationRequired(bool required);

    /***********************************************************************************************************************
     * @brief Resizes the map-interaction overlay to the graphics-view viewport.
     **********************************************************************************************************************/
    void updateMapInteractionOverlayGeometry();

    /***********************************************************************************************************************
     * @brief Fits the graphics view to the current map and resets its zoom limits.
     **********************************************************************************************************************/
    void resetGraphicsView();

    /***********************************************************************************************************************
     * @brief Opens the tile replacement dialog when a tile is right-clicked.
     *
     * @param[in] row Row of the clicked tile.
     * @param[in] col Column of the clicked tile.
     * @param[in] button Mouse button used for the click.
     **********************************************************************************************************************/
    void on_tile_right_clicked(std::size_t row, std::size_t col, Qt::MouseButton button);

private slots:
    /***********************************************************************************************************************
     * @brief Generates and displays a map from the selected settings.
     **********************************************************************************************************************/
    void onGenerateButtonClicked();

    /***********************************************************************************************************************
     * @brief Saves the current generated map as an image.
     **********************************************************************************************************************/
    void onSaveButtonClicked();

    /***********************************************************************************************************************
     * @brief Loads a tile set from a selected root directory and marks the current map as stale.
     **********************************************************************************************************************/
    void onLoadTileSetButtonClicked();

    /***********************************************************************************************************************
     * @brief Marks the current map as stale after the connection percentage changes.
     **********************************************************************************************************************/
    void onPercentConnectionChanged();

    /***********************************************************************************************************************
     * @brief Marks the current map as stale after the row count changes.
     **********************************************************************************************************************/
    void onNumRowsCChanged();

    /***********************************************************************************************************************
     * @brief Marks the current map as stale after the column count changes.
     **********************************************************************************************************************/
    void onNumColsChanged();

    /***********************************************************************************************************************
     * @brief Marks the current map as stale after the selected tile theme changes.
     **********************************************************************************************************************/
    void onStyleChanged();
};