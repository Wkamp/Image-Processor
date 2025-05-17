#pragma once

#include <vector>
#include <concepts>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <stdexcept>

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <Numeric T>
class Matrix {
private:
    std::vector<T> data;
    size_t rows;
    size_t cols;
    
    void init(size_t rows, size_t cols, T val = 0) {
        if (rows == 0 || cols == 0) {
            throw std::invalid_argument("Matrix dimension cannot be zero");
        }
        this->rows = rows;
        this->cols = cols;
        data.resize(rows * cols, val);
    }
    
    void init(const std::vector<T>& data, size_t rows, size_t cols) {
        if (rows == 0 || cols == 0) {
            throw std::invalid_argument("Matrix dimension cannot be zero");
        }

        if (data.size() != rows * cols) {
            throw std::invalid_argument("Matrix vector size differs from given dimensions");
        }

        this->data = data;
        this->rows = rows;
        this->cols = cols;
    }
    
    void init(const std::vector<std::vector<T>>& elements) {
        size_t tempRows = elements.size();
        if (tempRows == 0) {
            throw std::invalid_argument("Matrix requires at least one row");
        }

        size_t tempCols = elements.begin()->size(); 
        if (tempCols == 0) {
            throw std::invalid_argument("Matrix requires at least one column");
        }

        for (const auto& row : elements) {
            if (row.size() != tempCols) {
                throw std::invalid_argument("All rows must have the same number of columns");
            }
        }
        
        rows = tempRows;
        cols = tempCols;

        data.resize(rows * cols);
        size_t idx = 0;
        for (const auto& row : elements) {
            for (const auto& num : row) {
                data[idx++] = num;
            }
        }
    }

public:
    Matrix(size_t rows, size_t cols, T val = 0) {
        init(rows, cols, val);
    }

    Matrix(const std::vector<std::vector<T>>& elements) {
        init(elements);
    }
    
    Matrix(const std::vector<T>& data, size_t rows, size_t cols) {
        init(data, rows, cols);
    }

    const std::pair<size_t, size_t> dims() const {
        return {rows, cols};
    }
 
    const std::vector<T> getData() const {
        return data;
    }
  
    const T atIdx(size_t idx) const {
        if (idx >= data.size()) {
            throw std::out_of_range("Provided index out-of-bounds");
        }
        return data[idx];
    }
    
    void setIdx(size_t idx, T num) {
        if (idx >= data.size()) {
            throw std::out_of_range("Provided index out-of-bounds");
        }
        data[idx] = num;
    }

    void setMatrix(const std::vector<T>& newData, size_t newRows, size_t newCols) {
        init(newData, newRows, newCols);
    }
    
    void setMatrixVal(T val, size_t m = 0, size_t n = 0) {
        // uses current matrix dims if not specified
        if (m != 0) {
            rows = m;
        }
        if (n != 0) {
            cols = n;
        }
        data.assign(rows * cols, val);
    }
 
    void ones(size_t m = 0, size_t n = 0) {
        setMatrixVal(1, m, n);
    }

    void zeros(size_t m = 0, size_t n = 0) {
        setMatrixVal(0, m, n);
    }

    const T& operator()(size_t row, size_t col) const {
        if (row >= rows || col >= cols) {
            throw std::out_of_range("Matrix indices out of bounds");
        }
        return data[row * cols + col];
    }
    
    T& operator()(size_t row, size_t col) {
        if (row >= rows || col >= cols) {
            throw std::out_of_range("Matrix indices out of bounds");
        }
        return data[row * cols + col];
    }

    Matrix operator+(const Matrix& mat) const {
        if (rows != mat.rows || cols != mat.cols) {
            throw std::invalid_argument("Matrix dimensions must match for addition");
        }

        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < data.size(); i++) {
            result.data[i] = data[i] + mat.data[i];
        }
        
        return result;
    }

    Matrix operator-(const Matrix& mat) const {
        if (rows != mat.rows || cols != mat.cols) {
            throw std::invalid_argument("Matrix dimensions must match for subtraction");
        }

        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < data.size(); i++) {
            result.data[i] = data[i] - mat.data[i];
        }
        
        return result;
    }

    Matrix operator*(const Matrix& mat) const {
        if (cols != mat.rows) {
            throw std::invalid_argument("The columns of the first, and rows of the second must match for multiplication");
        }

        Matrix<T> result(rows, mat.cols);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < mat.cols; j++) {
                for (size_t k = 0; k < cols; k++) {
                    result(i,j) += data[cols * i + k] * mat(k,j);
                }
            }
        }

        return result;
    }

      void print() const {
        if (data.empty()) return;
        
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
