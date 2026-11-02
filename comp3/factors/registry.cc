#include "factors.h"

// Points are added, so the order doesn't change any score. It only sets the
// order factors appear in, and what the TESTING log's "before the crush
// factor" total includes (scoring.cc).
const std::vector<ProfileFactor>& profile_factors() {
    static const std::vector<ProfileFactor> factors = {
        {"class_year",  class_year_factor},
        {"mbti",        mbti_factor},
        {"costar",      costar_factor},
        {"rice_purity", rice_purity_factor},
        {"seriousness", seriousness_factor},
        {"crush",       crush_factor},
        {"bio",         bio_factor},
        {"prompts",     prompts_factor},
        {"ships",       ships_factor},
    };
    return factors;
}
