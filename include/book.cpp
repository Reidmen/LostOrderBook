#include "book.hpp"

#include <map>

#include "order.hpp"
#include "utils.hpp"

void Book::begin_order_deferral() { ++order_deferral_depth; }

void Book::end_order_deferral() {
    if (--order_deferral_depth != 0) {
        return;
    }

    while (!deferred.empty()) {
        auto order_ptr = deferred.front();
        deferred.pop();
        insert(order_ptr);
    }
}

void Book::execute_bid(ConstOrderPtr &order) {
    auto limit_iteration = asks.begin();
    double order_price = order->price;

    while (limit_iteration != asks.end() &&
           limit_iteration->first <= order_price && order->quantity > 0.0) {
        if (limit_iteration->second.trade(order) > 0.0) {
            market_price = limit_iteration->first;
        }

        if (limit_iteration->second.is_empty()) {
            asks.erase(limit_iteration++);
        } else {
            ++limit_iteration;
        }
    }

    auto trigger_limit_iteration = ask_triggers.begin();

    while (trigger_limit_iteration != ask_triggers.end() &&
           trigger_limit_iteration->first <= market_price) {
        trigger_limit_iteration->second.trigger_all();
        ++trigger_limit_iteration;
    }

    ask_triggers.erase(ask_triggers.begin(), trigger_limit_iteration);
}

bool Book::ask_is_fillable(ConstOrderPtr &order) const {
    auto limit_iterator = bids.begin();
    double quantity_remaining = order->quantity;
    const double order_price = order->price;

    while (limit_iterator != asks.end() &&
           limit_iterator->first >= order_price && quantity_remaining > 0.0) {
        const double limit_quantity = limit_iterator->second.quantity;
        const double all_or_nothing_quantity =
            limit_iterator->second.all_or_nothing_quantity;
        const double total_limit_quantity =
            limit_quantity + all_or_nothing_quantity;

        if (quantity_remaining >= total_limit_quantity) {
            quantity_remaining -= total_limit_quantity;
        } else if (quantity_remaining <= limit_quantity) {
            return true;
        } else {
            // expensive computation
            quantity_remaining -=
                limit_iterator->second.simulate_trade(quantity_remaining);
        }

        ++limit_iterator;
    }

    return quantity_remaining <= 0.0;
}

void Book::execute_queued_ask(ConstOrderPtr &order) {
    const double quantity = order->quantity;
    execute_ask(order);
    order->limit_iterator->second.quantity -= quantity;
}

void Book::check_asks_all_or_nothing(const double price) {
    auto limit_iter = asks.lower_bound(price);
    while (limit_iter != asks.end()) {
        auto &limit = limit_iter->second;
        auto order_iter = limit.all_or_nothing_iterators.begin();

        while (order_iter != limit.all_or_nothing_iterators.end()) {
            auto order = **order_iter;
            if (ask_is_fillable(order)) {
                execute_queued_ask(order);
            } else {
                ++order_iter;
            }
        }

        if (limit_iter->second.is_empty()) {
            asks.erase(limit_iter++);
        } else {
            ++limit_iter;
        }
    }
}

void Book::queue_bid_order(ConstOrderPtr &order) {
    const auto limit_iterator = bids.emplace(order->price, OrderLimit()).first;
    const auto order_iterator = limit_iterator->second.insert(order);

    order->limit_iterator = limit_iterator;
    order->order_iterator = order_iterator;
    order->queued = true;
    check_asks_all_or_nothing(order->price);
    order->on_queue();
}

bool Book::bid_is_fillable(ConstOrderPtr &order) const {
    auto limit_iterator = asks.begin();
    double quantity_remaining = order->quantity;
    const double order_price = order->price;

    while (limit_iterator != asks.end() &&
           limit_iterator->first <= order_price && quantity_remaining > 0.0) {
        const double limit_quantity = limit_iterator->second.quantity;
        const double all_or_nothing_quantity =
            limit_iterator->second.all_or_nothing_quantity;
        const double total_limit_quantity =
            limit_quantity + all_or_nothing_quantity;

        if (quantity_remaining >= total_limit_quantity) {
            quantity_remaining -= total_limit_quantity;
        } else if (quantity_remaining <= limit_quantity) {
            return true;
        } else {
            // expensive
            quantity_remaining =
                limit_iterator->second.simulate_trade(quantity_remaining);
        }

        ++limit_iterator;
    }

    return quantity_remaining <= 0.0;
}

void Book::insert_all_or_nothing_bid(ConstOrderPtr &order) {
    if (bid_is_fillable(order)) {
        execute_bid(order);
        order->book = nullptr;
        return;
    }

    if (order->immediate_or_cancel) {
        order->on_canceled();
        order->book = nullptr;
        return;
    }

    // queue unexecuted in all-or-nothing order
    queue_bid_order(order);
}

void Book::insert_bid(ConstOrderPtr &order) {
    execute_bid(order);

    if (order->immediate_or_cancel) {
        if (order->quantity > 0.0) {
            order->on_canceled();
        }
        order->book = nullptr;
        return;
    }

    if (order->quantity > 0.0) {
        queue_bid_order(order);
    } else {
        order->book = nullptr;
    }
}

void Book::execute_ask(ConstOrderPtr &order) {
    auto limit_iterator = bids.begin();
    double order_price = order->price;

    while (limit_iterator != bids.end() &&
           limit_iterator->first >= order_price && order->quantity > 0.0) {
        if (limit_iterator->second.trade(order) > 0.0) {
            market_price = limit_iterator->first;
        }

        if (limit_iterator->second.is_empty()) {
            bids.erase(limit_iterator++);
        } else {
            ++limit_iterator;
        }
    }

    auto trigger_limit_iterator = bid_triggers.begin();

    while (trigger_limit_iterator != bid_triggers.end() &&
           trigger_limit_iterator->first >= market_price) {
        trigger_limit_iterator->second.trigger_all();
        ++trigger_limit_iterator;
    }

    bid_triggers.erase(bid_triggers.begin(), trigger_limit_iterator);
}

void Book::execute_queued_bid(ConstOrderPtr &order) {
    const double quantity = order->quantity;
    execute_bid(order);
    order->limit_iterator->second.all_or_nothing_quantity -= quantity;
}

void Book::check_bids_all_or_nothing(const double price) {
    auto limit_iterator = bids.lower_bound(price);

    while (limit_iterator != bids.end()) {
        auto &limit_object = limit_iterator->second;
        auto order_iterator = limit_object.all_or_nothing_iterators.begin();

        while (order_iterator != limit_object.all_or_nothing_iterators.end()) {
            auto order_object = **order_iterator;
            if (bid_is_fillable(order_object)) {
                execute_queued_bid(order_object);
                limit_object.erase(*(order_iterator++));
            } else {
                ++order_iterator;
            }
        }

        if (limit_iterator->second.is_empty()) {
            bids.erase(limit_iterator++);
        } else {
            ++limit_iterator;
        }
    }
}

void Book::queue_ask_order(ConstOrderPtr &order) {
    const auto limit_iterator = asks.emplace(order->price, OrderLimit()).first;
    const auto order_iterator = limit_iterator->second.insert(order);

    order->limit_iterator = limit_iterator;
    order->order_iterator = order_iterator;
    order->queued = true;
    check_bids_all_or_nothing(order->price);
    order->on_queue();
}

void Book::insert_all_or_nothing_ask(ConstOrderPtr &order) {
    if (ask_is_fillable(order)) {
        execute_ask(order);
        order->book = nullptr;
        return;
    }

    if (order->immediate_or_cancel) {
        order->on_canceled();
        order->book = nullptr;
        return;
    }

    // queue unexecuted all-or-nothing order
    queue_ask_order(order);
}

void Book::insert_ask(ConstOrderPtr &order) {
    execute_ask(order);

    if (order->immediate_or_cancel) {
        if (order->quantity > 0.0) {
            order->on_canceled();
        }

        order->book = nullptr;
        return;
    }

    if (order->quantity > 0.0) {
        queue_ask_order(order);
    } else {
        order->book = nullptr;
    }
}

void Book::insert(ConstOrderPtr order) {
    // check if order is valid
    if (order_deferral_depth > 0) {
        deferred.push(order);
        return;
    }
    // begin_order_deferral();

    if (order->quantity <= 0.0) {
        order->on_rejected();
        return;
    }

    if (order->queued) {
        order->on_rejected();
        return;
    }
    // order is valid
    order->book = this;
    order->on_accepted();

    if (order->side == Utils::Side::bid) {
        if (order->all_or_nothing) {
            insert_all_or_nothing_bid(order);
        } else {
            insert_bid(order);
        }
    } else {
        if (order->all_or_nothing) {
            insert_all_or_nothing_ask(order);
        } else {
            insert_ask(order);
        }
    }

    end_order_deferral();
}

double Book::get_bid_price() const {
    const auto iter = bids.begin();
    return iter != bids.end() ? iter->first : Utils::min_price;
}

double Book::get_ask_price() const {
    const auto iter = asks.begin();
    return iter != asks.end() ? iter->first : Utils::max_price;
}

double Book::get_market_price() const { return market_price; }

std::map<double, OrderLimit>::iterator Book::bid_limits_begin() {
    return bids.begin();
}

std::map<double, OrderLimit>::iterator Book::bid_limits_end() {
    return bids.end();
}

std::map<double, OrderLimit>::iterator Book::ask_limits_begin() {
    return asks.begin();
}

std::map<double, OrderLimit>::iterator Book::ask_limits_end() {
    return asks.end();
}

std::map<double, OrderLimit>::iterator Book::bid_limit_at_price(
    const double price) {
    return bids.find(price);
}

std::map<double, OrderLimit>::iterator Book::ask_limit_at_price(
    const double price) {
    return asks.find(price);
}

// TODO ask / big orders begin / end to be implemented!

Book::~Book() {
    bids.clear();
    asks.clear();

    bid_triggers.clear();
    ask_triggers.clear();
}
