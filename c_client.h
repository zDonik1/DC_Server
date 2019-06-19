#pragma once

#include "c_base.h"
#include "networkdefinitions.h"

class C_Client : public C_Base
{
public:
    C_Client()
        : C_Base(Component::Client)
        , m_clientID(static_cast<ClientID>(Network::NullID))
    {
    }

    void readIn(std::stringstream &l_stream)
    {
    }

    ClientID getClientID() const
    {
        return m_clientID;
    }

    void setClientID(const ClientID &l_id)
    {
        m_clientID = l_id;
    }

private:
    ClientID m_clientID;
};
