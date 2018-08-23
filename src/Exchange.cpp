#include "Exchange.h"

namespace fixex {

void Exchange::onMessage(FIX42::NewOrderSingle const & message, FIX::SessionID const & session_id)
{}

void Exchange::onMessage(FIX42::OrderCancelRequest const & message, FIX::SessionID const & session_id)
{}

void Exchange::onMessage(FIX42::OrderCancelReplaceRequest const & message, FIX::SessionID const & session_id)
{}

void Exchange::accept(Order const &)
{}

void Exchange::update(Order const &)
{}

void Exchange::cancel(Order const &)
{}

void Exchange::report(Order const *, RequestType)
{}

void Exchange::reject(Order const *, RequestType)
{}

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

OrderBook const * Exchange::lookup(Symbol const & symbol)
{
    return &(m_orderbooks[symbol]);
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

