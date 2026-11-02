#include <cmath>

#include "../utils.h"
#include "factors.h"

// Answer points for pairs that other people submitted together in Crush
// Roulette ("ships"). A pair shipped as often as the school's average shipped
// pair gets 5.34; the most-shipped pairs approach the maximum, and pairs
// shipped once at a school where most shipped pairs get several ships get
// close to 0. Unshipped pairs get nothing.
static const double SHIPS_MAX_POINTS = 8.46;

double ships_factor(const PairContext& pair) {
    if (pair.shipval <= 0) {
        return 0;
    }
    // Share of shipped pairs at this school that were shipped less often
    // (assuming ship counts are roughly normal). If every shipped pair was
    // shipped equally often, this pair is average.
    double share_below = 0.5;
    if (pair.ship_sig > 0) {
        share_below = normal_cdf((pair.shipval - pair.ship_mu) / pair.ship_sig);
    }
    // log(1 + 2x) / log(3) runs from 0 (x = 0) to 1 (x = 1)
    return SHIPS_MAX_POINTS * std::log(1 + 2 * share_below) / std::log(3.0);
}
