#pragma once

#include <SFML/System.hpp>
#include "c_base.h"

class C_Position : public C_Base
{
public:
    C_Position()
        : C_Base(Component::Position)
        , m_elevation(0)
    {
    }

    ~C_Position()
    {
    }

    void readIn(std::stringstream &l_stream)
    {
        l_stream >> m_position.x >> m_position.y >> m_elevation;
    }

    const sf::Vector2f &getPosition()
    {
        return m_position;
    }

    const sf::Vector2f getOldPosition()
    {
        return m_positionOld;
    }

    unsigned int getElevation()
    {
        return m_elevation;
    }

    void setPosition(float l_x, float l_y)
    {
        m_positionOld = m_position;
        m_position = sf::Vector2f(l_x, l_y);
    }

    void setPosition(const sf::Vector2f &l_position)
    {
        m_positionOld = m_position;
        m_position = l_position;
    }

    void setElevation(unsigned int l_elevation)
    {
        m_elevation = l_elevation;
    }

    void moveBy(float l_x, float l_y)
    {
        m_positionOld = m_position;
        m_position += sf::Vector2f(l_x, l_y);
    }

    void moveBy(const sf::Vector2f &l_vector)
    {
        m_positionOld = m_position;
        m_position += l_vector;
    }

private:
    sf::Vector2f m_position;
    sf::Vector2f m_positionOld;
    unsigned int m_elevation;
};
