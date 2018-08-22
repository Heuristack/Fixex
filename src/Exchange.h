#ifndef EXCHANGE_H
#define EXCHANGE_H

#include "OrderBook.h"
#include "Order.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <ostream>

namespace fixex {

class Exchange
{
public:
    using Symbol = std::string;
    using SymbolIndexedOrderBooks = std::unordered_map<Symbol, OrderBook>;

public:
    Order const * insert(Order const &);
    Order const * remove(Order const &);
    Order const * lookup(Symbol const & symbol, std::string const & clordid);
    OrderBook const * lookup(Symbol const & symbol);

    std::vector<Order> match();

private:
    SymbolIndexedOrderBooks m_orderbooks;

public:
    friend std::ostream & operator << (std::ostream & stream, Exchange const & exchange);
};

}

#endif

