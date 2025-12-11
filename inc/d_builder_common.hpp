#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Tiles_Map;
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Entrances_Map;
std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Exit_Map;

