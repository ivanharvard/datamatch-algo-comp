#include "../compatibility.h"
#include "factors.h"

// Answer points for Crush Roulette crushes. A mutual crush is worth about
// twice the survey similarity of a typical person's best candidates (7.7),
// so it outweighs survey compatibility. Mutual crushes also skip the
// class-year gap and dealbreaker checks (check_compatability).
// should MUTUAL be automatic match? discuss (see slack complaint from Alex)
static const double MUTUAL_CRUSH_POINTS  = 16.39;
static const double ONE_WAY_CRUSH_POINTS = 7.06;

double crush_factor(const PairContext& pair) {
    Crush ctype = getcrushtype(&pair.u1, &pair.u2);
    if (ctype == MUTUAL) {
        return MUTUAL_CRUSH_POINTS;
    } else if (ctype == ONE_WAY) {
        return ONE_WAY_CRUSH_POINTS;
    }
    return 0;
}
