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

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_builder_ui.hpp"
#include "ui_D_Builder.h" // Generated header for the D_Builder UI

DBuilderUI::DBuilderUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), graphicsViewScene(new QGraphicsScene(this)),
                                          backgroundItem(nullptr), backgroundImage("imgs/GUI_background/Anara.png")
{
    ui->setupUi(this);
    connect(ui->GenerateButton, &QPushButton::clicked, this, &DBuilderUI::onGenerateButtonClicked);
    connect(ui->SaveButton, &QPushButton::clicked, this, &DBuilderUI::onSaveButtonClicked);
    connect(ui->LoadTileSetButton, &QPushButton::clicked, this, &DBuilderUI::onLoadTileSetButtonClicked);
    connect(ui->PercentConnectionsSpinner, &QSpinBox::valueChanged, this, &DBuilderUI::onPercentConnectionChanged);
    connect(ui->RowsSpinner, &QSpinBox::valueChanged, this, &DBuilderUI::onNumRowsCChanged);
    connect(ui->ColumnsSpinner, &QSpinBox::valueChanged, this, &DBuilderUI::onNumColsChanged);
    connect(ui->StyleComboBox, &QComboBox::currentTextChanged, this, &DBuilderUI::onStyleChanged);

    backgroundItem = graphicsViewScene->addPixmap(backgroundImage);
    ui->graphicsView->setScene(graphicsViewScene);
    ui->graphicsView->installEventFilter(this);
    updateBackgroundImage();
}

DBuilderUI::~DBuilderUI()
{
    delete ui;
}

bool DBuilderUI::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->graphicsView && event->type() == QEvent::Resize)
    {
        updateBackgroundImage();
    }
    return QMainWindow::eventFilter(watched, event);
}

void DBuilderUI::updateBackgroundImage()
{
    const QSize viewSize = ui->graphicsView->viewport()->size();
    graphicsViewScene->setSceneRect(0, 0, viewSize.width(), viewSize.height());
    backgroundItem->setPixmap(backgroundImage.scaled(viewSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    backgroundItem->setPos(0, 0);
}

void DBuilderUI::onGenerateButtonClicked()
{
    // TODO: Implementation for the generate button click event
}
void DBuilderUI::onSaveButtonClicked()
{
    // TODO: Implementation for the save button click event
}

void DBuilderUI::onLoadTileSetButtonClicked()
{
    // TODO: Implementation for the load tile set button click event
}

void DBuilderUI::onPercentConnectionChanged()
{
    // TODO: Implementation for the percent connection value change event
}

void DBuilderUI::onNumRowsCChanged()
{
    // TODO: Implementation for the number of rows value change event
}

void DBuilderUI::onNumColsChanged()
{
    // TODO: Implementation for the number of columns value change event
}

void DBuilderUI::onStyleChanged()
{
    // TODO: Implementation for the style change event
}