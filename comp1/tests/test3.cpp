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
    Int2D scores(n), sorted(n);
    build_intpp_from_vec(A, scores);              // fill scores
    Bool2D matched(n);                            // zero-initialized
    std::vector<int> cur(n, 0);
    std::vector<std::set<int>> match_sets(n);

    // Build preference order per proposer once.
    make_sortedindices(sorted.ptr(), scores.ptr(), n);

    // Drive matching purely via try_propose at capacity=1.
    const size_t iter = 1;
    bool changed = true;
    while (changed) {
        changed = false;
        for (int proposer = 0; proposer < n; ++proposer) {
            // Let this proposer keep advancing until they either match or exhaust options.
            while (try_propose(proposer, iter,
                               sorted.ptr(), scores.ptr(), matched.ptr(),
                               cur.data(), n, match_sets.data())) {
                changed = true;
            }
        }
    }

    // --- Invariants for capacity-1 matching ---
    for (int i = 0; i < n; ++i) {
        // Capacity respected
        assert(match_sets[i].size() <= 1);

        for (int j : match_sets[i]) {
            // No self matches
            assert(i != j);
            // Symmetry of matched matrix and match_sets
            assert(matched.ptr()[i][j] && matched.ptr()[j][i]);
            assert(match_sets[j].count(i) == 1);
            // No forbidden edges selected
            assert(scores.ptr()[i][j] >= 0 && scores.ptr()[j][i] >= 0);
        }
    }

    // --- Verify fixed point: another full pass shouldn’t change anything ---
    bool changed2 = false;
    for (int proposer = 0; proposer < n; ++proposer) {
        // If any proposer can still make a proposal, we weren't at a fixed point.
        if (try_propose(proposer, iter,
                        sorted.ptr(), scores.ptr(), matched.ptr(),
                        cur.data(), n, match_sets.data())) {
            changed2 = true;
            break;
        }
    }
    assert(!changed2);

    // Report whatever your implementation considers useful
    output(scores.ptr(), n, match_sets.data());
    return 0;
}