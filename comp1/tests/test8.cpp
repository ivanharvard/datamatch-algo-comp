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

const int n = 5;
const int A[n][n] = {
    {-1,  6,  3,  9,  9},
    { 3, -1,  1,  1,  2},
    { 9,  3, -1,  7,  4},
    { 4,  1,  5, -1,  4},
    { 5,  9,  4,  6, -1}
};

bool try_propose(const int proposer, 
                 const size_t iter,
                 int** sorted_indices, 
                 int** scores, 
                 bool** matched, 
                 int* cur_poss,
                 const int ucount, 
                 std::set<int>* match_sets);
void make_sortedindices(int** sorted_indices, int** scores, const int ucount);
void output(int** scores, const int ucount, std::set<int>* match_sets);

/// try_propose handles rejections on a small input you can trace by hand
///
/// In the first sweep, person 0 matches with 3. Person 1 then proposes to 0,
/// but 0 prefers 3, so 0 rejects 1. Person 1 should move straight on to their
/// next choice (4) in the same call. Later, 0 and 1 are both dumped for
/// people their partners like more, and have to keep going down their lists.
///
/// Expected final pairs: 0-4 and 2-3.

int main(void) {
    Int2D  scores(n), sorted(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            scores.ptr()[i][j] = A[i][j];

    Bool2D matched(n);
    std::vector<int> cur(n, 0);
    std::vector<std::set<int>> match_sets(n);

    make_sortedindices(sorted.ptr(), scores.ptr(), n);

    // Same loop as test3: each sweep, everyone without a partner gets one call.
    const size_t iter = 1;
    bool changed = true;
    while (changed) {
        changed = false;
        for (int proposer = 0; proposer < n; ++proposer) {
            if (match_sets[proposer].size() < iter &&
                try_propose(proposer, iter, sorted.ptr(), scores.ptr(),
                            matched.ptr(), cur.data(), n, match_sets.data())) {
                changed = true;
            }
        }
    }

    output(scores.ptr(), n, match_sets.data());
    std::printf("Pairs:");
    for (int i = 0; i < n; ++i)
        for (int j : match_sets[i])
            if (i < j) std::printf(" %d-%d", i, j);
    std::printf("\n");
    return 0;
}
