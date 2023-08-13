/*
 * Order header defines the following objects:
 *  - Order
 *  - OrderLimit
 *  - Trigger
 *  - TriggerLimit
 * as well as useful classes
 *  - Insertable
 *  - Stop
 */

#ifndef ORDER_HPP
#define ORDER_HPP

#include <list>
#include <map>

#include "utils.hpp"

class TriggerLimit;
class OrderLimit;
class Book;

/*
 * @brief Order class
 */
class Order : public std::enable_shared_from_this<Order> {
   private:
    Utils::Side side;
    double price;
    double quantity;
    bool immediate_or_cancel = false;
    bool all_or_nothing = false;
    bool queued = false;
    Book *book = nullptr;

    // iterators to allocate order in book, cancel O(1)
    std::map<double, OrderLimit>::iterator limitIterator;
    std::list<Utils::SharedOrderPtr>::iterator orderIterator;

   protected:
    virtual void on_accepted();
    virtual void on_queue();
    virtual void on_rejected();
    virtual void on_traded(Utils::ConstOrderPtr &other_order){};
    virtual void on_canceled(){};

   public:
    // @brief Constructor
    Order(const Utils::Side side, const double price, const double quantity,
          const bool immediate_or_cancel = false,
          const bool all_or_nothing = false);

    inline bool cancel();
    /*
     * @brief get instance of book into which the order was inserted.
     * @return book* pointer to the book object or nullptr
     */
    inline Book *get_book() const;
    inline Utils::Side get_side() const;
    inline double get_price() const;
    inline double get_quantity() const;
    inline void set_quantity(const double quantity);  // O(1) at best
    inline bool is_immediate_or_cancel() const;
    inline bool is_all_or_nothing() const;
    inline void set_all_or_nothing(const bool flag_all_or_nothing);
    inline bool is_queued() const;

    friend Book;
    friend OrderLimit;
};

class OrderLimit {
   private:
    double quantity = 0.0;
    double all_or_nothing_quantity = 0.0;

    // order are stored as double-linked lists for O(1) cancel
    std::list<Utils::SharedOrderPtr> orders;

    std::list<std::list<Utils::SharedOrderPtr>::iterator>
        all_or_nothing_iterator;
    std::list<Utils::SharedOrderPtr>::iterator insert(
        Utils::ConstOrderPtr &order);

    double simulate_trade(const double quantity) const;
    inline std::size_t get_order_count() const;
    inline double get_quantity() const;
    inline double get_all_or_nothing_quantity() const;

    inline std::list<Utils::SharedOrderPtr>::iterator begin();
    inline std::list<Utils::SharedOrderPtr>::iterator end();
    inline std::size_t order_count() const;
    inline std::size_t all_or_nothing_order_count() const;

    friend Order;
    friend Book;

    ~OrderLimit();
};

/*
 * @brief Object of class Trigger is an event handler
 * that gets triggered once the market price rices
 * or falls a specified price.
 * Triggers inserted on the bid side respond falling prices,
 * whereas triggers inserted on the ask side respond
 * to rising prices.
 */
class Trigger : public std::enable_shared_from_this<Trigger> {
   private:
    const Utils::Side side;
    double price;
    bool queued = false;
    Book *book = nullptr;

   protected:
    virtual void on_accepted();
    virtual void on_queued();
    virtual void on_rejected();
    virtual void on_triggered();
    virtual void on_canceled();

   public:
    inline double get_price() const;
    inline void set_price(double new_price);
    inline Utils::Side get_side() const;

    Trigger(Utils::Side side, double price);

    inline Book *get_book();
    inline bool cancel();
    inline bool is_queued() const;

    friend Book;
    friend TriggerLimit;
};

#endif
