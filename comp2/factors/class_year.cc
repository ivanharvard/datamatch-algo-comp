#include <cstdlib>

#include "../scoring.hpp"
#include "factors.hpp"

// Answer points by how many class years apart two users are. Pairs more than
// 2 years apart are only scored when they have a mutual crush
// (check_compatibility), and those get no class-year points: the mutual crush
// bonus in crush.cc replaces them.
static const double SAME_YEAR_POINTS        =  0.91;
static const double ONE_YEAR_APART_POINTS   =  0.00;
static const double TWO_YEARS_APART_POINTS  = -1.03;

double class_year_factor(const PairContext& pair) {
    if (get_crush_type(pair.u1, pair.u2) == Crush::MUTUAL) {
        return 0;
    }
    const int years_apart = std::abs(pair.u1.year - pair.u2.year);
    if (years_apart == 0) {
        return SAME_YEAR_POINTS;
    } else if (years_apart == 1) {
        return ONE_YEAR_APART_POINTS;
    } else if (years_apart == 2) {
        return TWO_YEARS_APART_POINTS;
    }
    return 0;
}
