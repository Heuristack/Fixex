#include "OrderBook.h"
#include "Order.h"
#include "Exchange.h"

#include "quickfix/Utility.h"
#include "quickfix/Values.h"

#include <iostream>

using namespace fixex;

int main()
{
    std::cout << "Fixex: a simple exchange based on " << FIX::BeginString_FIX42 << std::endl;
}

