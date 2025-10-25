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

void perform_matching(const int target, int** scores,
                      bool** matched, const int ucount);
void make_sortedindices(int** sorted_indices, int** scores, const int ucount);

/// perform_matching works on a small input

int main(void) {
    constexpr int ucount = n;

    // Build scores from A into an Int2D (int** view)
    Int2D scores(ucount);
    for (int i = 0; i < ucount; ++i) {
        for (int j = 0; j < ucount; ++j) {
            scores.ptr()[i][j] = A[i][j];
        }
    }

    // Matched matrix (bool** view), initially all false
    Bool2D matched(ucount);

    // Run matching at capacity = 1
    const int target = 1;
    perform_matching(target, scores.ptr(), matched.ptr(), ucount);

    return 0;
}