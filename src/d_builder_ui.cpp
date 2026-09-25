/***********************************************************************************************************************
 * @date 2026-09-07
 * @author Gregory Nitch
 *
 * @brief UI source file, contains the implementation of the user interface for the UI.
 **********************************************************************************************************************/

/*
************************************************************************************************************************
- - System Includes - -
************************************************************************************************************************
*/

#include <algorithm>
#include <filesystem>
#include <format>
#include <limits>

/*
************************************************************************************************************************
- - Local Includes - -
************************************************************************************************************************
*/

#include "d_builder_ui.hpp"
#include "d_map.hpp"
#include "d_tile.hpp"
#include "d_tile_replacement_dialog.hpp"
#include "ui_D_Builder.h" // Generated header for the D_Builder UI

/*
************************************************************************************************************************
- - 3rd Party Includes - -
************************************************************************************************************************
*/

#include <QFrame>
#include <QTimer>
#include <QMessageBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QCoreApplication>
#include <QEventLoop>
#include <QSignalBlocker>
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

    std::size_t cols = ui->ColumnsSpinner->value();
    std::size_t rows = ui->RowsSpinner->value();
    std::string file_path = std::format("{}{}_{}x{}.jpg",
                                        DEFAULT_OUTPUT_IMG_PATH,
                                        Active_Theme_Map.at(0)->get_theme(),
                                        cols,
                                        rows);

    const QString selectedFilePath = QFileDialog::getSaveFileName(
        this,
        "Save Dungeon Map",
        QString::fromStdString(file_path),
        "JPEG Image (*.jpg *.jpeg)");
    if (selectedFilePath.isEmpty())
        return;

    Dungeon_Map->save(selectedFilePath.toStdString());
}

void DBuilderUI::onLoadTileSetButtonClicked()
{
    const QString tileSetDirectory = QFileDialog::getExistingDirectory(
        this,
        "Select Tile Set Directory",
        QString::fromStdString(DEFAULT_INPUT_IMG_PATH));
    if (tileSetDirectory.isEmpty())
        return;

    QMessageBox generationPrompt(this);
    generationPrompt.setWindowTitle("Load Tile Set");
    generationPrompt.setText("Load the selected tile set?");
    generationPrompt.setInformativeText("Generate tile permutations before loading the tile set.");
    generationPrompt.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    generationPrompt.setDefaultButton(QMessageBox::Ok);
    auto *generateTilesCheckBox = new QCheckBox("Generate tile permutations", &generationPrompt);
    generationPrompt.setCheckBox(generateTilesCheckBox);
    if (generationPrompt.exec() != QMessageBox::Ok)
        return;

    const std::filesystem::path tileSetRootPath = tileSetDirectory.toStdString();
    std::vector<std::string> themeNames;
    try
    {
        for (const auto &entry : std::filesystem::directory_iterator(tileSetRootPath))
        {
            if (entry.is_directory())
                themeNames.push_back(entry.path().filename().string());
        }
    }
    catch (const std::filesystem::filesystem_error &error)
    {
        Logger.log(libcpp59::log_level::ERR, error.what());
        QMessageBox::critical(this, "Load Tile Set", error.what());
        return;
    }

    if (themeNames.empty())
    {
        const QString message = "The selected directory must contain at least one theme subdirectory.";
        Logger.log(libcpp59::log_level::ERR, message.toStdString());
        QMessageBox::warning(this, "Load Tile Set", message);
        return;
    }

    setMapGenerationRequired(true);
    Tile_Map.clear();
    Entrance_Map.clear();
    Exit_Map.clear();
    Active_Theme_Map.clear();
    Empty_Tile.reset();
    Loaded_Img_Dirs.clear();
    Loaded_Img_Dirs.reserve(themeNames.size());

    for (const std::string &themeName : themeNames)
    {
        const std::filesystem::path loadedThemePath =
            std::filesystem::path(DEFAULT_IMG_LOADED_ROOT_PATH) / themeName;
        std::filesystem::create_directories(loadedThemePath);
        Loaded_Img_Dirs.emplace(themeName, loadedThemePath);
    }

    QProgressDialog tileProgress("", QString(), 0, 1, this);
    tileProgress.setWindowTitle("D_Builder");
    tileProgress.setCancelButton(nullptr);
    tileProgress.setWindowModality(Qt::ApplicationModal);
    tileProgress.setMinimumDuration(0);
    tileProgress.setAutoClose(false);
    tileProgress.setAutoReset(false);
    const auto updateTileProgress = [&tileProgress](std::string const &phase, size_t completed, size_t total)
    {
        const size_t maximumValue = static_cast<size_t>(std::numeric_limits<int>::max());
        const int maximum = static_cast<int>(std::min(total, maximumValue));
        const int value = static_cast<int>(std::min(completed, maximumValue));
        tileProgress.setLabelText(QString::fromStdString(phase) + " (" + QString::number(completed) + " of " +
                                  QString::number(total) + ")");
        tileProgress.setRange(0, std::max(1, maximum));
        tileProgress.setValue(std::min(value, std::max(1, maximum)));
        tileProgress.show();
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    };

    try
    {
        D_Tile::load_tiles(tileSetRootPath, updateTileProgress);
        if (generateTilesCheckBox->isChecked())
            D_Tile::generate_tiles(updateTileProgress);
    }
    catch (const std::exception &error)
    {
        tileProgress.close();
        Logger.log(libcpp59::log_level::ERR, error.what());
        QMessageBox::critical(this, "Load Tile Set", error.what());
        return;
    }
    tileProgress.close();

    if (Tile_Map.empty())
    {
        const QString message = "No tiles were loaded from the selected directory.";
        Logger.log(libcpp59::log_level::ERR, message.toStdString());
        QMessageBox::warning(this, "Load Tile Set", message);
        return;
    }

    const std::string defaultTheme = Tile_Map.begin()->second->get_theme();
    Active_Theme_Map = D_Tile::filter_by_theme(defaultTheme);
    QStringList themes;
    themes.reserve(static_cast<qsizetype>(Loaded_Img_Dirs.size()));
    for (const auto &[themeName, loadedDirectory] : Loaded_Img_Dirs)
        themes.append(QString::fromStdString(themeName));
    themes.sort(Qt::CaseInsensitive);

    const QSignalBlocker styleComboBoxSignalBlocker(ui->StyleComboBox);
    ui->StyleComboBox->clear();
    ui->StyleComboBox->addItems(themes);
    ui->StyleComboBox->setCurrentText(QString::fromStdString(defaultTheme));
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
