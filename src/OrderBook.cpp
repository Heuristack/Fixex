#include "OrderBook.h"

namespace fixex {

Order * OrderBook::insert(Order const & order)
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

Order * OrderBook::remove(Order const & order)
{
    Order * order_ptr = nullptr;
    if (order.get_side() == Order::Side::BUY) {
        auto i = lookup(m_bids, order.get_orderid());
        if (i != m_bids.end()) {
            m_bids.erase(i);
            order_ptr = nullptr;
        }
        else {
            i = lookup(m_bids, order.get_clordid());
            if (i != m_bids.end()) {
                m_bids.erase(i);
                order_ptr = nullptr;
            }
        }
    }
    else {
        auto i = lookup(m_asks, order.get_orderid());
        if (i != m_asks.end()) {
            m_asks.erase(i);
            order_ptr = nullptr;
        }
        else {
            i = lookup(m_asks, order.get_clordid());
            if (i != m_asks.end()) {
                m_asks.erase(i);
                order_ptr = nullptr;
            }
        }
    }
    return order_ptr;
}

Order * OrderBook::remove(std::string const & orderid_or_clordid)
{
    Order * order_ptr = nullptr;
    auto bi = lookup(m_bids, orderid_or_clordid);
    if (bi != m_bids.end()) {
        m_bids.erase(bi);
        return order_ptr;
    }
    auto ai = lookup(m_asks, orderid_or_clordid);
    if (ai != m_asks.end()) {
        m_asks.erase(ai);
        return order_ptr;
    }
    return order_ptr;
}

Order * OrderBook::lookup(std::string const & orderid_or_clordid)
{
    Order * order_ptr = nullptr;
    auto bi = lookup(m_bids, orderid_or_clordid);
    if (bi != m_bids.end()) {
        order_ptr = const_cast<Order*>(&bi->second);
        return order_ptr;
    }
    auto ai = lookup(m_asks, orderid_or_clordid);
    if (ai != m_asks.end()) {
        order_ptr = const_cast<Order*>(&ai->second);
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

std::vector<Order> OrderBook::match()
{
    std::vector<Order> orders;
    while (!m_bids.empty() && !m_asks.empty()) {
        auto & bid = m_bids.begin()->second;
        auto & ask = m_asks.begin()->second;
        if (bid.get_price() >= ask.get_price()) {
            match(bid, ask);
            orders.push_back(bid);
            orders.push_back(ask);
            if (bid.is_closed()) remove(bid);
            if (ask.is_closed()) remove(ask);
        }
        else break;
    }
    return orders;
}

std::ostream & operator << (std::ostream & stream, OrderBook const & orderbook)
{
    for (auto const & price_order : orderbook.m_bids) {
        stream << "BID: " << price_order.second << "\n";
    }
    for (auto const & price_order : orderbook.m_asks) {
        stream << "ASK: " << price_order.second << "\n";
    }
    return stream;
}

}

