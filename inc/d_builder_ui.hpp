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

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

class DBuilderUI : public QMainWindow
{
public:
    explicit DBuilderUI(QWidget *parent = nullptr);
    ~DBuilderUI();
    privateSlots : void onGenerateButtonClicked();
    void onSaveButtonClicked();
    void onLoadTileSetButtonClicked();
    void onPercentConnectionChanged();
    void onNumRowsCChanged();
    void onNumColsChanged();
    void onStyleChanged();

private:
    Ui::D_Builder *ui;
};