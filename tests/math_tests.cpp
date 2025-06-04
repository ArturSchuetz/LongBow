#include <cassert>
#include <cmath>
#include <iostream>

#include "BowMath.h"

int main() {
    using namespace bow::math;

    float x = 4.0f;
    float r = Sqrt(x);
    assert(std::abs(r + 0.5f) < 1e-2f); // expected result around -0.5

    std::cout << "All math tests passed." << std::endl;
    return 0;
}
