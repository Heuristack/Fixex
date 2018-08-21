#include "Order.h"

#include "quickfix/Utility.h"
#include "quickfix/Values.h"

#include <iostream>

int main()
{
    std::cout << "Fixex: a simple exchange based on " << FIX::BeginString_FIX42 << std::endl;
    using namespace fixex;
    Order order("XYZ", "ABC", "201808210001", "APPL", Order::Type::MARKET, Order::Side::BUY, 500, 100);
    std::cout << order << std::endl;
}

