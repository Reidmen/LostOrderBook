#ifdef ORDER_LIMIT_HPP
#define ORDER_LIMIT_HPP
#include <algorithm>
#include <list>
#include <memory>

#include "include/utils.hpp"
#include "order.hpp"
namespace OrderBook {

class Order;
class Book;

class OrderLimit {
   private:
    double quantity = 0.0;
    double all_or_nothing_quantity = 0.0;

    // order are stored as double-linked lists for O(1) cancel
    std::list<SharedOrderPtr> orders;

    std::list<std::list<SharedOrderPtr>::iterator> all_or_nothing_iterator;
    std::list<OrderBook::SharedOrderPtr>::iterator insert(ConstOrderPtr &order);

    double simulate_trade(const double quantity) const;
    inline std::size_t get_order_count() const;
    inline double get_quantity() const;
    inline double get_all_or_nothing_quantity() const;

    inline std::list<SharedOrderPtr>::iterator begin();
    inline std::list<SharedOrderPtr>::iterator end();
    inline std::size_t order_count() const;
    inline std::size_t all_or_nothing_order_count() const;

    friend Book;
    friend Order;

    ~OrderLimit();
}

}  // namespace OrderBook

// TODO not completed
OrderBook::OrderLimit::simulate_trade(const double new_quantity) const {
    const double total_quantity = quantity + all_or_nothing_quantity;

    if (new_quantity >= total_quantity) {
        return new_quantity - total_quantity;
    }
};

double OrderBook::OrderLimit::get_quantity() const { return quantity; }
double OrderBook::OrderLimit::get_all_or_nothing_quantity() const {
    return all_or_nothing_quantity;
}

std::size_t OrderBook::OrderLimit::order_count() const { return orders.size(); }
std::size_t OrderBook::OrderLimit::all_or_nothing_order_count() const {
    return all_or_nothing_iterator.size();
}
OrderBook::OrderLimit::~OrderLimit() {
    for (auto &order : orders) {
        order->book = nullptr;
        order->queued = false;
    }
}

#endif
