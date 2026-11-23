#include "Tile.h"

#include <cctype>
#include <sstream>

namespace {

    bool charToSuit(char c, Suit& outSuit) {
        switch (c) {
        case 'm': outSuit = Suit::Manzu; return true;
        case 'p': outSuit = Suit::Pinzu; return true;
        case 's': outSuit = Suit::Souzu; return true;
        case 'z': outSuit = Suit::Honor; return true;
        default: return false;
        }
    }

    char suitToChar(Suit suit) {
        switch (suit) {
        case Suit::Manzu: return 'm';
        case Suit::Pinzu: return 'p';
        case Suit::Souzu: return 's';
        case Suit::Honor: return 'z';
        }
        return '?';
    }

    int maxRankForSuit(Suit suit) {
        return (suit == Suit::Honor) ? 7 : 9;
    }

} // namespace

std::string Tile::toString() const {
    return std::to_string(rank) + suitToChar(suit);
}

bool Tile::isHonor() const {
    return suit == Suit::Honor;
}

bool Tile::isTerminalOrHonor() const {
    if (suit == Suit::Honor) {
        return true;
    }
    return rank == 1 || rank == 9;
}

bool Tile::operator==(const Tile& other) const {
    return suit == other.suit && rank == other.rank;
}

ParseResult parseTilesInput(const std::string& input) {
    ParseResult result;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token) {
        if (token.size() < 2) {
            result.error = "Klaida: netinkamas plytelių formatas („" + token + "“).";
            return result;
        }

        char suitChar = token.back();
        Suit suit;
        if (!charToSuit(suitChar, suit)) {
            result.error = std::string("Klaida: nežinoma plytelių rūšis „") + suitChar + "“.";
            return result;
        }

        int maxRank = maxRankForSuit(suit);
        for (std::size_t i = 0; i + 1 < token.size(); ++i) {
            char ch = token[i];
            if (!std::isdigit(static_cast<unsigned char>(ch))) {
                result.error = "Klaida: tikimasi skaičiaus prieš rūšį („" + token + "“).";
                return result;
            }
            int rank = ch - '0';
            if (rank < 1 || rank > maxRank) {
                result.error = "Klaida: netinkamas plytelės numeris „" + std::to_string(rank) +
                    suitToChar(suit) + "“ (leistina 1.." + std::to_string(maxRank) + ").";
                return result;
            }
            result.tiles.push_back(Tile{ suit, rank });
        }
    }

    if (result.tiles.empty()) {
        result.error = "Klaida: neįvesta nė viena plytelė.";
        return result;
    }

    int counts[4][10] = {};
    for (const auto& t : result.tiles) {
        int sIdx = static_cast<int>(t.suit);
        counts[sIdx][t.rank]++;
        if (counts[sIdx][t.rank] > 4) {
            result.error = "Klaida: per daug vienodų plytelių („" + t.toString() + "“ > 4).";
            return result;
        }
    }

    result.ok = true;
    return result;
}