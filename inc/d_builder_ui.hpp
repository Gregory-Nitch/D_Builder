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

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

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

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onGenerateButtonClicked();
    void onSaveButtonClicked();
    void onLoadTileSetButtonClicked();
    void onPercentConnectionChanged();
    void onNumRowsCChanged();
    void onNumColsChanged();
    void onStyleChanged();

private:
    void updateBackgroundImage();

    Ui::MainWindow *ui;
    QGraphicsScene *graphicsViewScene;
    QGraphicsPixmapItem *backgroundItem;
    QPixmap backgroundImage;
};