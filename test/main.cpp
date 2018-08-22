#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "OrderBook.h"
#include "Order.h"
#include "Exchange.h"

#include "quickfix/Utility.h"
#include "quickfix/Values.h"

#include <iostream>

using namespace fixex;

TEST_CASE("Integrate Quickfix", "integration")
{
    SECTION("FIX version")
    {
        REQUIRE(FIX::BeginString_FIX42 == std::string{"FIX.4.2"});
    }
}

TEST_CASE("Simple exchange based on FIX", "Fixex")
{
    Order bid("XYZ", "ABC", "201808210001", "APPL", Order::Type::MARKET, Order::Side::BUY,  220, 200);
    Order ask("XYZ", "ABC", "201808210002", "APPL", Order::Type::MARKET, Order::Side::SELL, 215, 100);

    SECTION("Order can be constructed")
    {
        REQUIRE(bid.get_price() == 220);
        REQUIRE(ask.get_price() == 215);
    }

    OrderBook orderbook;
    orderbook.insert(bid);
    orderbook.insert(ask);

    SECTION("OrderBook can insert Orders")
    {
        REQUIRE(orderbook.get_bidsize() == 1);
        REQUIRE(orderbook.get_asksize() == 1);
    }

    SECTION("OrderBook can lookup Orders")
    {
        REQUIRE(orderbook.lookup(bid.get_clordid())->get_price() == bid.get_price());
        REQUIRE(orderbook.lookup(ask.get_clordid())->get_price() == ask.get_price());
    }

    SECTION("OrderBook can match")
    {
        auto filled_orders = orderbook.match();
        REQUIRE(orderbook.get_bidsize() == 1);
        REQUIRE(orderbook.get_asksize() == 0);
        REQUIRE(filled_orders.size() == 2);
    }

    Exchange exchange;
    exchange.insert(bid);
    exchange.insert(ask);
    SECTION("Exchange can insert Orders")
    {
        REQUIRE(exchange.lookup(bid.get_symbol())->get_bidsize() == 1);
        REQUIRE(exchange.lookup(ask.get_symbol())->get_asksize() == 1);
    }

    SECTION("Exchange can match")
    {
        auto filled_orders = exchange.match();
        REQUIRE(exchange.lookup(bid.get_symbol())->get_bidsize() == 1);
        REQUIRE(exchange.lookup(ask.get_symbol())->get_asksize() == 0);
        REQUIRE(filled_orders.size() == 2);
    }
}

