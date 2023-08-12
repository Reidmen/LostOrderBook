#ifndef COMMON_HPP
#define COMMON_HPP

#include <cfloat>
#include <memory>

namespace OrderBook {
enum Side { bid = 0, ask = 1 };
enum Offset { abs = 0, pct = 1 };

const double max_price = DBL_MAX;
const double min_price = 0.0;

class Order;
using SharedOrderPtr = std::shared_ptr<Order>;
using ConstOrderPtr = const SharedOrderPtr;

class Trigger;
using SharedTriggerPtr = std::shared_ptr<Trigger>;
using ConstTriggerPtr = const SharedTriggerPtr;

}  // namespace OrderBook

#endif
