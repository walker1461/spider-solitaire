#include "run_rules.h"

bool isValidRun(const std::vector<Card>& cards, const Pile& pile, const int startIndex) {
    const auto& indices = pile.cardIndices;

    for (int i = startIndex; i < indices.size() - 1; i++) {
        const Card& a = cards[indices[i]];
        const Card& b = cards[indices[i + 1]];

        if (!a.faceUp || !b.faceUp) return false;
        if (a.suit != b.suit) return false;
        if (a.rank != b.rank + 1) return false;
    }
    return true;
}

// find the longest run from the top, to darken cards that are trapped beneath
int findTopRun(const std::vector<Card>& cards, const Pile& pile) {
    const auto& indices = pile.cardIndices;
    if (indices.empty()) return -1;
    int i = static_cast<int>(indices.size()) - 1;
    if (!cards[indices[i]].faceUp) return -1;

    while (i > 0) {
        const Card& a = cards[indices[i]];
        const Card& b = cards[indices[i - 1]];

        if (!b.faceUp) break;
        if (a.suit != b.suit) break;
        if (b.rank != a.rank + 1) break;

        i--;
    }
    return i;
}