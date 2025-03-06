#pragma once

#include <vector>
#include <concepts>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <bits/stdc++.h>

template <std::unsigned_integral T>
class Matrix {

private:
  std::vector<T> data;
  size_t rows;
  size_t cols;

public:
  Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
    if (rows == 0 || cols == 0) {
      throw std::invalid_argument("Matrix dimension cannot be zero");
    }
    data.resize(rows * cols, 0); // default matrix is all 0's
  }

  Matrix(const std::vector<std::vector<T>> &elements) {
    rows = elements.size();
    if (rows == 0) {
      throw std::invalid_argument("Matrix requires at least one row");
    }

    cols = elements.begin()->size();
    if (cols == 0) {
      throw std::invalid_argument("Matrix requires at least one column");
    }

    for (const auto &row : elements) {
      if (row.size() != cols) {
        throw std::invalid_argument("All rows must have the same number of columns");
      }
    }

    data.resize(rows * cols);
    size_t idx = 0;
    for (const auto &row : elements) {
      for (const auto &num : row) {
        data[idx++] = num;
      }
    }
  }

  const size_t getRows() const {
    return rows;
  }
  
  const size_t getCols() const {
    return cols;
  }

  const T atIdx(T idx) const {
    if (idx >= data.size()) {
      throw std::out_of_range("Index provided out-of-bounds");
    }

    return data[idx];
  }

  void setIdx(T idx, T num) {
    if (idx >= data.size()) {
      throw std::out_of_range("Index provided out-of-bounds");
    }
    data[idx] = num;
  }

  const T &operator()(size_t row, size_t col) const {
    return data[row * cols + col];
  }
  
  T &operator()(size_t row, size_t col) {
    return data[row * cols + col];
  }

  Matrix operator+(Matrix mat) {
    if (data.size() != (mat.getRows() * mat.getCols())) {
      throw std::invalid_argument("Matrix dimensions must match for addition");
    }

    Matrix<T> result(rows, cols);

    for (size_t i = 0; i < data.size(); i++) {
      if (data[i] > std::numeric_limits<T>::max() - mat.atIdx(i)) {
        throw std::overflow_error("Matrix sum caused element overflow");
      }

      result.setIdx(i, data[i] + mat.atIdx(i));
    }

    return result;
  }

  std::pair<size_t, size_t> dims() {
    return std::pair {+rows, +cols};
  }

  // sets all elements in matrix to a certain value, resizing if needed
  void setMatrix(T val, size_t m = 0, size_t n = 0) {
    if (m != 0) {
      rows = m;
    }
    if (n != 0) {
      cols = n;
    }
    data.assign(rows * cols, val);
  }
  
  void print() const {
    T printWidth = std::to_string(*std::max_element(data.begin(), data.end())).size() + 1;
    for (size_t i = 0; i < data.size(); i++) {
      if (i % cols == 0 && i != 0) {
        std::cout << "\n";
      }
      std::cout << std::setw(printWidth) << +data[i];
    }
    std::cout << std::endl;
  }

};
