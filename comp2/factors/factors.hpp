#pragma once

// Profile factors, in answer points.
//
// A pair's score is its survey similarity plus the points from every factor
// in profile_factors(). Everything is in one unit, ANSWER POINTS: roughly one
// matching answer on a 12-question survey.
//   - Matching a survey answer earns 1 / (1 + share of people who chose it),
//     so 0.5 to 1.0 points; a common answer earns less.
//   - For scale: on a real Datamatch export, a typical person's 10 best
//     candidates had about 7.7 points of survey similarity.
//
// A factor returns how many points one pair gets. 0 means no effect, and
// negative points are allowed. Only differences between a person's
// candidates change who they are matched with, so adding the same amount to
// every pair changes nothing.
//
// To add a factor:
//   1. Write it in its own file in factors/ (profile.cc is a small example).
//      Put its point values at the top as named constants, with what each
//      one means.
//   2. Declare it below.
//   3. Add one line for it in profile_factors() in factors/registry.cc.

#include <vector>

#include "../user.hpp"

// Everything a factor can look at for one pair of users.
struct PairContext {
    const User& u1;
    const User& u2;
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
double crush_factor(const PairContext& pair);
double rice_purity_factor(const PairContext& pair);
double bio_factor(const PairContext& pair);
double prompts_factor(const PairContext& pair);
double my_factor(const PairContext& pair);   // yours: factors/my_factor.cc

// Lookup helper used by rice_purity_factor
float calculate_rice_purity(float rps1, float rps2) noexcept;  // 0.45 to 1.5, 1.0 = no effect
