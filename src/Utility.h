#ifndef UTILITY_H
#define UTILITY_H

#include <sstream>
#include <string>

namespace fixex {

class IdentifierGenerator
{
public:
    IdentifierGenerator(std::string const & s, long i): m_pre{s}, m_num{i}
    {}

    std::string get();

private:
    std::string m_pre;
    long m_num;
};

}

#endif

