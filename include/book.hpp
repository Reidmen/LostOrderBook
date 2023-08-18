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

    /*
     * @brief check if the bid order can be filled completely.
     * This check is performec before all-or-nothing order
     * are executed.
     *
     * @param order, the all-or-nothing bid order to be executed
     * @return true if the order is completely fillable
     * @return false if the order is partially fillable
     */
    inline bool bid_is_fillable(ConstOrderPtr order) const;

    /*
     * @brief check if the ask order can be filled completely.
     * This check is performec before all-or-nothing order
     * are executed.
     *
     * @param order, the all-or-nothing ask order to be executed
     * @return true if the order is completely fillable
     * @return false if the order is partially fillable
     */
    inline bool ask_is_fillable(ConstOrderPtr order) const;

    inline void execute_bid(ConstOrderPtr &order);
    inline void execute_ask(ConstOrderPtr &order);

    inline void execute_queued_bid(ConstOrderPtr &order);
    inline void execute_queued_ask(ConstOrderPtr &order);

    inline void queue_bid_order(ConstOrderPtr &order);
    inline void queue_ask_order(ConstOrderPtr &order);

    inline void queue_bid_trigger(ConstTriggerPtr trigger);
    inline void queue_ask_trigger(ConstTriggerPtr trigger);

    /*
     * @brief check if any all-or-nothing bids at the specified
     * price or lower are executable. This function is called
     * if the quantity of queued order is increased.
     *
     * @param price, the price from which queued all-or-nothing
     * will be checked.
     */
    inline void check_bids_all_or_nothing(const double price);

    /*
     * @brief check if any all-or-nothing asks at the specified
     * price or ask are executable. This function is called
     * if the quantity of queued order is increased.
     *
     * @param price, the price from which queued all-or-nothing
     * will be checked.
     */
    inline void check_asks_all_or_nothing(const double price);


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
