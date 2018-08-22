#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include <string>
#include <queue>
#include <functional>
#include <map>
#include <algorithm>

namespace fixex {

class OrderBook
{
public:
    using Price = double;
    using BidCompare = std::greater<Price>;
    using AskCompare = std::less<Price>;
    using BidOrders = std::multimap<Price, Order, BidCompare>;
    using AskOrders = std::multimap<Price, Order, AskCompare>;

public:
    Order const * insert(Order const &);
    Order const * remove(Order const &);
    Order const * lookup(std::string const & clordid);

    std::queue<Order> match();

    size_t get_bidsize() { return m_bids.size(); }
    size_t get_asksize() { return m_asks.size(); }

private:
    template <typename Container, typename Iterator = typename Container::const_iterator>
    Iterator lookup(Container const & container, std::string const & clordid)
    {
        return std::find_if(container.begin(), container.end(), [clordid](std::pair<Price,Order> const & priceorder){
            return priceorder.second.get_clordid() == clordid;
        });
    }

    void match(Order & bid, Order & ask);

private:
    std::queue<Order> m_orders;
    BidOrders m_bids;
    AskOrders m_asks;

public:
    friend std::ostream & operator << (std::ostream & stream, OrderBook const & orderbook);
};

}

#endif
