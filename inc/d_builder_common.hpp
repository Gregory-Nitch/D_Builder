#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <format>

#define DEFAULT_PATH "../imgs/input"

#define ERR_FORMAT(msg) \
    std::format("ERR:{}:{}:{}: {}", __FILE__, __func__, __LINE__, msg)

#define LOG_DEBUG(msg) \
    std::cout << "INFO:" << __func__ << ":" << __LINE__ << ":" << msg << std::endl

extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Tile_Map;
extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Entrance_Map;
extern std::unordered_map<uint64_t, std::shared_ptr<D_Tile>> Exit_Map;
