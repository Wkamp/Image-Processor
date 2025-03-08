#pragma once

#include <vector>
#include <concepts>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <bits/stdc++.h>
#include <algorithm>

/* TODO:  
 * test overflow detection
 * define wrapping behavior
 * define saturation behavior
 * implement sum
 * implement sub
 * implement multiply
 * */

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <Numeric T>
class Matrix {

private:
  std::vector<T> data;
  size_t rows;
  size_t cols;
  
  OverflowPolicy policy = OverflowPolicy::exception; // enum defined in public
  enum class OverflowStatus {
    noOverFlow;
    overflow;
    underflow;
  }
  
  void init(size_t rows, size_t cols, T val=0) {
    if (rows == 0 || cols == 0) {
      throw std::invalid_argument("Matrix dimension cannot be zero");
    }
    this->rows = rows;
    this->cols = cols;

    if (val = 0) {
      data.resize(rows * cols, val);
    }
  }
  
  void init(std::vector<T> &data, size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
      throw std::invalid_argument("Matrix dimension cannot be zero");
    }

    if (data.size() != rows * cols) {
      throw std::invalid_argument("Matrix vector size differs from given dimensions")
    }

    this->data = data;
    this->rows = rows;
    this->cols = cols;
  }
  
  void init(const std::vector<std::vector<T>> &elements) {
    size_t tempRows = elements.size();
    if (tempRows == 0) {
      throw std::invalid_argument("Matrix requires at least one row");
    }

    size_t tempCols = elements.begin()->size(); 
    if (tempCols == 0) {
      throw std::invalid_argument("Matrix requires at least one column");
    }

    for (const auto &row : elements) {
      if (row.size() != tempCols) {
        throw std::invalid_argument("All rows must have the same number of columns");
      }
    }
    
    rows = tempRows;
    cols = tempCol;

    data.resize(rows * cols);
    size_t idx = 0;
    for (const auto &row : elements) {
      for (const auto &num : row) {
        data[idx++] = num;
      }
    }
  }

  // the following overflow functions check over/under flow

  OverflowStatus overflowAdd(T a, T b) {
    OverflowStatus overflow = OverflowStatus::noOverFlow;

    if (std::is_integral_v<T>) {
      if (b > 0 && a > std::numeric_limits<T>::max() - b) {
        overflow = OverflowStatus::overflow;
      }
      else if (b < 0 && a < std::numeric_limits<T>::min() - b) {
        overflow = OverflowStatus::underflow;
      }
    }
    else if (std::is_floating_point_v<T>) {
      T result = a + b
      if (std::isinf(result)) {
        overflow = OverflowStatus::overflow;
      }
    }

    return overflow;
  }
  
  OverflowStatus overflowSub(T a, T b) {
    OverflowStatus overflow = OverflowStatus::noOverFlow;

    if (std::is_integral_v<T>) {
      if (b > 0 && a > std::numeric_limits<T>::min() + b) {
        overflow = OverflowStatus::overflow;
      }
      else if (b < 0 && a < std::numeric_limits<T>::max() + b) { // underflow
        overflow = OverflowStatus::underflow;
      }

    }
    else if (std::is_floating_point_v<T>) {
      T result = a - b;
      if (std::isinf(result)) {
        overflow = OverflowStatus::overflow;
      }
    }

    return overflow;
  }
  
  OverflowStatus overflowMulti(T a, T b) {
    OverflowStatus overflow = OverflowStatus::noOverFlow;

    if (std::is_integral_v<T>) {
      if (a > 0) {
        if (b > 0 && a > std::numeric_limits<T>::max() / b) {
          overflow = OverflowStatus::overflow;
        }
        else if (b < 0 && b < std::numeric_limits<T>::min() / a) {
          overflow = OverflowStatus::underflow;
        }
      }
      else if (b > 0 && a < std::numeric_limits<T>::min() / b) {
        overflow = OverflowStatus::overflow;
      }
      else if (b < 0 && a < std::numeric_limits<T>::max() / b) {
        overflow = OverflowStatus::underflow;
      }
    }
    else if (std::is_floating_point_v<T>) {
      T result = a * b;
      if (std::isinf(result)) {
        overflow = OverflowStatus::overflow;
      }
    }

    return overflow;
  }
  
  OverflowStatus overflowDiv(T a, T b) {
    OverflowStatus overflow = OverflowStatus::noOverFlow;

    if (std::is_floating_point_v<T>) {
      if (b != 0) {

        T result = a / b;
        if (std::isinf(result)) {
          overflow = OverflowStatus::overflow;
        }
      }
    }
    
    return overflow;
  }


  T wrapOverflowAdd(T a, T b);
  T wrapOverflowSub(T a, T b);
  T wrapOverflowMulti(T a, T b);
  T wrapOverflowDiv(T a, T b);

  T saturateOverflowAdd(T a, T b);
  T saturateOverflowSub(T a, T b);
  T saturateOverflowMulti(T a, T b);
  T saturateOverflowDiv(T a, T b);

public:
  Matrix(size_t rows, size_t cols, T val=0) {
    init(rows, cols, val);
  }

  Matrix(const std::vector<std::vector<T>> &elements) {
    init(elements);
  }
  
  Matrix(std::vector<T> &data, size_t rows, size_t cols) {
    init(data, rows, cols);
  }

  enum class OverflowPolicy {
    exception, // default behavior
    wrap, // wraps negative -> positive, and vice-versa for all types
    saturate // clamps out-of-bounds values to max/min values
  };

  const std::pair<size_t, size_t> dims() const {
    return std::pair {+rows, +cols};
  }
 
  const std::vector getData() const {
    return data;
  }
  
  const T atIdx(T idx) const {
    if (idx >= data.size() || idx < 0) {
      throw std::out_of_range("Provided index out-of-bounds");
    }

    return data[idx];
  }
  
  void setOverflowPolicy(OverflowPolicy policy) {
    this->policy = policy;
  }

  void setIdx(T idx, T num) {
    if (idx >= data.size() || idx < 0) {
      throw std::out_of_range("Provided index out-of-bounds");
    }
    data[idx] = num;
  }

  void setMatrix(std::vector<T> &newData, size_t newRows, size_t newCols) {
    init(newData, newRows, newCols);
  }
  
  // sets all elements in matrix to a certain value, resizing if needed
  void setMatrixVal(T val, size_t m = 0, size_t n = 0) {
    if (m != 0) {
      rows = m;
    }
    if (n != 0) {
      cols = n;
    }
    data.assign(rows * cols, val);
  }
 
  void ones(size_t m = 0, size_t n = 0) {
    if (m != 0) {
      rows = m;
    }
    if (n != 0) {
      cols = n;
    }
    data.assign(rows * cols, 1);
  }

  void zeros(size_t m = 0, size_t n = 0) {
    if (m != 0) {
      rows = m;
    }
    if (n != 0) {
      cols = n;
    }
    data.assign(rows * cols, 0);
  }

  const T &operator()(size_t row, size_t col) const {
    return data[row * cols + col];
  }
  
  T &operator()(size_t row, size_t col) {
    return data[row * cols + col];
  }

  Matrix operator+(Matrix mat) {
    std::pair<size_t, size_t> matDims = mat.dims();
    if (data.size() != (matDims.first * matDims.second) {
      throw std::invalid_argument("Matrix dimensions must match for addition");
    }

    // for oveflow checking
    T max1 = *std::max_element(data.begin(), data.end());
    T min1 = *std::min_element(data.begin(), data.end());
    T max2 = *std::max_element(mat.begin(), mat.end());
    T min2 = *std::min_element(mat.begin(), mat.end());
    
    Matrix<T> result(rows, cols);

    // if no overflow in max/min, then no additional check is needed
    if (overflowAdd(max1, max2) || overflowAdd(min1, min2)) { 
      switch (policy) {
        case OverflowPolicy::exception:
          throw std::overflow_error("over/under flow in matrix sum");
          break;

        case OverflowPolicy::wrap:
          for (size_t i = 0; i < data.size(); i++) {
            T operationResult;;; 
            result.setIdx(i, operationResult);
          }

          break;
        case OverflowPolicy::saturate:
          for (size_t i = 0; i < data.size(); i++) {
            result.setIdx(i, data[i] + mat.atIdx(i));
          }
        break;
      }
    }
    else {
      for (size_t i = 0; i < data.size(); i++) {
        result.setIdx(i, data[i] + mat.atIdx(i));
      }
    }
    
    return result;
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
