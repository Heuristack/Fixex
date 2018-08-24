#include "OrderBook.h"
#include "Order.h"
#include "Exchange.h"
#include "Application.h"

#include "quickfix/Utility.h"
#include "quickfix/Values.h"

#include <iostream>

using namespace fixex;

int main() try
{
    FIX::SessionSettings session_settings;
    std::cin >> session_settings;

    FIX::FileStoreFactory store_factory(session_settings);
    FIX::ScreenLogFactory log_factory(session_settings);
    Exchange application;

    FIX::SocketAcceptor acceptor(application, store_factory, session_settings, log_factory);
    acceptor.start();
    while (true) {
        FIX::process_sleep(1);
    }
    acceptor.stop();

    return 0;
}
catch (std::exception const & e)
{
    std::cerr << "Exception Caught: " << std::quoted(e.what()) << "! Bye!" << std::endl;
    return 1;
}

