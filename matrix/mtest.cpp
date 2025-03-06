#include <iostream>
#include <concepts>
#include <vector>
#include <cstdint>
#include "matrix.hpp"

int main() {
  Matrix<uint8_t> a({{1, 0}, {0, 1}});
  Matrix<uint8_t> b({{1, 0}, {0, 1}});
  Matrix<uint8_t> c = a + b;
  c.print();
}
