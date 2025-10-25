#include <set>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <memory>

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

/// try_propose works on a small input

int main(void) {
    // ----- scores as vector<vector<int>> + int** view
    std::vector<std::vector<int>> scores_vec(n, std::vector<int>(n, -1));

    for (int i=0;i<n;++i) for (int j=0;j<n;++j) scores_vec[i][j] = A[i][j];
    std::vector<int*> scores_rows(n);
    for (int i=0;i<n;++i) scores_rows[i] = scores_vec[i].data();
    int** scores = scores_rows.data();

    // ----- matched as true bool** rows (simple + safe for tests)
    std::vector<bool*> matched_rows(n);
    std::vector<std::vector<bool>> matched_storage(n, std::vector<bool>(n, false));
    // We need bool** with contiguous bool*. std::vector<bool> is special (no .data()),
    // so allocate real bool rows instead:
    std::vector<std::unique_ptr<bool[]>> matched_real(n);
    for (int i=0;i<n;++i) {
        matched_real[i] = std::unique_ptr<bool[]>(new bool[n]{});
        matched_rows[i] = matched_real[i].get();
    }
    bool** matched = matched_rows.data();

    // ----- sorted_indices
    std::vector<std::vector<int>> sorted_indices_vec(n, std::vector<int>(n));
    std::vector<int*> sorted_rows(n);
    for (int i=0;i<n;++i) sorted_rows[i] = sorted_indices_vec[i].data();
    int** sorted_indices = sorted_rows.data();

    // Build the sorted preference indices using your function
    make_sortedindices(sorted_indices, scores, n);

    // ----- proposers’ cursors
    std::vector<int> cur_poss_vec(n, 0);
    int* cur_poss = cur_poss_vec.data();

    // ----- match sets
    std::vector<std::set<int>> match_sets_vec(n);
    std::set<int>* match_sets = match_sets_vec.data();

    // One capacity round (iter = 1): keep letting proposers propose until no change
    const size_t iter = 1;
    bool changed = true;
    while (changed) {
        changed = false;
        for (int p = 0; p < n; ++p) {
            while (try_propose(p, iter, sorted_indices, scores, matched,
                               cur_poss, n, match_sets)) {
                changed = true;
            }
        }
    }

    output(scores, n, match_sets);

    return 0;
}