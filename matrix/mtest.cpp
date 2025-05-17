#include <iostream>
#include <vector>
#include <cstdint>
#include "matrix.hpp"

using namespace std;

int main() {
    Matrix<uint8_t> a(3, 3, 1);
    Matrix<uint8_t> b(3, 3, 2);
    Matrix<uint8_t>c = a * b;

    c.print();

}



