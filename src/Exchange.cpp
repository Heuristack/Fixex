#include "Exchange.h"

namespace fixex {

void Exchange::onMessage(FIX42::NewOrderSingle const & message, FIX::SessionID const & session_id) try
{
    FIX::SenderCompID sender; FIX::TargetCompID target;
    FIX::ClOrdID clordid;
    FIX::Symbol symbol;
    FIX::Side side;
    FIX::OrdType ordtype;
    FIX::Price price;
    FIX::OrderQty orderqty;
    FIX::TimeInForce timeinforce(FIX::TimeInForce_DAY);
    message.getHeader().get(sender);
    message.getHeader().get(target);
    message.get(clordid);
    message.get(side);
    message.get(symbol);
    message.get(ordtype);
    if (ordtype == FIX::OrdType_LIMIT) {
        message.get(price);
    }
    message.get(orderqty);
    message.get(timeinforce);
    Order order(sender, target, clordid, get_ordid_generator().get(), symbol, convert(ordtype), convert(side), price, orderqty);
    accept(order);
}
catch (std::exception const & e)
{
    std::cerr << "Exception Caught: " << e.what() << "!" << std::endl;
}

void Exchange::onMessage(FIX42::OrderCancelReplaceRequest const & message, FIX::SessionID const & session_id) try
{
    FIX::SenderCompID sender;
    FIX::TargetCompID target;
    FIX::OrderID orderid;
    FIX::ClOrdID clordid;
    FIX::Symbol symbol;
    FIX::Side side;
    FIX::OrdType ordtype;
    FIX::Price price;
    FIX::OrderQty orderqty;
    FIX::TimeInForce timeinforce(FIX::TimeInForce_DAY);
    message.getHeader().get(sender);
    message.getHeader().get(target);
    message.get(orderid);
    message.get(clordid);
    message.get(side);
    message.get(symbol);
    message.get(ordtype);
    if (ordtype == FIX::OrdType_LIMIT) {
        message.get(price);
    }
    message.get(orderqty);
    message.get(timeinforce);
    Order order(sender, target, clordid, orderid, symbol, convert(ordtype), convert(side), price, orderqty);
    update(order);
}
catch (std::exception const & e)
{
    std::cerr << "Exception Caught: " << e.what() << "!" << std::endl;
}

void Exchange::onMessage(FIX42::OrderCancelRequest const & message, FIX::SessionID const & session_id) try
{
    FIX::SenderCompID sender;
    FIX::TargetCompID target;
    FIX::OrderID orderid;
    FIX::ClOrdID clordid;
    FIX::Symbol symbol;
    FIX::Side side;
    message.getHeader().get(sender);
    message.getHeader().get(target);
    message.get(orderid);
    message.get(clordid);
    message.get(side);
    Order order(sender, target, clordid, orderid, symbol, Order::Type::LIMIT, convert(side), 0, 0);
    cancel(order);
}
catch (std::exception const & e)
{
    std::cerr << "Exception Caught: " << e.what() << "!" << std::endl;
}

void Exchange::accept(Order const & order)
{
    Order const * order_ptr = insert(order);
    if (order_ptr != nullptr) {
        report(&order, RequestType::CREATE);
        for (auto const & matched_order : match(order.get_symbol())) {
            report(&matched_order, RequestType::TRADED);
        }
    }
    else {
        reject(&order, nullptr, RequestType::CREATE, "can't create order");
    }
}

void Exchange::update(Order const & order)
{
    Order * origorder_ptr = lookup(order.get_symbol(), order.get_orderid());
    if (origorder_ptr != nullptr) {
        /**
         * TODO:
         * OrderCancel Replacement Rules:
         * 1. price or quant
         * 2. quant can't increase
         * 3. race
         *
        **/
        origorder_ptr->update_clordid(order.get_clordid());
        origorder_ptr->update_price(order.get_price());
        origorder_ptr->update_orderqty(order.get_orderqty());
        report(origorder_ptr, RequestType::UPDATE);
        for (auto const & matched_order : match(order.get_symbol())) {
            report(&matched_order, RequestType::TRADED);
        }
    }
    else {
        reject(&order, origorder_ptr, RequestType::UPDATE, "can't update order");
    }
}

void Exchange::cancel(Order const & order)
{
    Order * origorder_ptr = lookup(order.get_symbol(), order.get_orderid());
    if (origorder_ptr != nullptr) {
        remove(*origorder_ptr);
        report(&order, RequestType::REMOVE);
    }
    else {
        reject(&order, origorder_ptr, RequestType::UPDATE, "can't update order");
    }
}

void Exchange::report(Order const * order, RequestType reqtype) try
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
        FIX::OrderID(order->get_orderid()),
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
catch (std::exception const & e)
{
    std::cerr << "Exception Caught: " << e.what() << "!" << std::endl;
}

void Exchange::reject(Order const * order, Order const * origorder, RequestType reqtype, std::string const & reason) try
{
    FIX::TargetCompID target(order->get_sender());
    FIX::SenderCompID sender(order->get_target());
    if (reqtype == RequestType::CREATE) {
        FIX42::ExecutionReport execution = FIX42::ExecutionReport(
            FIX::OrderID(order->get_orderid()),
            FIX::ExecID(get_exeid_generator().get()),
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::ExecType_REJECTED),
            FIX::OrdStatus(FIX::OrdStatus_REJECTED),
            FIX::Symbol(order->get_symbol()),
            FIX::Side(convert(order->get_side())),
            FIX::LeavesQty(0),
            FIX::CumQty(0),
            FIX::AvgPx(0)
        );
        execution.set(FIX::ClOrdID(order->get_clordid()));
        execution.set(FIX::OrderQty(order->get_orderqty()));
        FIX::Session::sendToTarget(execution, sender, target);
    }
    else {
        FIX::CxlRejResponseTo cxlrejresponseto = '1';
        if (reqtype == RequestType::UPDATE) cxlrejresponseto = '2';
        FIX42::OrderCancelReject cancelreject = FIX42::OrderCancelReject(
            FIX::OrderID(order->get_orderid()),
            FIX::ClOrdID(order->get_clordid()),
            FIX::OrigClOrdID(origorder->get_clordid()),
            FIX::OrdStatus(FIX::OrdStatus_REJECTED),
            cxlrejresponseto
        );
        FIX::Session::sendToTarget(cancelreject, sender, target);
    }
}
catch (std::exception const & e)
{
    std::cerr << "Exception Caught: " << e.what() << "!" << std::endl;
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

