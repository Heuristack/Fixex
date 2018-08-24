#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include <string>
#include <vector>
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
    Order * insert(Order const &);
    Order * remove(Order const &);
    Order * remove(std::string const & orderid_or_clordid);
    Order * lookup(std::string const & orderid_or_clordid);

    std::vector<Order> match();

    size_t get_bidsize() const { return m_bids.size(); }
    size_t get_asksize() const { return m_asks.size(); }

private:
    template <typename Container, typename Iterator = typename Container::const_iterator>
    Iterator lookup(Container const & container, std::string const & orderid_or_clordid)
    {
        return std::find_if(container.begin(), container.end(), [orderid_or_clordid](std::pair<Price,Order> const & price_order){
            return price_order.second.get_orderid() == orderid_or_clordid
                || price_order.second.get_clordid() == orderid_or_clordid;
        });
    }

    void match(Order & bid, Order & ask);

private:
    BidOrders m_bids;
    AskOrders m_asks;

public:
    friend std::ostream & operator << (std::ostream & stream, OrderBook const & orderbook);
};

}

#endif

