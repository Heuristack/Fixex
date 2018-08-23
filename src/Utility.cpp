#include "Utility.h"

namespace fixex {

std::string IdentifierGenerator::get()
{
    std::stringstream s;
    s << m_pre << m_num++;
    return s.str();
}

}

