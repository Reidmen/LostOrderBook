#ifdef ORDER_HPP
#define ORDER_HPP

#include <list>
#include <map>
#include <memory>

#include "utils.hpp"

namespace OrderBook {

class OrderLimit;
class Book;

/*
 * @brief Order class
 *
 */
class Order : public std::enable_sharing_from_this<Order> {
   private:
    const Side side;
    const double price;
    double quantity;
    const bool immediate_or_cancel = false;
    bool all_or_nothing = false;
    bool queued = false;
    Book *book = nullptr;

    // iterators to allocate order in book, cancel O(1)
    std::map<double, OrderBook::OrderLimit>::iterator limitIterator;
    std::list<order_ptr>::iterator orderIterator;

   protected:
    virtual void on_accepted();
    virtual void on_queue();
    virtual void on_rejected();
    virtual void on_traded(ConstTriggerPtr &other_order){};
    virtual void on_canceled(){};

   public:
    // @brief Constructor
    Order(const Side side, const double price, const double quantity,
          const bool immediate_or_cancel = false,
          cost bool all_or_nothing = false);

    inline bool cancel();
    /* @brief get instance of book into which the order was inserted.
     *
     * @return book* pointer to the book object or nullptr
     */
    inline Book *get_book() const;
    inline Side get_side() const;
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

}  // namespace OrderBook

OrderBook::Order::Order(const OrderBook::Side side, const double price,
                        const double quantity, const bool immediate_or_cancel,
                        const bool all_or_nothing) {
    this->side = side;
    this->price = price;
    this->quantity = quantity;
    this->immediate_or_cancel = immediate_or_cancel;
    this->all_or_nothing = all_or_nothing;
}

double OrderBook::Order::get_price() const {return price};
double OrderBook::Order::get_quantity() const {return quantity};
double OrderBook::Order::is_immediate_or_cancel() const {
    return immediate_or_cancel;
}
double OrderBook::Order::is_all_or_nothing() const { return all_or_nothing; }
bool OrderBook::Order::is_queued() const { return queued; }


#endif
