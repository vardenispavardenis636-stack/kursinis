#pragma once

#include "Tile.h"
#include <string>
#include <vector>

enum class WinType {
    Ron,
    Tsumo
};

struct HandContext {
    std::string rawTiles;
    WinType winType = WinType::Ron;
    bool isRiichi = false;
    bool isDealer = false;
    int doraCount = 0;
};

struct CalculationResult {
    bool valid = false;
    std::string error;
    std::vector<std::string> yakuList;
    int han = 0;
    int fu = 0;
    int points = 0;
};

class HandCalculator {
public:
    CalculationResult evaluate(const HandContext& ctx) const;
};