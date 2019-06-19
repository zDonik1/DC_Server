#pragma once

#include <SFML/Graphics/Rect.hpp>
#include "c_timedcomponentbase.h"

class C_Attacker : public C_TimedComponentBase
{
public:
    C_Attacker()
        : C_TimedComponentBase(Component::Attacker)
        , m_attacked(false)
        , m_knockback(0.f)
        , m_attackDuration(0)
    {
    }

    void readIn(std::stringstream &l_stream)
    {
        l_stream >> m_offset.x >> m_offset.y >> m_attackArea.width >> m_attackArea.height >> m_knockback >> m_attackDuration;
    }

    void setAreaPosition(const sf::Vector2f &l_pos)
    {
        m_attackArea.left = l_pos.x;
        m_attackArea.top = l_pos.y;
    }

    const sf::FloatRect &getAreaOfAttack()
    {
        return m_attackArea;
    }

    const sf::Vector2f &getOffset()
    {
        return m_offset;
    }

    bool hasAttacked()
    {
        return m_attacked;
    }

    void setAttacked(bool l_attacked)
    {
        m_attacked = l_attacked;
    }

    float getKnockback()
    {
        return m_knockback;
    }

    sf::Uint32 getAttackDuration()
    {
        return m_attackDuration;
    }

private:
    sf::FloatRect m_attackArea;
    sf::Vector2f m_offset;
    bool m_attacked;
    float m_knockback;
    sf::Uint32 m_attackDuration;
};
