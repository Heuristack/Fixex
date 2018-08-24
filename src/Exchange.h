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
    void reject(Order const *, RequestType, std::string const &);

public:
    using Symbol = std::string;
    using SymbolIndexedOrderBooks = std::unordered_map<Symbol, OrderBook>;

public:
    Order const * insert(Order const &);
    Order const * remove(Order const &);
    Order const * lookup(Symbol const & symbol, std::string const & clordid);

    OrderBook const * lookup(Symbol const & symbol);
    std::vector<Order> match(Symbol const & symbol);
    std::vector<Order> match();

private:
    SymbolIndexedOrderBooks m_orderbooks;

public:
    friend std::ostream & operator << (std::ostream & stream, Exchange const & exchange);
};

inline Order::Side convert(FIX::Side const & side) { if (side == FIX::Side_SELL) return Order::Side::SELL; else return Order::Side::BUY; }
inline Order::Type convert(FIX::OrdType const & ordtype) { if (ordtype == FIX::OrdType_LIMIT) return Order::Type::LIMIT; else return Order::Type::MARKET; }
inline FIX::Side convert(Order::Side side) { if (side == Order::Side::SELL) return FIX::Side_SELL; else return FIX::Side_BUY; }
inline FIX::OrdType convert(Order::Type ordtype) { if (ordtype == Order::Type::LIMIT) return FIX::OrdType_LIMIT; else return FIX::OrdType_MARKET; }

}

#endif

