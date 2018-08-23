#include "Application.h"

namespace fixex {

void Executor::onMessage(FIX42::NewOrderSingle const & message, FIX::SessionID const & session_id)
{
    FIX::OrdType ordtype;
    FIX::Symbol symbol;
    FIX::Side side;
    FIX::OrderQty orderqty;
    FIX::Price price;
    FIX::ClOrdID clordid;
    FIX::Account account;
    message.get(ordtype);
    if (ordtype != FIX::OrdType_LIMIT) {
        throw FIX::IncorrectTagValue(ordtype.getField());
    }
    message.get(symbol);
    message.get(side);
    message.get(orderqty);
    message.get(price);
    message.get(clordid);

    FIX42::ExecutionReport executionreport = FIX42::ExecutionReport(
        FIX::OrderID(get_ordid_generator().get()),
        FIX::ExecID(get_exeid_generator().get()),
        FIX::ExecTransType(FIX::ExecTransType_NEW),
        FIX::ExecType(FIX::ExecType_FILL),
        FIX::OrdStatus(FIX::OrdStatus_FILLED),
        symbol,
        side,
        FIX::LeavesQty(0),
        FIX::CumQty(orderqty),
        FIX::AvgPx(price)
    );
    executionreport.set(clordid);
    executionreport.set(orderqty);
    executionreport.set(FIX::LastShares(orderqty));
    executionreport.set(FIX::LastPx(price));
    if(message.isSet(account)) {
        executionreport.setField( message.get(account) );
    }

    try {
        FIX::Session::sendToTarget(executionreport, session_id);
    }
    catch (FIX::SessionNotFound const & e) {
        std::cerr << "Exception Caught: " << std::quoted(e.what()) << "!" << std::endl;
    }
}
}

