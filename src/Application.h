#ifndef APPLICATION_H
#define APPLICATION_H

#include "Utility.h"

#include "quickfix/SessionSettings.h"
#include "quickfix/Session.h"
#include "quickfix/Application.h"
#include "quickfix/SocketAcceptor.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/Log.h"
#include "quickfix/Mutex.h"
#include "quickfix/Values.h"
#include "quickfix/Utility.h"

#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix42/OrderCancelReplaceRequest.h"
#include "quickfix/fix42/OrderCancelRequest.h"
#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix42/OrderCancelReject.h"

#include <iostream>

namespace fixex {

class Application : public FIX::Application, public FIX::MessageCracker
{
public:
    Application(): m_generator_ordid{"O", 1000}, m_generator_exeid{"E", 1000}
    {}

public:
    void onCreate(FIX::SessionID const &) override { std::cerr << "Application is created!" << std::endl; }
    void onLogon (FIX::SessionID const &) override { std::cerr << "logon!"  << std::endl; }
    void onLogout(FIX::SessionID const &) override { std::cerr << "logout!" << std::endl; }

    void toAdmin(FIX::Message &, FIX::SessionID const &) override {}
    void toApp(FIX::Message &, FIX::SessionID const &) throw(FIX::DoNotSend) override {}

    void fromAdmin(FIX::Message const &, FIX::SessionID const &)
    throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) override {}
    void fromApp(FIX::Message const & message, FIX::SessionID const & session_id)
    throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) override { crack(message, session_id); }

public:
    IdentifierGenerator & get_ordid_generator() const { return m_generator_ordid; }
    IdentifierGenerator & get_exeid_generator() const { return m_generator_exeid; }

private:
    IdentifierGenerator mutable m_generator_ordid;
    IdentifierGenerator mutable m_generator_exeid;
};

class Executor : public Application
{
public:
    void onMessage(FIX42::NewOrderSingle const &, FIX::SessionID const &) override;
};

}

#endif

