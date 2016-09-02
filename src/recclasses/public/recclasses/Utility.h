#ifndef RECCLASSES_UTILITY_H
#define RECCLASSES_UTILITY_H

#include <cmath>

/// a and b are considered equal, if both a NaN
inline bool nan_aware_equality(float a, float b)
{ return (a == b) || (std::isnan(a) && std::isnan(b)); }

/// a and b are considered equal, if both a NaN
inline bool nan_aware_equality(double a, double b)
{ return (a == b) || (std::isnan(a) && std::isnan(b)); }

#endif
