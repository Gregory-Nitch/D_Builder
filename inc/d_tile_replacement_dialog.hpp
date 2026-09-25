/***********************************************************************************************************************
 * @date 2026-09-25
 * @author Gregory Nitch
 *
 * @brief Dialog used to select an active-theme replacement for a map tile.
 **********************************************************************************************************************/

#pragma once

/*
************************************************************************************************************************
- - System Includes - -
************************************************************************************************************************
*/

#include <memory>
#include <unordered_map>
#include <array>
#include <cstdint>

/*
************************************************************************************************************************
- - 3rd Party Includes - -
************************************************************************************************************************
*/

#include <QDialog>

/*
************************************************************************************************************************
- - Local Includes - -
************************************************************************************************************************
*/

#include "d_tile.hpp"

class QComboBox;
class QCheckBox;
class QGridLayout;
class QGroupBox;

class TileReplacementDialog : public QDialog
{
public:
    /***********************************************************************************************************************
     * @brief Creates a dialog for selecting an active-theme replacement for a tile.
     *
     * @param[in] current_tile Tile being replaced.
     * @param[in] active_theme_tiles Tiles available in the active theme.
     * @param[in] parent Parent widget, if any.
     **********************************************************************************************************************/
    TileReplacementDialog(std::shared_ptr<D_Tile> current_tile,
                          std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> const &active_theme_tiles,
                          QWidget *parent = nullptr);

    /***********************************************************************************************************************
     * @brief Returns the tile selected in the dialog.
     *
     * @retval std::shared_ptr<D_Tile> Selected replacement tile, or nullptr if none was selected.
     **********************************************************************************************************************/
    std::shared_ptr<D_Tile> selectedTile() const;

private:
    std::shared_ptr<D_Tile> current_tile;
    std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> const &active_theme_tiles;
    std::shared_ptr<D_Tile> selected_tile;
    QComboBox *connection_filter = nullptr;
    QGroupBox *custom_mask_editor = nullptr;
    std::array<QCheckBox *, 32> custom_mask_bits{};
    QGridLayout *tile_grid = nullptr;

    /***********************************************************************************************************************
     * @brief Builds a connection bitmap from the custom-mask checkboxes.
     *
     * @retval uint32_t Selected connection bitmap.
     **********************************************************************************************************************/
    uint32_t customConnectionMask() const;

    /***********************************************************************************************************************
     * @brief Enables the custom-mask editor only when that filter is selected.
     **********************************************************************************************************************/
    void updateCustomMaskEditor();

    /***********************************************************************************************************************
     * @brief Rebuilds the selectable tile grid for the active filter.
     **********************************************************************************************************************/
    void rebuildTileGrid();
};