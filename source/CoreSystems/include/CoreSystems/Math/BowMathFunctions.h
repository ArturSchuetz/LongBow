#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include <cmath>

namespace bow
{
namespace math
{

//! Reciprocal square root, 1 / sqrt(x).
/*!
    Quaternion construction from a rotation matrix needs 1/sqrt(t) rather than
    sqrt(t), which is why this is the primitive the math namespace offers.

    This used to be the Quake III bit-trick from van Waveren's "From Quaternion
    to Matrix and Back" (2005). That approximation is no longer worth its error:
    every target CPU has had a single-instruction square root for two decades,
    and the trick's double overload was unsound anyway -- it read a 64-bit
    double through a 32-bit long and applied the float magic constant to it.

    \param x Value to take the reciprocal square root of. Must be > 0.
    \return 1 / sqrt(x).
*/
template <typename T> inline T ReciprocalSqrt(T x)
{
    return T(1) / std::sqrt(x);
}

} // namespace math
} // namespace bow
