#include "OrderBook.h"

namespace fixex {

Order const * OrderBook::insert(Order const & order)
{
    Order * order_ptr = nullptr;
    if (order.get_side() == Order::Side::BUY) {
        auto i = m_bids.insert({order.get_price(), order});
        order_ptr = &i->second;
    }
    else {
        auto i = m_asks.insert({order.get_price(), order});
        order_ptr = &i->second;
    }
    return order_ptr;
}

Order const * OrderBook::remove(Order const & order)
{
    Order const * order_ptr = &order;
    if (order.get_side() == Order::Side::BUY) {
        auto i = lookup(m_bids, order.get_clordid());
        if (i != m_bids.end()) {
            m_bids.erase(i);
            order_ptr = nullptr;
        }
    }
    else {
        auto i = lookup(m_asks, order.get_clordid());
        if (i != m_asks.end()) {
            m_asks.erase(i);
            order_ptr = nullptr;
        }
    }
    return order_ptr;
}

Order const * OrderBook::lookup(std::string const & clordid)
{
    Order const * order_ptr = nullptr;
    auto bi = lookup(m_bids, clordid);
    if (bi != m_bids.end()) {
        order_ptr = &bi->second;
        return order_ptr;
    }
    auto ai = lookup(m_asks, clordid);
    if (ai != m_asks.end()) {
        order_ptr = &ai->second;
        return order_ptr;
    }
    return order_ptr;
}

void OrderBook::match(Order & bid, Order & ask)
{
    auto price = ask.get_price();
    auto quant = std::min(bid.get_leavesqty(), ask.get_leavesqty());
    bid.execute(price, quant);
    ask.execute(price, quant);
}

std::queue<Order> OrderBook::match()
{
    std::queue<Order> orders;
    while (!m_bids.empty() && !m_asks.empty()) {
        auto & bid = m_bids.begin()->second;
        auto & ask = m_asks.begin()->second;
        if (bid.get_price() > ask.get_price()) {
            match(bid, ask);
            orders.push(bid);
            orders.push(ask);
            if (bid.is_closed()) remove(bid);
            if (ask.is_closed()) remove(ask);
        }
        else break;
    }
    return orders;
}

std::ostream & operator << (std::ostream & stream, OrderBook const & orderbook)
{
    for (auto const & priceorder : orderbook.m_bids) {
        stream << "BID: " << priceorder.second << "\n";
    }
    for (auto const & priceorder : orderbook.m_asks) {
        stream << "ASK: " << priceorder.second << "\n";
    }
    return stream;
}

}

