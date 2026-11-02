#include <algorithm>
#include <cmath>

#include "factors.hpp"

// calculate_rice_purity() returns a scale from 0.45 to 1.5 where 1.0 means no
// effect (either user has no score). Each 0.1 above 1.0 is worth this many
// answer points, and each 0.1 below costs the same, so this factor runs from
// -3.43 to +3.12 points.
static const double RICE_PURITY_POINTS_PER_TENTH = 0.624;

/// calculate_rice_purity(rps1, rps2):
///    Calculate the rice purity score compatibility between two users.

float calculate_rice_purity(float rps1, float rps2) noexcept {
    if (rps1 < 0.0f || rps2 < 0.0f) {
        return 1.0f;
    }

    const float alpha = 1.0f / 1000.0f;
    const float beta  = std::max(rps1, rps2) / 100.0f;
    const float diff  = std::fabs(rps1 - rps2);

    const float x = std::pow(diff, 2.4f) * alpha;
    float score   = 1.3f - std::pow(x, beta);
    score = std::max(score, 0.0f);

    return ((score + 2.0f) / 3.3f * 1.4f + 0.6f) * 0.75f;
}

double rice_purity_factor(const PairContext& pair) {
    const float scale = calculate_rice_purity(pair.u1.ricepurity, pair.u2.ricepurity);
    return RICE_PURITY_POINTS_PER_TENTH * (scale - 1.0) * 10;
}
