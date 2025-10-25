#pragma once
#include <vector>
#include <random>

#include "view.hpp"

// Build an n×n matrix with seed, diagonal = -1
inline std::vector<std::vector<int>> make_random_matrix(int n, int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, 100);

    std::vector<std::vector<int>> A(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            A[i][j] = (i == j) ? -1 : dist(rng);
    return A;
}

inline void build_intpp_from_vec(const std::vector<std::vector<int>>& src,
                                 Int2D& dst) {
    assert(static_cast<int>(src.size()) == dst.n);
    for (int i = 0; i < dst.n; ++i) {
        assert(static_cast<int>(src[i].size()) == dst.n);
        std::copy(src[i].begin(), src[i].begin() + dst.n, dst.rows[i]); 
    }
}