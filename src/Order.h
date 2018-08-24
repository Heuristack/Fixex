#ifndef ORDER_H
#define ORDER_H

#include <ostream>
#include <iomanip>
#include <string>

namespace fixex {

class Order
{
public:
    enum class Side { BUY, SELL };
    enum class Type { MARKET, LIMIT };

public:
    Order(std::string const & sender, std::string const & target,
          std::string const & clordid, std::string const & orderid,
          std::string const & symbol,
          Type type, Side side,
          double price, long orderqty)
    : m_sender{sender}, m_target{target},
      m_clordid{clordid}, m_orderid{orderid},
      m_symbol{symbol},
      m_type{type}, m_side{side},
      m_price{price},m_orderqty{orderqty}
    {
        m_leavesqty = m_orderqty;
        m_cumqty = 0;
        m_avgpx = 0;
        m_lastqty = 0;
        m_lastpx = 0;
    }

public:
    std::string const & get_sender() const { return m_sender; }
    std::string const & get_target() const { return m_target; }
    std::string const & get_clordid() const { return m_clordid; }
    std::string const & get_orderid() const { return m_orderid; }
    std::string const & get_symbol() const { return m_symbol; }
    Type get_type() const { return m_type; }
    Side get_side() const { return m_side; }
    double get_price() const { return m_price; }
    long get_orderqty() const { return m_orderqty; }

public:
    double get_lastpx() const { return m_lastpx; }
    long get_lastqty() const { return m_lastqty; }
    double get_avgpx() const { return m_avgpx; }
    long get_cumqty() const { return m_cumqty; }
    long get_leavesqty() const { return m_leavesqty; }

    bool is_filled() const { return m_orderqty == m_cumqty; }
    bool is_closed() const { return m_leavesqty == 0; }

public:
    void execute(double p, long q)
    {
        m_lastpx = p;
        m_lastqty = q;
        m_avgpx = (p * q + m_avgpx * m_cumqty)/(q + m_cumqty);
        m_cumqty += q;
        m_leavesqty -= q;
    }

    bool update_price(double p)
    {
        m_price = p;
        return true;
    }

    bool update_orderqty(double q)
    {
        double delta = q - m_leavesqty;
        if ((m_orderqty + delta >= 0) && (m_leavesqty + delta >= 0)) {
            m_orderqty += delta;
            m_leavesqty += delta;
            return true;
        }
        return false;
    }

    void update_clordid(std::string const & clordid)
    {
        m_clordid = clordid;
    }

private:
    std::string m_sender;
    std::string m_target;
    std::string m_clordid;
    std::string m_orderid;
    std::string m_symbol;
    Type m_type;
    Side m_side;
    double m_price;
    long m_orderqty;
    double m_lastpx;
    long m_lastqty;
    double m_avgpx;
    long m_cumqty;
    long m_leavesqty;

public:
    friend std::ostream & operator << (std::ostream & stream, Order const & order);
};

}

#endif

