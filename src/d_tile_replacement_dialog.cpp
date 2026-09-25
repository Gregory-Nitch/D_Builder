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
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

constexpr int TILE_CHOOSER_COLUMNS = 4;
constexpr int TILE_THUMBNAIL_SIZE = 144;

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
    layout->addWidget(connection_filter);

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
            { rebuildTileGrid(); });

    rebuildTileGrid();
}

std::shared_ptr<D_Tile> TileReplacementDialog::selectedTile() const
{
    return selected_tile;
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
    const uint32_t current_connections = current_tile->get_connections().mask;

    for (auto const &[id, tile] : active_theme_tiles)
    {
        if (tile == nullptr)
            continue;
        if (require_matching_connections && tile->get_connections().mask != current_connections)
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
        auto *no_matches = new QLabel("No active-theme tiles match this filter.", this);
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