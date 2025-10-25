#pragma once
#include <vector>
#include <memory>

struct Int2D {
    int n;
    std::vector<int> buf;         // n*n row-major
    std::vector<int*> rows;       // n pointers
    Int2D(int n_) : n(n_), buf(n_*n_), rows(n_) {
        for (int i = 0; i < n; ++i) rows[i] = buf.data() + i*n;
    }
    int**       ptr()       { return rows.data(); }
    int* const* ptr() const { return rows.data(); }
    const int&  at(int r,int c) const { return buf[r*n + c]; }
};

struct Bool2D {
    int n{};
    std::unique_ptr<bool[]> buf;  // n*n row-major, one allocation
    std::vector<bool*> rows;      // n row pointers

    explicit Bool2D(int n_) : n(n_), buf(new bool[n_ * n_]{}), rows(n_) {
        for (int i = 0; i < n; ++i) rows[i] = buf.get() + i * n;
    }

    bool**       ptr()       { return rows.data(); }
    bool* const* ptr() const { return rows.data(); }

    bool& at(int r, int c)       { return rows[r][c]; }
    bool  at(int r, int c) const { return rows[r][c]; }

    void fill(bool v) {
        // bool is trivially copyable; fill is fine
        std::fill(buf.get(), buf.get() + n* n, v);
    }
};

