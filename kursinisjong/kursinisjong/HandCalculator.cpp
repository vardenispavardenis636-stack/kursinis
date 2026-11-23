#include "HandCalculator.h"

#include <array>

namespace {

    int tileIndex(const Tile& t) {
        int base = static_cast<int>(t.suit) * 9;
        return base + (t.rank - 1);
    }

    bool isKokushiMusou(const std::array<int, 34>& cnt) {
        static const int orphans[13] = { 0, 8, 9, 17, 18, 26, 27, 28, 29, 30, 31, 32, 33 };
        bool hasPair = false;
        for (int idx : orphans) {
            if (cnt[idx] == 0) {
                return false;
            }
            if (cnt[idx] == 2) {
                if (hasPair) {
                    return false;
                }
                hasPair = true;
            }
            else if (cnt[idx] != 1) {
                return false;
            }
        }
        int total = 0;
        for (int c : cnt) total += c;
        return total == 14 && hasPair;
    }

    bool isChiitoitsu(const std::array<int, 34>& cnt) {
        int pairs = 0;
        int total = 0;
        for (int c : cnt) {
            total += c;
            if (c == 2) {
                ++pairs;
            }
            else if (c != 0) {
                return false;
            }
        }
        return total == 14 && pairs == 7;
    }

    bool canFormStandardHand(std::array<int, 34>& cnt, int pairsFound, int meldsFound) {
        int first = -1;
        for (int i = 0; i < 34; ++i) {
            if (cnt[i] > 0) {
                first = i;
                break;
            }
        }
        if (first == -1) {
            return pairsFound == 1 && meldsFound == 4;
        }

        if (pairsFound == 0 && cnt[first] >= 2) {
            cnt[first] -= 2;
            if (canFormStandardHand(cnt, 1, meldsFound)) {
                cnt[first] += 2;
                return true;
            }
            cnt[first] += 2;
        }

        if (cnt[first] >= 3) {
            cnt[first] -= 3;
            if (canFormStandardHand(cnt, pairsFound, meldsFound + 1)) {
                cnt[first] += 3;
                return true;
            }
            cnt[first] += 3;
        }

        int suitBase = (first / 9) * 9;
        int posInSuit = first % 9;
        if (first < 27 && posInSuit <= 6) {
            int i1 = suitBase + posInSuit;
            int i2 = suitBase + posInSuit + 1;
            int i3 = suitBase + posInSuit + 2;
            if (cnt[i1] > 0 && cnt[i2] > 0 && cnt[i3] > 0) {
                cnt[i1]--; cnt[i2]--; cnt[i3]--;
                if (canFormStandardHand(cnt, pairsFound, meldsFound + 1)) {
                    cnt[i1]++; cnt[i2]++; cnt[i3]++;
                    return true;
                }
                cnt[i1]++; cnt[i2]++; cnt[i3]++;
            }
        }

        return false;
    }

    int roundUpToHundred(int val) {
        return ((val + 99) / 100) * 100;
    }

    int computePoints(int han, int fu, bool isDealer, WinType winType) {
        int base = 0;
        if (han >= 13) {
            base = 8000;
        }
        else if (han >= 11) {
            base = 6000;
        }
        else if (han >= 8) {
            base = 4000;
        }
        else if (han >= 6) {
            base = 3000;
        }
        else if (han == 5 || (han == 4 && fu >= 40) || (han == 3 && fu >= 70)) {
            base = 2000;
        }
        else {
            base = fu * (1 << (han + 2));
            if (base > 2000) {
                base = 2000;
            }
        }

        if (!isDealer && winType == WinType::Ron) {
            return roundUpToHundred(base * 4);
        }
        if (isDealer && winType == WinType::Ron) {
            return roundUpToHundred(base * 6);
        }
        if (!isDealer) {
            int nonDealerPay = roundUpToHundred(base);
            int dealerPay = roundUpToHundred(base * 2);
            return dealerPay + 2 * nonDealerPay;
        }
        return roundUpToHundred(base * 2) * 3;
    }

} // namespace

CalculationResult HandCalculator::evaluate(const HandContext& ctx) const {
    CalculationResult res;

    ParseResult parsed = parseTilesInput(ctx.rawTiles);
    if (!parsed.ok) {
        res.error = parsed.error;
        return res;
    }

    if (parsed.tiles.size() != 14) {
        res.error = "Klaida: laiminčioje rankoje turi būti 14 plytelių.";
        return res;
    }

    std::array<int, 34> cnt{};
    for (const auto& t : parsed.tiles) {
        cnt[tileIndex(t)]++;
    }

    if (isKokushiMusou(cnt)) {
        res.valid = true;
        res.yakuList.push_back("Kokushi Musou");
        res.han = 13;
        res.fu = 0;
        res.points = computePoints(res.han, res.fu, ctx.isDealer, ctx.winType);
        return res;
    }

    if (isChiitoitsu(cnt)) {
        res.valid = true;
        res.yakuList.push_back("Chiitoitsu");
        int han = 2;
        if (ctx.isRiichi) {
            res.yakuList.push_back("Riichi");
            han += 1;
        }
        han += ctx.doraCount;
        res.han = han;
        res.fu = 25;
        res.points = computePoints(res.han, res.fu, ctx.isDealer, ctx.winType);
        return res;
    }

    std::array<int, 34> work = cnt;
    if (!canFormStandardHand(work, 0, 0)) {
        res.error = "Klaida: ranka nesudaro leistinos laiminčios kombinacijos.";
        return res;
    }

    int han = 0;
    if (ctx.isRiichi) {
        res.yakuList.push_back("Riichi");
        han += 1;
    }

    bool hasHonor = false;
    for (int i = 27; i < 34; ++i) {
        if (cnt[i] > 0) hasHonor = true;
    }
    if (!hasHonor && !res.yakuList.empty()) {
        res.yakuList.push_back("Pinfu");
        han += 1;
    }

    if (han == 0) {
        res.error = "Klaida: ranka neturi nė vieno yaku (vien dora laimėti negalima).";
        return res;
    }

    han += ctx.doraCount;
    res.valid = true;
    res.han = han;
    res.fu = (ctx.winType == WinType::Ron) ? 30 : 20;
    res.points = computePoints(res.han, res.fu, ctx.isDealer, ctx.winType);
    return res;
}