#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "factors.h"

// calculate_rice_purity() returns a scale from 0.45 to 1.5 where 1.0 means
// no effect (either user has no score). Each 0.1 above 1.0 is worth this
// many answer points, and each 0.1 below costs the same.
static const double RICE_PURITY_POINTS_PER_TENTH = 0.624;

// Intended to depend on how far apart the two scores are. Because `1 / 1000`
// below is integer division, alpha is 0 and this returns 1.5 for every pair
// where both users have a score, so the factor is a flat +3.12 points.
float calculate_rice_purity(const float rps1, const float rps2) {
    if (rps1 == -1.0 || rps2 == -1.0) {
        return 1.0;
    }

    float alpha = 1 / 1000; // alpha value
    float beta = std::max(rps1, rps2) / 100;
    float diff = abs(rps1 - rps2);

    float x = pow(diff, 2.4) * alpha;

    float scale = 1.3 - pow(x, beta);

    if (scale < -2.0) {
        scale = -2.0;
    }

    // 3.3 is maximum score - first scale to 0 - 1, then (0.6 - 2) * 0.75 = 0.45 - 1.5
    return ((scale + 2.0) / 3.3 * 1.4 + 0.6) * 0.75;


}

double rice_purity_factor(const PairContext& pair) {
    float scale = calculate_rice_purity(pair.u1.ricepurity, pair.u2.ricepurity);
    return RICE_PURITY_POINTS_PER_TENTH * (scale - 1.0) * 10;
}
