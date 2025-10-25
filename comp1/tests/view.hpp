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
    int** ptr() { return rows.data(); }
    int*  data(){ return buf.data();  }
    int&  at(int r,int c){ return buf[r*n + c]; }
};

struct Bool2D {
    int n;
    std::vector<std::unique_ptr<bool[]>> storage;
    std::vector<bool*> rows;
    Bool2D(int n_) : n(n_), storage(n_), rows(n_) {
        for (int i = 0; i < n; ++i) {
            storage[i] = std::unique_ptr<bool[]>(new bool[n_]{}); // zeroed
            rows[i] = storage[i].get();
        }
    }
    bool** ptr(){ return rows.data(); }
};
