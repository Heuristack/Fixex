#include "Order.h"

namespace fixex {

std::ostream & operator << (std::ostream & stream, Order const & order)
{
    stream << "ClOrdID = " << order.m_clordid << ", "
           << "Price = " << order.m_price << ", "
           << "OrderQty = " << order.m_orderqty << "";
    return stream;
}

}

