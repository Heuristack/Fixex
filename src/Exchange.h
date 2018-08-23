#ifndef EXCHANGE_H
#define EXCHANGE_H

#include "Application.h"
#include "OrderBook.h"
#include "Order.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <ostream>

namespace fixex {

class Exchange : public Application
{
public:
    void onMessage(FIX42::NewOrderSingle const &, FIX::SessionID const &) override;
    void onMessage(FIX42::OrderCancelRequest const &, FIX::SessionID const &) override;
    void onMessage(FIX42::OrderCancelReplaceRequest const &, FIX::SessionID const &) override;

public:
    enum class RequestType { CREATE, UPDATE, REMOVE, TRADED };
    void accept(Order const &);
    void update(Order const &);
    void cancel(Order const &);
    void report(Order const *, RequestType);
    void reject(Order const *, RequestType);

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

