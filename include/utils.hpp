#ifndef COMMON_HPP
#define COMMON_HPP

#include <cfloat>
#include <memory>

namespace OrderBook {
    enum side { bid = 0, ask = 1};
    enum offset{ abs = 0, pct = 1};

    const double max_price = DBL_MAX;
    const double min_price = 0.0;

    class Order;
    using shared_order_ptr = std::shared_ptr<Order>;
    using const_order_ptr = const shared_order_ptr;

    class Trigger;
    using shared_trigger_ptr = std::shared_ptr<Trigger>;
    using const_trigger_ptr = const shared_trigger_ptr;

}

#endif
