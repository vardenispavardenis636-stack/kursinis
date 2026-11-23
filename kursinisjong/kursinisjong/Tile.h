#pragma once

#include <string>
#include <vector>

enum class Suit {
    Manzu, // m: 1..9
    Pinzu, // p: 1..9
    Souzu, // s: 1..9
    Honor  // z: 1=East, 2=South, 3=West, 4=North, 5=White, 6=Green, 7=Red
};

struct Tile {
    Suit suit;
    int rank; // 1..9 for m/p/s, 1..7 for z

    std::string toString() const;
    bool isHonor() const;
    bool isTerminalOrHonor() const;
    bool operator==(const Tile& other) const;
};

struct ParseResult {
    bool ok = false;
    std::string error;
    std::vector<Tile> tiles;
};

ParseResult parseTilesInput(const std::string& input);