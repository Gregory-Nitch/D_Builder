#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#define DEFAULT_PATH "../imgs/input"

extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Tile_Map;
extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Entrance_Map;
extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Exit_Map;

