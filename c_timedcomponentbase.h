#pragma once

#include <SFML/System.hpp>
#include "c_base.h"

class C_TimedComponentBase : public C_Base
{
public:
    C_TimedComponentBase(const Component &l_type)
        : C_Base(l_type)
        , m_duration(sf::milliseconds(0))
    {
    }

    virtual ~C_TimedComponentBase()
    {
    }

    const sf::Time &getTimer() const
    {
        return m_duration;
    }

    void setTimer(const sf::Time &l_time)
    {
        m_duration = l_time;
    }

    void addToTimer(const sf::Time &l_time)
    {
        m_duration += l_time;
    }

    void resetTimer()
    {
        m_duration = sf::milliseconds(0);
    }

protected:
    sf::Time m_duration;
};
