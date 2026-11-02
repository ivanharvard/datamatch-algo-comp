#pragma once
#include <vector>
#include <cstddef>

template <typename T>
struct Matrix2D {
    size_t rows = 0, cols = 0;
    std::vector<T> data;

    Matrix2D() = default;
    Matrix2D(size_t r, size_t c) : rows(r), cols(c), data(r * c) {}

    T& operator()(size_t i, size_t j) {
        return data[i * cols + j];
    }

    const T& operator()(size_t i, size_t j) const {
        return data[i * cols + j];
    }
};

template <typename T>
struct Matrix3D {
    size_t dim1 = 0, dim2 = 0, dim3 = 0;
    std::vector<T> data;

    Matrix3D() = default;
    Matrix3D(size_t d1, size_t d2, size_t d3) : dim1(d1), dim2(d2), dim3(d3), data(d1 * d2 * d3) {}

    T& operator()(size_t i, size_t j, size_t k) {
        return data[i * (dim2 * dim3) + j * dim3 + k];
    }

    const T& operator()(size_t i, size_t j, size_t k) const {
        return data[i * (dim2 * dim3) + j * dim3 + k];
    }
};

