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

DBuilderUI::DBuilderUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), graphicsViewScene(new QGraphicsScene(this)),
                                          backgroundItem(nullptr), backgroundImage("imgs/GUI_background/Anara.png")
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

    backgroundItem = graphicsViewScene->addPixmap(backgroundImage);
    ui->graphicsView->setScene(graphicsViewScene);
    ui->graphicsView->setFrameShape(QFrame::NoFrame);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->viewport()->installEventFilter(this);
    QTimer::singleShot(0, this, &DBuilderUI::updateBackgroundImage);
}

DBuilderUI::~DBuilderUI()
{
    delete ui;
}

bool DBuilderUI::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->graphicsView->viewport() && event->type() == QEvent::Resize)
    {
        updateBackgroundImage();
    }
    return QMainWindow::eventFilter(watched, event);
}

void DBuilderUI::updateBackgroundImage()
{
    const QSize viewSize = ui->graphicsView->viewport()->size();
    if (viewSize.isEmpty())
        return;

    const QPixmap scaledBackground = backgroundImage.scaled(viewSize, Qt::KeepAspectRatioByExpanding,
                                                            Qt::SmoothTransformation);
    graphicsViewScene->setSceneRect(0, 0, viewSize.width(), viewSize.height());
    backgroundItem->setPixmap(scaledBackground);
    backgroundItem->setPos((viewSize.width() - scaledBackground.width()) / 2,
                           (viewSize.height() - scaledBackground.height()) / 2);
}

void DBuilderUI::onGenerateButtonClicked()
{
    needs_graphics_view_reset = false;
    //! TODO: Remove overlay from graphics view

    Active_Theme_Map = D_Tile::filter_by_theme(ui->StyleComboBox->currentText().toStdString());
    //! ERROR: there is an error here, the active theme is coming up empty!!!
    Dungeon_Map->generate(ui->ColumnsSpinner->value(),
                          ui->RowsSpinner->value(),
                          ui->PercentConnectionsSpinner->value(),
                          Active_Theme_Map);

    //! TODO: Implementation for updating the graphics view after generation
}
void DBuilderUI::onSaveButtonClicked()
{
    if (needs_graphics_view_reset)
    {
        Logger.log(libcpp59::log_level::INFO, "Graphics view needs to be reset before saving.");
        QMessageBox::information(this, "Info", "Graphics view needs to be reset before saving.");
        return;
    }

    //! TODO: Implementation for the save button click event
}

void DBuilderUI::onLoadTileSetButtonClicked()
{
    needs_graphics_view_reset = true;
    //! TODO: Implementation for the load tile set button click event
}

void DBuilderUI::onPercentConnectionChanged()
{
    needs_graphics_view_reset = true;

    //! TODO: Implementation for the percent connection value change event
}

void DBuilderUI::onNumRowsCChanged()
{
    needs_graphics_view_reset = true;

    //! TODO: Implementation for the number of rows value change event
}

void DBuilderUI::onNumColsChanged()
{
    needs_graphics_view_reset = true;

    //! TODO: Implementation for the number of columns value change event
}

void DBuilderUI::onStyleChanged()
{
    needs_graphics_view_reset = true;

    //! TODO: Implementation for the style change event
}
