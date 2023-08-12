#ifdef ORDER_LIMIT_HPP
#define ORDER_LIMIT_HPP
#include <list>
#include <memory>
#include <memory>

#include "include/utils.hpp"
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

    inline std::list<SharedOrderPtr>::iterator begin();
    inline std::list<SharedOrderPtr>::iterator end();
    inline std::size_t order_cound() const;
    inline std::size_t all_or_nothing_order_count() const;

    friend Book;
    friend Order;

    ~OrderLimit();
}

}  // namespace OrderBook

#include <algorithm>

#include "order.hpp"

OrderBook::OrderLimit::simulate_trade(const double new_quantity) const {
    const double total_quantity = quantity + all_or_nothing_quantity;

    if (new_quantity >= total_quantity) {
        return new_quantity - total_quantity;
    }
};

#endif
