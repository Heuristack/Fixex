#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "OrderBook.h"
#include "Order.h"
#include "Exchange.h"
#include "Utility.h"

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

TEST_CASE("Utilities", "util")
{
    IdentifierGenerator generator("G", 5000);
    SECTION("IdentifierGenerator can generate")
    {
        REQUIRE(generator.get() == std::string{"G5000"});
        REQUIRE(generator.get() == std::string{"G5001"});
    }
}

TEST_CASE("Exchange Functionalities", "exchange")
{
    IdentifierGenerator generator("ORDERID", 1000);

    Order bid("XYZ", "ABC", "201808210001", generator.get(), "APPL", Order::Type::MARKET, Order::Side::BUY,  220, 200);
    Order ask("XYZ", "ABC", "201808210002", generator.get(), "APPL", Order::Type::MARKET, Order::Side::SELL, 215, 100);

    SECTION("Order can be constructed")
    {
        REQUIRE(bid.get_price() == 220);
        REQUIRE(ask.get_price() == 215);
        REQUIRE(bid.get_orderid() == std::string{"ORDERID1000"});
        REQUIRE(ask.get_orderid() == std::string{"ORDERID1001"});
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
        REQUIRE(orderbook.lookup(bid.get_orderid())->get_orderqty() == bid.get_orderqty());
        REQUIRE(orderbook.lookup(ask.get_orderid())->get_orderqty() == ask.get_orderqty());
    }

    auto filled_orders = orderbook.match();

    SECTION("OrderBook can match")
    {
        REQUIRE(orderbook.get_bidsize() == 1);
        REQUIRE(orderbook.get_asksize() == 0);
        REQUIRE(filled_orders.size() == 2);
    }

    SECTION("Order can execute")
    {
        REQUIRE(orderbook.lookup(bid.get_clordid())->get_lastqty() == 100);
        REQUIRE(orderbook.lookup(bid.get_clordid())->get_lastpx() == 215);
        REQUIRE(orderbook.lookup(bid.get_clordid())->get_leavesqty() == 100);
        REQUIRE(orderbook.lookup(bid.get_clordid())->get_avgpx() == 215);
        REQUIRE(orderbook.lookup(bid.get_clordid())->get_cumqty() == 100);
    }

    SECTION("OrderBook can remove")
    {
        orderbook.remove(bid);
        REQUIRE(orderbook.get_bidsize() == 0);
    }

    Exchange exchange;
    exchange.insert(bid);
    exchange.insert(ask);

    SECTION("Exchange can insert Orders")
    {
        REQUIRE(exchange.lookup(bid.get_symbol())->get_bidsize() == 1);
        REQUIRE(exchange.lookup(ask.get_symbol())->get_asksize() == 1);
    }

    auto exchange_filled_orders = exchange.match();

    SECTION("Exchange can match")
    {
        REQUIRE(exchange.lookup(bid.get_symbol())->get_bidsize() == 1);
        REQUIRE(exchange.lookup(ask.get_symbol())->get_asksize() == 0);
        REQUIRE(exchange_filled_orders.size() == 2);
    }
}

