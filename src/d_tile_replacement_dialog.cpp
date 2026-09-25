/***********************************************************************************************************************
 * @date 2026-09-25
 * @author Gregory Nitch
 *
 * @brief Tile replacement dialog implementation.
 **********************************************************************************************************************/

/*
========================================================================================================================
- - System Includes - -
========================================================================================================================
*/

#include <algorithm>
#include <vector>

/*
========================================================================================================================
- - Local Includes - -
========================================================================================================================
*/

#include "d_tile_replacement_dialog.hpp"

/*
========================================================================================================================
- - 3rd Party Includes - -
========================================================================================================================
*/

#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

constexpr int TILE_CHOOSER_COLUMNS = 4;
constexpr int TILE_THUMBNAIL_SIZE = 144;
constexpr int CONNECTION_SIDES = 4;
constexpr int CONNECTIONS_PER_SIDE = 8;

TileReplacementDialog::TileReplacementDialog(
    std::shared_ptr<D_Tile> current_tile,
    std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> const &active_theme_tiles,
    QWidget *parent)
    : QDialog(parent),
      current_tile(std::move(current_tile)),
      active_theme_tiles(active_theme_tiles)
{
    setWindowTitle("Replace Tile");
    resize(760, 620);

    auto *layout = new QVBoxLayout(this);
    connection_filter = new QComboBox(this);
    connection_filter->addItem("Matching connections");
    connection_filter->addItem("All active-theme tiles");
    connection_filter->addItem("Custom mask");
    layout->addWidget(connection_filter);

    custom_mask_editor = new QGroupBox("Custom connection mask", this);
    auto *custom_mask_layout = new QGridLayout(custom_mask_editor);
    constexpr std::array<char const *, CONNECTION_SIDES> side_labels = {"Top", "Right", "Bottom", "Left"};
    for (int side = 0; side < CONNECTION_SIDES; ++side)
    {
        custom_mask_layout->addWidget(new QLabel(side_labels[side], custom_mask_editor), side, 0);
        for (int connection = 0; connection < CONNECTIONS_PER_SIDE; ++connection)
        {
            const int bit_index = (side * CONNECTIONS_PER_SIDE) + connection;
            auto *bit_checkbox = new QCheckBox(QString::number(connection), custom_mask_editor);
            custom_mask_bits.at(static_cast<std::size_t>(bit_index)) = bit_checkbox;
            custom_mask_layout->addWidget(bit_checkbox, side, connection + 1);
            connect(bit_checkbox, &QCheckBox::toggled, this, [this](bool)
                    {
                        if (connection_filter->currentIndex() == 2)
                            rebuildTileGrid(); });
        }
    }
    layout->addWidget(custom_mask_editor);

    auto *scroll_area = new QScrollArea(this);
    scroll_area->setWidgetResizable(true);
    auto *tile_container = new QWidget(scroll_area);
    tile_grid = new QGridLayout(tile_container);
    tile_grid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    scroll_area->setWidget(tile_container);
    layout->addWidget(scroll_area);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    connect(connection_filter,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this,
            [this](int)
            {
                updateCustomMaskEditor();
                rebuildTileGrid();
            });

    updateCustomMaskEditor();
    rebuildTileGrid();
}

std::shared_ptr<D_Tile> TileReplacementDialog::selectedTile() const
{
    return selected_tile;
}

uint32_t TileReplacementDialog::customConnectionMask() const
{
    uint32_t mask = CONNECTION_ZERO_MASK;
    for (std::size_t bit_index = 0; bit_index < custom_mask_bits.size(); ++bit_index)
    {
        if (custom_mask_bits.at(bit_index)->isChecked())
            mask |= (uint32_t{1} << bit_index);
    }
    return mask;
}

void TileReplacementDialog::updateCustomMaskEditor()
{
    custom_mask_editor->setEnabled(connection_filter->currentIndex() == 2);
}

void TileReplacementDialog::rebuildTileGrid()
{
    while (QLayoutItem *item = tile_grid->takeAt(0))
    {
        delete item->widget();
        delete item;
    }

    std::vector<std::shared_ptr<D_Tile>> candidates;
    candidates.reserve(active_theme_tiles.size());
    const bool require_matching_connections = connection_filter->currentIndex() == 0;
    const bool require_custom_connections = connection_filter->currentIndex() == 2;
    const uint32_t required_connections = require_custom_connections
                                              ? customConnectionMask()
                                              : current_tile->get_connections().mask;

    for (auto const &[id, tile] : active_theme_tiles)
    {
        if (tile == nullptr)
            continue;
        if ((require_matching_connections || require_custom_connections) &&
            tile->get_connections().mask != required_connections)
            continue;

        candidates.push_back(tile);
    }

    std::sort(candidates.begin(), candidates.end(), [](std::shared_ptr<D_Tile> const &left, std::shared_ptr<D_Tile> const &right)
              {
                  if (left->get_name() == right->get_name())
                      return left->get_id() < right->get_id();
                  return left->get_name() < right->get_name(); });

    if (candidates.empty())
    {
        auto *no_matches = new QLabel("No active-theme tiles match this filter.", tile_grid->parentWidget());
        no_matches->setAlignment(Qt::AlignCenter);
        tile_grid->addWidget(no_matches, 0, 0);
        return;
    }

    for (std::size_t index = 0; index < candidates.size(); ++index)
    {
        std::shared_ptr<D_Tile> const &tile = candidates[index];
        const QPixmap thumbnail = QPixmap::fromImage(*tile->get_image()).scaled(TILE_THUMBNAIL_SIZE, TILE_THUMBNAIL_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        auto *tile_button = new QPushButton(tile_grid->parentWidget());
        tile_button->setFixedSize(TILE_THUMBNAIL_SIZE + 16, TILE_THUMBNAIL_SIZE + 16);
        tile_button->setIcon(QIcon(thumbnail));
        tile_button->setIconSize(thumbnail.size());
        tile_button->setToolTip(QString::fromStdString(tile->get_name()));
        connect(tile_button, &QPushButton::clicked, this, [this, tile]
                {
                    selected_tile = tile;
                    accept(); });

        const int row = static_cast<int>(index / TILE_CHOOSER_COLUMNS);
        const int column = static_cast<int>(index % TILE_CHOOSER_COLUMNS);
        tile_grid->addWidget(tile_button, row, column);
    }
}