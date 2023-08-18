#include "book.hpp"

#include "order.hpp"

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


void Book::queue_bid_order(ConstOrderPtr &order) {
    const auto limit_iteration = bids.emplace(order->price, OrderLimit()).first;
    const auto order_iteration = limit_iteration->second.insert(order);

    order->limit_iteration = limit_iteration;
    order->order_iteration = order_iteration;
    order->queued = true;
    check_asks_all_or_nothing(order->price);
    order->on_queue();

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
