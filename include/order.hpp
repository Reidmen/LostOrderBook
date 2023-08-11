#ifdef ORDER_HPP
#define ORDER_HPP

#include <map>
#include <list>

namespace OrderBook {

    class OrderLimit;
    class Book;


    /*
     * @brief Order class
     *
     */
    class Order: public std::enable_sharing<Order> {
        private:
            // pointer to book, guaranteed to deferencable
            Book *book = nullptr;

            // iterators to allocate order in book, cancel O(1)
            std::map<double, OrderBook::OrderLimit>::iterator limitIterator;
            std::list<order_ptr>::iterator orderIterator;

        protected:
            virtual void on_accepted();
            virtual void on_queue();
            virtual void on_rejected();

        public:
            // @brief Construct a new order object
            Order();

            inline bool Cancel();
            inline book *get_book() const;

    }

}


#endif
