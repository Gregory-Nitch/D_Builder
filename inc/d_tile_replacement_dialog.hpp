/***********************************************************************************************************************
 * @date 2026-09-25
 * @author Gregory Nitch
 *
 * @brief Dialog used to select an active-theme replacement for a map tile.
 **********************************************************************************************************************/

#pragma once

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <memory>
#include <unordered_map>

/*
========================================================================================================================
- - 3rd Party Includes - -
========================================================================================================================
*/

#include <QDialog>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_tile.hpp"

class QComboBox;
class QGridLayout;

class TileReplacementDialog : public QDialog
{
public:
    TileReplacementDialog(std::shared_ptr<D_Tile> current_tile,
                          std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> const &active_theme_tiles,
                          QWidget *parent = nullptr);
    std::shared_ptr<D_Tile> selectedTile() const;

private:
    std::shared_ptr<D_Tile> current_tile;
    std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> const &active_theme_tiles;
    std::shared_ptr<D_Tile> selected_tile;
    QComboBox *connection_filter = nullptr;
    QGridLayout *tile_grid = nullptr;

    void rebuildTileGrid();
};