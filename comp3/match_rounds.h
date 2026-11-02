#ifndef MATCH_ROUNDS_H
#define MATCH_ROUNDS_H

// Stage 4: turning scores into matches for one school.
//
// Runs perform_matching() (matching.cc) twice: first for SPONSORED ("can date")
// matches, then for the rest. Then makes sure everyone has SPONSORED sponsored
// matches where possible.

#include <utility>
#include <vector>

#include "inputs.h"
#include "score_matrix.h"
#include "types.h"

struct MatchResults {
    // Every match as (i, j) with i < j
    std::vector<std::pair<size_t, size_t>> matches;
    // matchesperperson[i]: (partner index, boosted score) for each of i's matches
    std::vector<std::vector<std::pair<size_t, long long>>> matchesperperson;
    // Integer match scores; sponsored pairs have SPONSORED_CUTOFF added
    Matrix2D<long long> boostedscores;
    // Boosted score of every match (and of every newly sponsored one), sorted
    // ascending once balance_sponsored_matches() finishes. Used for ratings.
    std::vector<long long> matchscores;
};

// Runs both matching rounds and fills out.matches, out.matchesperperson and
// out.boostedscores. Returns false (after logging) if the scores span too
// wide a range to convert to ints for perform_matching().
bool run_match_rounds(const ScoreMatrices& scored, size_t tucount,
                      MatchResults& out);

// Marks extra matches as sponsored so that everyone has SPONSORED of them
// where possible, and fills out.matchscores (sorted).
void balance_sponsored_matches(const SchoolInput& school, MatchResults& out);

#endif
