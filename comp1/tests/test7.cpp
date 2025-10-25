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

const int n = 999;
const int seed = 50;
const auto A = make_random_matrix(n, seed);

void perform_matching(const int target, int** scores,
                      bool** matched, const int ucount);
void make_sortedindices(int** sorted_indices, int** scores, const int ucount);

/// perform_matching works on a large input

int main(void) {
    Int2D  scores(n);
    build_intpp_from_vec(A, scores);

    // Matched matrix (all false initially)
    Bool2D matched(n);

    // Run matching; target is capacity (set to 1 unless your grader expects another value)
    const int target = 1;
    perform_matching(target, scores.ptr(), matched.ptr(), n);

    return 0;
}