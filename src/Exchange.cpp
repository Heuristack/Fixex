#include "Exchange.h"

namespace fixex {

Order const * Exchange::insert(Order const & order)
{
    return m_orderbooks[order.get_symbol()].insert(order);
}

Order const * Exchange::remove(Order const & order)
{
    return m_orderbooks[order.get_symbol()].remove(order);
}

Order const * Exchange::lookup(Symbol const & symbol, std::string const & clordid)
{
    return m_orderbooks[symbol].lookup(clordid);
}

std::vector<Order> Exchange::match()
{
    std::vector<Order> orders;
    for (auto & symbol_orderbook : m_orderbooks) {
        for (auto const & order : symbol_orderbook.second.match()) {
            orders.push_back(order);
        }
    }
    return orders;
}

std::ostream & operator << (std::ostream & stream, Exchange const & exchange)
{
    for (auto const & symbol_orderbook : exchange.m_orderbooks) {
        stream << "SYMBOL: " << symbol_orderbook.first << "\n";
        stream << symbol_orderbook.second;
    }
    return stream;
}

}

