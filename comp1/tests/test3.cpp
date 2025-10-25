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

const int n = 1001; 
const int seed = 42;
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

/// try_propose works on a large, random input

int main() {
    // Generate scores and set up views
    const auto A = make_random_matrix(n, seed);

    Int2D  scores(n), sorted(n);
    build_intpp_from_vec(A, scores);      // fill scores into Int2D

    Bool2D matched(n);                    // zero-initialized bool matrix
    std::vector<int> cur(n, 0);           // proposal cursors per proposer
    std::vector<std::set<int>> match_sets(n);

    // Build preference order per proposer once.
    make_sortedindices(sorted.ptr(), scores.ptr(), n);

    // Run deferred-acceptance with capacity = 1 via try_propose
    const size_t iter = 1;
    bool changed = true;
    while (changed) {
        changed = false;

        // Only proposers who are under capacity may propose this sweep
        for (int proposer = 0; proposer < n; ++proposer) {
            if (match_sets[proposer].size() < iter) {
                if (try_propose(proposer, iter,
                                sorted.ptr(), scores.ptr(), matched.ptr(),
                                cur.data(), n, match_sets.data())) {
                    changed = true;
                }
            }
        }
    }

    // --- Invariants for capacity-1 matching ---
    for (int i = 0; i < n; ++i) {
        // Capacity respected (each node has at most one partner)
        assert(match_sets[i].size() <= 1);

        for (int j : match_sets[i]) {
            // No self-matches
            assert(i != j);

            // Symmetry: matched matrix and sets agree
            assert(matched.ptr()[i][j] && matched.ptr()[j][i]);
            assert(match_sets[j].count(i) == 1);

            // No forbidden edges selected
            assert(scores.ptr()[i][j] >= 0 && scores.ptr()[j][i] >= 0);
        }
    }

    // --- Verify fixed point: another full pass shouldn’t change anything ---
    bool changed2 = false;
    for (int proposer = 0; proposer < n; ++proposer) {
        if (match_sets[proposer].size() < iter &&
            try_propose(proposer, iter,
                        sorted.ptr(), scores.ptr(), matched.ptr(),
                        cur.data(), n, match_sets.data())) {
            changed2 = true;
            break;
        }
    }
    assert(!changed2);

    // Report summary
    output(scores.ptr(), n, match_sets.data());
    return 0;
}