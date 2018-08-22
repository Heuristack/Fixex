#include "OrderBook.h"
#include "Order.h"

#include "quickfix/Utility.h"
#include "quickfix/Values.h"

#include <iostream>

using namespace fixex;

int main()
{
    std::cout << "Fixex: a simple exchange based on " << FIX::BeginString_FIX42 << std::endl;

    Order bid("XYZ", "ABC", "201808210001", "APPL", Order::Type::MARKET, Order::Side::BUY,  220, 200);
    Order ask("XYZ", "ABC", "201808210002", "APPL", Order::Type::MARKET, Order::Side::SELL, 215, 100);
    std::cout << bid << std::endl;
    std::cout << ask << std::endl;

    OrderBook orderbook;
    orderbook.insert(bid);
    orderbook.insert(ask);
    std::cout << orderbook << std::endl;

    auto orders = orderbook.match();
    std::cout << orders.size() << std::endl;
    std::cout << orderbook << std::endl;
}

