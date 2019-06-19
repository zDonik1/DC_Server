#pragma once

#include "c_base.h"

enum class EntityState
{
    Idle,
    Walking,
    Attacking,
    Hurt,
    Dying
};

class C_State : public C_Base
{
public:
    C_State() : C_Base(Component::State)
    {
    }

    void readIn(std::stringstream &l_stream)
    {
        unsigned int state = 0;
        l_stream >> state;
        m_state = (EntityState)state;
    }

    EntityState getState() const
    {
        return m_state;
    }

    void setState(const EntityState &l_state)
    {
        m_state = l_state;
    }

private:
    EntityState m_state;
};
