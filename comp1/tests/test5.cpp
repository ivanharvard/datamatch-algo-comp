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
        // scores (mutable) + int** view
    std::vector<std::vector<int>> scores_vec(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            scores_vec[i][j] = A[i][j];

    std::vector<int*> scores_rows(n);
    for (int i = 0; i < n; ++i) 
        scores_rows[i] = scores_vec[i].data();
    int** scores = scores_rows.data();

    // matched bool** (real bool rows)
    std::vector<std::unique_ptr<bool[]>> matched_store(n);
    for (int i = 0; i < n; ++i) 
        matched_store[i] = std::unique_ptr<bool[]>(new bool[n]());
    std::vector<bool*> matched_rows(n);
    for (int i = 0; i < n; ++i) 
        matched_rows[i] = matched_store[i].get();
    bool** matched = matched_rows.data();

    // sorted_indices int** 
    std::vector<std::vector<int>> sorted_indices_vec(n, std::vector<int>(n));
    std::vector<int*> sorted_rows(n);
    for (int i = 0; i < n; ++i) 
        sorted_rows[i] = sorted_indices_vec[i].data();
    int** sorted_indices = sorted_rows.data();

    // cursors
    std::vector<int> cur_poss_vec(n, 0);
    int* cur_poss = cur_poss_vec.data();

    // match sets (per target)
    std::vector<std::set<int>> match_sets_vec(n);
    std::set<int>* match_sets = match_sets_vec.data();

    // Build candidate orders
    make_sortedindices(sorted_indices, scores, n);

    const size_t iter = 1;
    bool ch0 = try_propose(0, iter, sorted_indices, scores, matched, cur_poss, n, match_sets);
    match_sets[1].insert(2);
    make_matches(2, sorted_indices, scores, matched, cur_poss, n, match_sets);

    output(scores, n, match_sets);

    return 0;
}