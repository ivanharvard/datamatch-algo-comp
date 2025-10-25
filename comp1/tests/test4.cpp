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

const int n = 4;
const int A[n][n] = {
    {-1, 10,  3,  2},
    {10, -1,  1,  0},
    { 3,  1, -1,  9},
    { 2,  0,  9, -1}
};

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

/// make_matches works on a small input

int main() {
    constexpr int ucount = n;

    // Build scores from the static matrix A
    Int2D  scores(ucount), sorted(ucount);
    for (int i = 0; i < ucount; ++i) {
        for (int j = 0; j < ucount; ++j) {
            scores.ptr()[i][j] = A[i][j];
        }
    }

    Bool2D matched(ucount);                    // zero-initialized
    std::vector<int> cur_poss(ucount, 0);      // proposal cursors
    std::vector<std::set<int>> match_sets(ucount);

    // Build preference order per proposer
    make_sortedindices(sorted.ptr(), scores.ptr(), ucount);

    // Run matching for capacity = 1
    const int target = 1;
    make_matches(target,
                 sorted.ptr(),
                 scores.ptr(),
                 matched.ptr(),
                 cur_poss.data(),
                 ucount,
                 match_sets.data());

    // --- Invariants for capacity-1 matching ---
    for (int i = 0; i < ucount; ++i) {
        // Capacity respected
        assert(match_sets[i].size() <= 1);

        for (int j : match_sets[i]) {
            // No self matches
            assert(i != j);
            // Symmetry between matrix and sets
            assert(matched.ptr()[i][j] && matched.ptr()[j][i]);
            assert(match_sets[j].count(i) == 1);
            // No forbidden edges selected
            assert(scores.ptr()[i][j] >= 0 && scores.ptr()[j][i] >= 0);
        }
    }

    if (!match_sets[0].empty()) {
        int p = *match_sets[0].begin();
        assert(p == 1);
    }
    if (!match_sets[1].empty()) {
        int p = *match_sets[1].begin();
        assert(p == 0);
    }
    if (!match_sets[2].empty()) {
        int p = *match_sets[2].begin();
        assert(p == 3);
    }
    if (!match_sets[3].empty()) {
        int p = *match_sets[3].begin();
        assert(p == 2);
    }

    output(scores.ptr(), ucount, match_sets.data());
    return 0;
}