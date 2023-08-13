#ifndef BOOK_HPP
#define BOOK_HPP

#include <functional>
#include <memory>
#include <queue>

#include "order.hpp"

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
