#include "Order.h"

namespace fixex {

std::ostream & operator << (std::ostream & stream, Order const & order)
{
    stream << "SenderCompID = " << order.m_sender << ", "
           << "TargetCompID = " << order.m_target << ", "
           << "ClOrdID = " << order.m_clordid << ", "
           << "OrderID = " << order.m_orderid << ", "
           << "Symbol = " << order.m_symbol << ", "
           << "Type = " << static_cast<unsigned>(order.m_type) << ", "
           << "Side = " << static_cast<unsigned>(order.m_side) << ", "
           << "Price = " << order.m_price << ", "
           << "OrderQty = " << order.m_orderqty << ", "
           << "LastPx = " << order.m_lastpx << ", "
           << "LastQty = " << order.m_lastqty << ", "
           << "AvgPx = " << order.m_avgpx << ", "
           << "CumQty = " << order.m_cumqty << ", "
           << "LeavesQty = " << order.m_leavesqty << "";
    return stream;
}

}

