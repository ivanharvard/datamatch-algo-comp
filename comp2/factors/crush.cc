#include "../scoring.hpp"
#include "factors.hpp"

// Answer points for Crush Roulette crushes. A mutual crush is worth about
// twice the survey similarity of a typical person's best candidates (7.7),
// so it outweighs survey compatibility.
static const double MUTUAL_CRUSH_POINTS  = 16.39;
static const double ONE_WAY_CRUSH_POINTS = 7.06;

double crush_factor(const PairContext& pair) {
    const Crush ctype = get_crush_type(pair.u1, pair.u2);
    if (ctype == Crush::MUTUAL) {
        return MUTUAL_CRUSH_POINTS;
    } else if (ctype == Crush::ONE_WAY) {
        return ONE_WAY_CRUSH_POINTS;
    }
    return 0;
}
