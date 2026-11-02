#ifndef FACTORS_H
#define FACTORS_H

// Profile factors, in answer points.
//
// A pair's score is its survey similarity plus the points from every factor
// in profile_factors() (scoring.cc adds them up). Everything is in one unit,
// ANSWER POINTS: roughly one matching answer on a 12-question survey.
//   - Matching a survey answer earns 1 / (1 + share of people who chose it),
//     so 0.5 to 1.0 points; a common answer earns less. Survey similarity
//     also includes partial credit for similar answers, a rare-answer bonus,
//     and the bio, prompt and Spotify similarities (survey_points in
//     scoring.cc).
//   - Survey similarity is scaled to a 12-question survey, so a factor worth
//     +1 point counts the same at every school regardless of survey length.
//   - For scale: on a real export, the median person's 10 best candidates
//     had about 7.7 points of survey similarity.
//
// A factor returns how many points one pair gets. 0 means no effect;
// negative points are allowed. Only differences between a person's
// candidates change who they are matched with, so adding the same amount to
// every pair changes nothing.
//
// To add a factor:
//   1. Write it in its own file in factors/ (see bio_factor in profile.cc
//      for a small example). Put its point values at the top as named
//      constants, with what each one means.
//   2. Declare it below.
//   3. Add one line for it in profile_factors() in factors/registry.cc.
// The Makefile picks up new .cc files automatically.
//
// The point values in these files were converted from the multipliers the
// scoring used before it was in answer points: a factor that multiplied the
// score by m now adds 7.7 * ln(m) points, 7.7 being the typical survey
// similarity above. Near that similarity this ranks candidates the same way
// the multipliers did.

#include <string>
#include <vector>

#include "../user.h"

// Everything a factor can look at for one pair of users.
struct PairContext {
    const User& u1;
    const User& u2;
    float shipval;   // times others submitted u1 and u2 together in Crush Roulette
    float ship_mu;   // mean ship count for the school (see crush_roulette.cc)
    float ship_sig;  // standard deviation of ship counts for the school
};

// Returns the answer points one pair gets from a factor.
using FactorFn = double (*)(const PairContext& pair);

struct ProfileFactor {
    const char* name;
    FactorFn points;
};

// All profile factors. Defined in registry.cc.
const std::vector<ProfileFactor>& profile_factors();

// One function per factor, each in factors/<name>.cc
double class_year_factor(const PairContext& pair);
double mbti_factor(const PairContext& pair);
double costar_factor(const PairContext& pair);
double rice_purity_factor(const PairContext& pair);
double seriousness_factor(const PairContext& pair);
double crush_factor(const PairContext& pair);
double bio_factor(const PairContext& pair);
double prompts_factor(const PairContext& pair);
double ships_factor(const PairContext& pair);

// Lookup helpers used by the factors above
float mbti_compatability(std::string mbti1, std::string mbti2);       // answer points
float costar_compatibility(std::string costar1, std::string costar2); // 0 to 1
float calculate_rice_purity(const float rps1, const float rps2);      // 0.45 to 1.5, 1.0 = no effect

#endif
