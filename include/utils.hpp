#ifndef UTILS_HPP
#define UTILS_HPP

#include <cfloat>
#include <memory>

namespace Utils {
enum Side { bid = 0, ask = 1 };
enum Offset { abs = 0, pct = 1 };

const double max_price = DBL_MAX;
const double min_price = 0.0;
const double negative_price = -DBL_MAX;
}  // namespace Utils

#endif
