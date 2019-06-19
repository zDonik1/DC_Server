#pragma once

#include <unordered_map>
#include "entitymessages.h"
#include "communicator.h"

using Subscriptions = std::unordered_map<EntityMessage, Communicator>;

class MessageHandler
{
public:
    bool subscribe(const EntityMessage &l_type, Observer *l_observer)
    {
        return m_communicators[l_type].addObserver(l_observer);
    }

    bool unsubscribe(const EntityMessage &l_type, Observer *l_observer)
    {
        return m_communicators[l_type].removeObserver(l_observer);
    }

    void dispatch(const Message &l_msg)
    {
        auto itr = m_communicators.find(static_cast<EntityMessage>(l_msg.m_type));

        if (itr == m_communicators.end()) {
            return;
        }

        itr->second.broadcast(l_msg);
    }

private:
    Subscriptions m_communicators;
};
