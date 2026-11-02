#ifndef SCORING_H
#define SCORING_H

// Per-pair scoring, in answer points (see factors/factors.h for the unit):
//
//   score = survey_points() + profile_points()
//
// Profile factors live in factors/, one file each. Dealbreakers live in
// compatibility.cc.

#include <limits>
#include <optional>
#include <string>
#include <unordered_map>

#include "compatibility.h"
#include "types.h"
#include "user.h"

// Per-school survey statistics used by survey_points()
struct College {
    std::string name;
    Matrix3D<float> cosine_sims;
    Matrix2D<float> answer_dist;
    float exp_sim = 0.0;
    float dist_min = std::numeric_limits<float>::max();
};

// Sum of every profile factor's points (factors/registry.cc), or nullopt if
// u1 and u2 can't be matched (match type, class year gap, dealbreakers).
std::optional<float> profile_points(const User* u1, const User* u2,
                                    const float shipval, const float ship_mu,
                                    const float ship_sig);

// How similar u1 and u2's survey answers, bios, prompts and Spotify artists
// are, in answer points scaled to a 12-question survey. Always above 0.
// nullopt if they can't be matched: different schools, or so many identical
// answers that they look coordinated.
std::optional<float> survey_points(
    const User* u1,
    const User* u2,
    const size_t num_questions,
    const std::unordered_map<std::string, College>& college_dists,
    const float bio_score,
    const float short_answer_score,
    const float spotify_score
);

#endif
