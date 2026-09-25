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
#include "d_tile_replacement_dialog.hpp"
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
#include <QLabel>
#include <QVBoxLayout>

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
                clickHandler = [this](std::size_t row, std::size_t col, Qt::MouseButton button)
                {
                    on_tile_right_clicked(row, col, button);
                };
                auto *tileItem = new D_TileGraphicsItem(tile, row, col, clickHandler, nullptr);
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

    mapInteractionOverlay = new QWidget(ui->graphicsView);
    mapInteractionOverlay->setStyleSheet("background-color: rgba(50, 50, 50, 220);");
    auto *overlayLayout = new QVBoxLayout(mapInteractionOverlay);
    auto *overlayMessage = new QLabel("Map settings changed. Generate a new map to continue.", mapInteractionOverlay);
    overlayMessage->setAlignment(Qt::AlignCenter);
    overlayMessage->setWordWrap(true);
    overlayMessage->setStyleSheet("background: transparent; color: #e05252; font-size: 24px;");
    overlayLayout->addWidget(overlayMessage);
    updateMapInteractionOverlayGeometry();
    mapInteractionOverlay->hide();
    ui->graphicsView->viewport()->installEventFilter(this);

    QTimer::singleShot(0, this, [this]
                       {
                           resetGraphicsView();
                           updateMapInteractionOverlayGeometry(); });
}

DBuilderUI::~DBuilderUI()
{
    delete ui;
}

bool DBuilderUI::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->graphicsView->viewport() && event->type() == QEvent::Resize)
    {
        QTimer::singleShot(0, this, [this]
                           { updateMapInteractionOverlayGeometry(); });
    }

    return QMainWindow::eventFilter(watched, event);
}

void DBuilderUI::setMapGenerationRequired(bool required)
{
    requires_map_generation = required;
    updateMapInteractionOverlayGeometry();
    mapInteractionOverlay->setVisible(required);
    if (required)
        mapInteractionOverlay->raise();
}

void DBuilderUI::updateMapInteractionOverlayGeometry()
{
    mapInteractionOverlay->setGeometry(ui->graphicsView->viewport()->geometry());
}

void DBuilderUI::on_tile_right_clicked(std::size_t row, std::size_t col, Qt::MouseButton button)
{
    if (button == Qt::RightButton)
    {
        std::shared_ptr<D_Tile> const &current_tile = Dungeon_Map->get_display_mat().at(col).at(row);
        TileReplacementDialog dialog(current_tile, Active_Theme_Map, this);
        if (dialog.exec() != QDialog::Accepted)
            return;

        std::shared_ptr<D_Tile> replacement = dialog.selectedTile();
        if (!replacement)
            return;

        Logger.log(libcpp59::log_level::INFO, "Replacing tile [" + current_tile->get_name() + "] at row " + std::to_string(row) + ", col " + std::to_string(col) + " with tile: " + replacement->get_name());
        Dungeon_Map->swap_tile(
            static_cast<uint8_t>(col),
            static_cast<uint8_t>(row),
            replacement);

        tile_graphics_mat[col][row]->setPixmap(
            QPixmap::fromImage(*replacement->get_image()));
    }
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
                auto *tileItem = new D_TileGraphicsItem(tile, row, col, clickHandler, nullptr);
                tile_graphics_mat[col][row] = tileItem;
                tileItem->setPos(col * tile->get_image()->width(), row * tile->get_image()->height());
                graphicsViewScene->addItem(tileItem);
            }
        }
    }

    resetGraphicsView();
    setMapGenerationRequired(false);
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
    setMapGenerationRequired(true);
    //! TODO: Implementation for the load tile set button click event
}

void DBuilderUI::onPercentConnectionChanged()
{
    setMapGenerationRequired(true);
}

void DBuilderUI::onNumRowsCChanged()
{
    setMapGenerationRequired(true);
}

void DBuilderUI::onNumColsChanged()
{
    setMapGenerationRequired(true);
}

void DBuilderUI::onStyleChanged()
{
    setMapGenerationRequired(true);
}
