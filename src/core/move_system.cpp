#include "move_system.h"

void moveRun(std::vector<Card>& cards, std::vector<Pile>& piles, const int fromPile, const int toPile, const int startIndex) {
    Pile& source = piles[fromPile];
    Pile& destination = piles[toPile];

    std::vector<int> run;
    for (int i = startIndex; i < source.cardIndices.size(); i++) {
        run.push_back(source.cardIndices[i]);
    }
    source.cardIndices.erase(source.cardIndices.begin() + startIndex, source.cardIndices.end());

    if (!source.cardIndices.empty()) {
        cards[source.cardIndices.back()].faceUp = true;
    }

    static int nextLiftGroupId = 1;
    const int groupId = nextLiftGroupId++;

    for (int const idx : run) {
        destination.cardIndices.push_back(idx);
        cards[idx].pileIndex = toPile;
        cards[idx].isDragging = false;
        cards[idx].isLifted = true;
        cards[idx].liftGroupId = groupId;
        cards[idx].scale = 1.0f;
    }
};
