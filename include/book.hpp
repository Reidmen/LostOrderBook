#ifndef BOOK_HPP
#define BOOK_HPP

#include <functional>
#include <memory>
#include <ostream>
#include <queue>
#include <utility>

#include "order.hpp"

template <class T, class... Args>
std::shared_ptr<T> insert(Args &&... args) {
    auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
    insert(ptr);
    return ptr;
}

/*
 * @brief operator ostream object to handle orders from stream
 */
std::ostream &operator<<(std::ostream &os, const Book &book);

/*
 * @brief Book implements a price-time-priority matching engine.
 * Orders and Triggers can be inserted into the book object
 */
class Book {
   private:
    /*
     * During execution event handlers like "on_trade" are
     * called to insert additional orders. The orders are
     * deferred. Only once the outer insertion call is
     * completed, the additional ordera executed.
     */
    std::size_t order_deferral_depth = 0;
    std::queue<SharedOrderPtr> deferred;

    std::map<double, OrderLimit, std::greater<double>> bids;
    std::map<double, OrderLimit, std::less<double>> asks;

    std::map<double, TriggerLimit, std::greater<double>> bid_triggers;
    std::map<double, TriggerLimit, std::less<double>> ask_triggers;

    // initialize market price with negative values
    double market_price = Utils::negative_price;

    /*
     * @brief When called, subsequent orders will be deferred
     * rather than queued immediately. This is required
     * to ensure orders are fully executed before new orders
     */
    inline void begin_order_deferral();

    /*
     * @brief Once the outer insertion call is executed,
     * orders from the deferral queue are executed
     */
    inline void end_order_deferral();
    inline void insert_bid(ConstOrderPtr &order);
    inline void insert_ask(ConstOrderPtr &order);

    inline void insert_all_or_nothing_bid(ConstOrderPtr &order);
    inline void insert_all_or_nothing_ask(ConstOrderPtr &order);

   public:
    template <class T, class... Args>
    inline std::shared_ptr<T> insert(Args &&... args);

    inline void insert(std::shared_ptr<Order> order);
    inline void insert(std::shared_ptr<Trigger> trigger);
    inline void insert(const Insertable &insertable);

    /*
     * @brief Get the best bid price
     *
     * @return double
     */
    inline double get_bid_price() const;
    /*
     * @brief Get the best ask price
     *
     * @return double
     */
    inline double get_ask_price() const;
    /*
     * @brief Get the price and which the last trade ocurred
     *
     * @return double the current market price
     */
    inline double get_market_price() const;

    // destructor
    ~Book();

    friend Order;
    friend Trigger;
};

#endif
