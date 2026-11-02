#include <cstdlib>

#include "factors.h"

// Answer points by how far apart two users' "matchCategory" answers are
// (0 to 4, so they can be at most 4 apart). Pairs 3 apart get no points.
static const double SERIOUSNESS_POINTS_BY_DIFFERENCE[] = {
    1.08,   // same answer
    0.73,   // 1 apart
    0.38,   // 2 apart
    0.00,   // 3 apart
    -0.39,  // 4 apart
};

double seriousness_factor(const PairContext& pair) {
    int difference = abs(pair.u1.seriousness - pair.u2.seriousness);
    if (difference > 4) {
        difference = 4;
    }
    return SERIOUSNESS_POINTS_BY_DIFFERENCE[difference];
}
