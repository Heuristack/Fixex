#include "Exchange.h"

namespace fixex {

void Exchange::onMessage(FIX42::NewOrderSingle const & message, FIX::SessionID const & session_id) try
{
    FIX::SenderCompID sender;
    FIX::TargetCompID target;
    FIX::ClOrdID clordid;
    FIX::Symbol symbol;
    FIX::OrdType ordtype;
    FIX::Side side;
    FIX::Price price;
    FIX::OrderQty orderqty;
    FIX::TimeInForce timeinforce(FIX::TimeInForce_DAY);
    message.getHeader().get(sender);
    message.getHeader().get(target);
    message.get(clordid);
    message.get(symbol);
    message.get(ordtype);
    message.get(side);
    if (ordtype == FIX::OrdType_LIMIT) {
        message.get(price);
    }
    message.get(orderqty);
    message.get(timeinforce);
    Order order(sender, target, clordid, symbol, convert(ordtype), convert(side), price, orderqty);
    accept(order);
}
catch (std::exception const & e)
{
    std::cerr << "Exception Caught: " << e.what() << "!" << std::endl;
}

void Exchange::onMessage(FIX42::OrderCancelRequest const & message, FIX::SessionID const & session_id) try
{}
catch (std::exception const & e)
{
    std::cerr << "Exception Caught: " << e.what() << "!" << std::endl;
}

void Exchange::onMessage(FIX42::OrderCancelReplaceRequest const & message, FIX::SessionID const & session_id) try
{}
catch (std::exception const & e)
{
    std::cerr << "Exception Caught: " << e.what() << "!" << std::endl;
}

void Exchange::accept(Order const & order)
{
    Order const * order_ptr = insert(order);
    std::cerr << "INSERT: " <<  *this << std::endl;
    if (order_ptr != nullptr) {
        report(&order, RequestType::CREATE);

        auto matched_orders = match();
        std::cerr << "MATCH:" << matched_orders.size() << ":" <<  *this << std::endl;

        for (auto const & matched_order : matched_orders) {
            report(&matched_order, RequestType::TRADED);
        }
    }
    else {
        reject(&order, RequestType::CREATE, "can't create order");
    }
}

void Exchange::update(Order const &)
{}

void Exchange::cancel(Order const &)
{}

void Exchange::report(Order const * order, RequestType reqtype)
{
    FIX::TargetCompID target(order->get_sender());
    FIX::SenderCompID sender(order->get_target());
    FIX::ExecTransType exectranstype;
    char status = '0';
    switch (reqtype) {
        case RequestType::CREATE: default:
            exectranstype = FIX::ExecTransType_NEW;
            status = FIX::OrdStatus_NEW;
            break;
        case RequestType::UPDATE:
            exectranstype = FIX::ExecTransType_NEW;
            status = FIX::OrdStatus_REPLACED;
            break;
        case RequestType::REMOVE:
            exectranstype = FIX::ExecTransType_NEW;
            status = FIX::OrdStatus_CANCELED;
            break;
        case RequestType::TRADED:
            exectranstype = FIX::ExecTransType_NEW;
            if (order->is_filled()) status = FIX::OrdStatus_FILLED;
            else status = FIX::OrdStatus_PARTIALLY_FILLED;
            break;
    }
    FIX42::ExecutionReport execution = FIX42::ExecutionReport(
        FIX::OrderID(get_ordid_generator().get()),
        FIX::ExecID(get_exeid_generator().get()),
        FIX::ExecTransType(FIX::ExecTransType_NEW),
        FIX::ExecType(status),
        FIX::OrdStatus(status),
        FIX::Symbol(order->get_symbol()),
        FIX::Side(convert(order->get_side())),
        FIX::LeavesQty(order->get_leavesqty()),
        FIX::CumQty(order->get_cumqty()),
        FIX::AvgPx(order->get_avgpx())
    );
    execution.set(FIX::ClOrdID(order->get_clordid()));
    execution.set(FIX::OrderQty(order->get_orderqty()));
    if (reqtype == RequestType::TRADED) {
        execution.set(FIX::LastShares(order->get_lastqty()));
        execution.set(FIX::LastPx(order->get_lastpx()));
    }
    FIX::Session::sendToTarget(execution, sender, target);
}

void Exchange::reject(Order const * order, RequestType reqtype, std::string const & reason)
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

std::vector<Order> Exchange::match(Symbol const & symbol)
{
    std::vector<Order> orders;
    for (auto const & order : m_orderbooks[symbol].match()) {
        orders.push_back(order);
    }
    return orders;
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

