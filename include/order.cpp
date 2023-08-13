#include "order.hpp"

/*
 * @brief Order class
 */
Order::Order(const Utils::Side side, const double price, const double quantity,
             const bool immediate_or_cancel, const bool all_or_nothing) {
    this->side = side;
    this->price = price;
    this->quantity = quantity;
    this->immediate_or_cancel = immediate_or_cancel;
    this->all_or_nothing = all_or_nothing;
}

double Order::get_price() const { return price; }
double Order::get_quantity() const { return quantity; }
bool Order::is_immediate_or_cancel() const { return immediate_or_cancel; }
bool Order::is_all_or_nothing() const { return all_or_nothing; }
bool Order::is_queued() const { return queued; }

/*
 * @brief OrderLimit class
 */
double OrderLimit::get_quantity() const { return quantity; }
double OrderLimit::get_all_or_nothing_quantity() const {
    return all_or_nothing_quantity;
}
OrderLimit::~OrderLimit() {
    for (auto &order : orders) {
        order->book = nullptr;
        order->queued = false;
    }
}

std::size_t OrderLimit::order_count() const { return orders.size(); }
std::size_t OrderLimit::all_or_nothing_order_count() const {
    return all_or_nothing_iterator.size();
}

