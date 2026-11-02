#include <string>
#include <unordered_map>

#include "factors.h"

// Answer points for a Co-Star compatibility of 1.0; a pair gets this times
// its table value below, so real sign pairs get 0.84 (0.27) to 3.06 (0.98).
static const double COSTAR_POINTS_AT_FULL_COMPATIBILITY = 3.12;

// Returns the compatibility (0 to 1) of two zodiac signs. A pair where
// either user has no sign gets 1.0, more than any real sign pair.
float costar_compatibility(std::string costar1, std::string costar2) {
    const float costar_vals[12][12] = {
        {0.45, 0.78, 0.27, 0.68, 0.85, 0.68, 0.9, 0.9, 0.73, 0.58, 0.3, 0.45},
        {0.0, 0.5, 0.42, 0.47, 0.83, 0.97, 0.85, 0.93, 0.5, 0.38, 0.63, 0.67},
        {0.0, 0.0, 0.75, 0.83, 0.65, 0.35, 0.43, 0.53, 0.94, 0.97, 0.9, 0.98},
        {0.0, 0.0, 0.0, 0.75, 0.68, 0.35, 0.55, 0.6, 0.95, 0.98, 0.95, 0.88},
        {0.0, 0.0, 0.0, 0.0, 0.6, 0.88, 0.93, 0.6, 0.28, 0.33, 0.68, 0.53},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.45, 0.97, 0.93, 0.58, 0.73, 0.35, 0.38},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.75, 0.73, 0.35, 0.65, 0.68, 0.88},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.45, 0.28, 0.3, 0.48, 0.63},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.8, 0.88, 0.88, 0.97},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.65, 0.9, 0.85},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.65, 0.88},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.6}};

    std::unordered_map<std::string, int> costar_index = {{"Aquarius", 0}, {"Aries", 1}, {"Cancer", 2},
            {"Capricorn", 3}, {"Gemini", 4}, {"Leo", 5}, {"Libra", 6}, {"Sagittarius", 7},
            {"Scorpio", 8}, {"Taurus", 9}, {"Virgo", 10}, {"Pisces", 11}};
    if (costar1 == "" || costar2 == "") {
        return 1.0;
    }
    if (costar_index[costar1] <= costar_index[costar2]){
        return costar_vals[costar_index[costar1]][costar_index[costar2]];
    }
    else{
        return costar_vals[costar_index[costar2]][costar_index[costar1]];
    }
}

double costar_factor(const PairContext& pair) {
    float compatibility = costar_compatibility(pair.u1.costar, pair.u2.costar);
    return COSTAR_POINTS_AT_FULL_COMPATIBILITY * compatibility;
}
