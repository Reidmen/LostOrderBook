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
