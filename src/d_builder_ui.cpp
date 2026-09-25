/***********************************************************************************************************************
 * @date 2026-09-07
 * @author Gregory Nitch
 *
 * @brief UI source file, contains the implementation of the user interface for the UI.
 **********************************************************************************************************************/

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <algorithm>
#include <format>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_builder_ui.hpp"
#include "d_map.hpp"
#include "d_tile.hpp"
#include "ui_D_Builder.h" // Generated header for the D_Builder UI

/*
========================================================================================================================
- - 3rd Party Includes - -
========================================================================================================================
*/

#include <QFrame>
#include <QTimer>
#include <QMessageBox>
#include <QStringList>

DBuilderUI::DBuilderUI(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow),
                                          graphicsViewScene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    QStringList themes;
    themes.reserve(static_cast<qsizetype>(Loaded_Img_Dirs.size()));

    for (const auto &[themeName, loadedDirectory] : Loaded_Img_Dirs)
    {
        themes.append(QString::fromStdString(themeName));
    }

    themes.sort(Qt::CaseInsensitive);
    ui->StyleComboBox->addItems(themes);

    connect(ui->GenerateButton, &QPushButton::clicked, this, &DBuilderUI::onGenerateButtonClicked);
    connect(ui->SaveButton, &QPushButton::clicked, this, &DBuilderUI::onSaveButtonClicked);
    connect(ui->LoadTileSetButton, &QPushButton::clicked, this, &DBuilderUI::onLoadTileSetButtonClicked);
    connect(ui->PercentConnectionsSpinner, &QSpinBox::valueChanged, this, &DBuilderUI::onPercentConnectionChanged);
    connect(ui->RowsSpinner, &QSpinBox::valueChanged, this, &DBuilderUI::onNumRowsCChanged);
    connect(ui->ColumnsSpinner, &QSpinBox::valueChanged, this, &DBuilderUI::onNumColsChanged);
    connect(ui->StyleComboBox, &QComboBox::currentTextChanged, this, &DBuilderUI::onStyleChanged);

    const auto &displayMat = Dungeon_Map->get_display_mat();
    tile_graphics_mat.resize(displayMat.size());
    for (std::size_t col = 0; col < displayMat.size(); ++col)
        tile_graphics_mat[col].resize(displayMat[col].size());

    for (std::size_t col = 0; col < displayMat.size(); ++col)
    {
        for (std::size_t row = 0; row < displayMat[col].size(); ++row)
        {
            const auto &tile = displayMat[col][row];
            if (tile)
            {
                auto *tileItem = new D_TileGraphicsItem(tile, row, col, nullptr);
                tile_graphics_mat[col][row] = tileItem;
                tileItem->setPos(col * tile->get_image()->width(), row * tile->get_image()->height());
                graphicsViewScene->addItem(tileItem);
            }
        }
    }

    ui->graphicsView->setScene(graphicsViewScene);
    ui->graphicsView->setFrameShape(QFrame::NoFrame);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    QTimer::singleShot(0, this, [this]
                       { resetGraphicsView(); });
}

DBuilderUI::~DBuilderUI()
{
    delete ui;
}

void DBuilderUI::resetGraphicsView()
{
    const QRectF mapBounds = graphicsViewScene->itemsBoundingRect();
    graphicsViewScene->setSceneRect(mapBounds);
    ui->graphicsView->resetTransform();
    ui->graphicsView->fitInView(mapBounds, Qt::KeepAspectRatio);
    ui->graphicsView->centerOn(mapBounds.center());
    minZoom = ui->graphicsView->transform().m11();
    ui->graphicsView->setZoomLimits(minZoom, minZoom * maxZoomMultiplier);
}

void DBuilderUI::onGenerateButtonClicked()
{
    requires_map_generation = false;
    //! TODO: Remove overlay from graphics view

    Active_Theme_Map = D_Tile::filter_by_theme(ui->StyleComboBox->currentText().toLower().toStdString());
    Dungeon_Map->generate(ui->ColumnsSpinner->value(),
                          ui->RowsSpinner->value(),
                          ui->PercentConnectionsSpinner->value(),
                          Active_Theme_Map);

    // Clear our view state for before updating the graphics view
    graphicsViewScene->clear();
    tile_graphics_mat.clear();

    const auto &displayMat = Dungeon_Map->get_display_mat();
    tile_graphics_mat.resize(displayMat.size());
    for (std::size_t col = 0; col < displayMat.size(); ++col)
        tile_graphics_mat[col].resize(displayMat[col].size());

    for (std::size_t col = 0; col < displayMat.size(); ++col)
    {
        for (std::size_t row = 0; row < displayMat[col].size(); ++row)
        {
            const auto &tile = displayMat[col][row];
            if (tile)
            {
                auto *tileItem = new D_TileGraphicsItem(tile, row, col, nullptr);
                tile_graphics_mat[col][row] = tileItem;
                tileItem->setPos(col * tile->get_image()->width(), row * tile->get_image()->height());
                graphicsViewScene->addItem(tileItem);
            }
        }
    }

    resetGraphicsView();
}
void DBuilderUI::onSaveButtonClicked()
{
    if (requires_map_generation)
    {
        Logger.log(libcpp59::log_level::INFO, "Graphics view needs to be reset before saving.");
        QMessageBox::information(this, "Info", "Graphics view needs to be reset before saving.");
        return;
    }

    //! TODO: Implementation for the save button click event
}

void DBuilderUI::onLoadTileSetButtonClicked()
{
    requires_map_generation = true;
    //! TODO: Implementation for the load tile set button click event
}

void DBuilderUI::onPercentConnectionChanged()
{
    requires_map_generation = true;

    //! TODO: Implementation for the percent connection value change event
}

void DBuilderUI::onNumRowsCChanged()
{
    requires_map_generation = true;

    //! TODO: Implementation for the number of rows value change event
}

void DBuilderUI::onNumColsChanged()
{
    requires_map_generation = true;

    //! TODO: Implementation for the number of columns value change event
}

void DBuilderUI::onStyleChanged()
{
    requires_map_generation = true;

    //! TODO: Implementation for the style change event
}
