#include "factors.hpp"

// Every profile factor. Points are added, so the order doesn't change any
// score.
const std::vector<ProfileFactor>& profile_factors() {
    static const std::vector<ProfileFactor> factors = {
        {"class_year",  class_year_factor},
        {"crush",       crush_factor},
        {"rice_purity", rice_purity_factor},
        {"bio",         bio_factor},
        {"prompts",     prompts_factor},
        // TODO: add a line for your factor (factors/my_factor.cc)
    };
    return factors;
}
