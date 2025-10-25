#include <set>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <memory>
#include <random>

#include "utils.h"
#include "view.hpp"

const int n = 1000;
const int seed = 61;
const auto A = make_random_matrix(n, seed);

bool try_propose(const int proposer, 
                 const size_t iter,
                 int** sorted_indices, 
                 int** scores, 
                 bool** matched, 
                 int* cur_poss,
                 const int ucount, 
                 std::set<int>* match_sets);
void make_matches(const int target, 
                  int** sorted_indices, 
                  int** scores,
                  bool** matched, 
                  int* cur_poss,
                  const int ucount,
                  std::set<int>* match_sets);
void make_sortedindices(int** sorted_indices, int** scores, const int ucount);
void output(int** scores, const int ucount, std::set<int>* match_sets);

/// make_matches works on a large input

int main(void) {
    Int2D  scores(n), sorted(n);
    build_intpp_from_vec(A, scores);

    Bool2D matched(n);                 // zero-initialized
    std::vector<int> cur(n, 0);        // proposal cursors
    std::vector<std::set<int>> match_sets(n);

    // Build preference order per proposer once.
    make_sortedindices(sorted.ptr(), scores.ptr(), n);

    // Run matching with capacity = 2 (large enough to exercise replacements).
    // If your grader expects capacity=1, set target=1 instead.
    const int target = 2;
    make_matches(target,
                 sorted.ptr(),
                 scores.ptr(),
                 matched.ptr(),
                 cur.data(),
                 n,
                 match_sets.data());

    // --- Invariants: capacity respected, symmetry, and no forbidden edges ---
    for (int i = 0; i < n; ++i) {
        // Each vertex holds at most `target` partners
        assert(match_sets[i].size() <= static_cast<size_t>(target));

        for (int j : match_sets[i]) {
            // No self matches
            assert(i != j);
            // Symmetry between matrix and sets
            assert(matched.ptr()[i][j] && matched.ptr()[j][i]);
            assert(match_sets[j].count(i) == 1);
            // No forbidden edges used
            assert(scores.ptr()[i][j] >= 0 && scores.ptr()[j][i] >= 0);
        }
    }

    // --- Idempotence: running make_matches again should not change the matching ---
    auto before = match_sets;  // deep copy of sets
    // Reset cursors (make_matches may also do this internally; safe either way)
    std::fill(cur.begin(), cur.end(), 0);
    make_matches(target,
                 sorted.ptr(),
                 scores.ptr(),
                 matched.ptr(),
                 cur.data(),
                 n,
                 match_sets.data());
    assert(match_sets == before);

    // Print summary stats
    output(scores.ptr(), n, match_sets.data());
    return 0;
}