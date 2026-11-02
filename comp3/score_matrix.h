#ifndef SCORE_MATRIX_H
#define SCORE_MATRIX_H

// Stage 3: scoring every pair of users in a school.
//
// The per-pair math lives in scoring.cc (profile_points, survey_points)
// and factors/ (one file per profile factor). This file just runs it over all
// pairs and stores the results in N x N matrices.

#include <cstdint>
#include <string>
#include <unordered_map>

#include "crush_roulette.h"
#include "inputs.h"
#include "scoring.h"
#include "types.h"

// Precomputed text/music similarities between pairs of users
// (written by the Python scripts in scripts/).
struct PairSims {
    Matrix2D<float> bios;
    Matrix2D<float> short_answers;
    Matrix2D<float> spotify_artists;
};

struct ScoreMatrices {
    // eligible(i, j) == 1: i and j may be matched. Kept separate from the
    // scores because scores can be any number, including negative.
    Matrix2D<uint8_t> eligible;
    // profile_points(i, j): sum of all profile factors (only where eligible)
    Matrix2D<float> profile_points;
    // scores(i, j): survey points + profile points (only where eligible)
    Matrix2D<float> scores;
    // matchtypes(i, j): 1 = love, 0 = friendship, -1 = neither
    Matrix2D<int8_t> matchtypes;
};

// Fills out.eligible, out.profile_points and out.matchtypes for every pair.
void compute_profile_points(const SchoolInput& school, const CrushRoulette& crush,
                            ScoreMatrices& out);

// Makes every pair where one person blocked the other ineligible, and sets
// its matchtype to -1. Also fills in users[i].blocklist.
void apply_blocklists(const json& full, SchoolInput& school, ScoreMatrices& out);

// Fills out.scores for every eligible pair. Pairs whose survey answers rule
// them out (see survey_points) become ineligible.
void compute_scores(const SchoolInput& school, const PairSims& sims,
                    const std::unordered_map<std::string, College>& college_dists,
                    ScoreMatrices& out);

#endif
