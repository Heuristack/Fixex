#include "Order.h"

namespace fixex {

std::ostream & operator << (std::ostream & stream, Order const & order)
{
    stream << "ClOrdID = " << order.m_clordid << ", "
           << "Symbol = " << order.m_symbol << ", "
           << "Side = " << static_cast<unsigned>(order.m_side) << ", "
           << "Price = " << order.m_price << ", "
           << "OrderQty = " << order.m_orderqty << ", "
           << "LastPx = " << order.m_lastpx << ", "
           << "LastQty = " << order.m_lastqty << ", "
           << "LeavesQty = " << order.m_leavesqty << "";
    return stream;
}

}

