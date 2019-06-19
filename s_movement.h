#pragma once

#include "s_base.h"
#include "direction.h"
#include "c_position.h"
#include "c_movable.h"

enum class Axis
{
    x, y
};

class Map;

class S_Movement : public S_Base
{
public:
    S_Movement(SystemManager *l_systemMgr);
    ~S_Movement();

    void update(float l_dt);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
    void notify(const Message &l_message);

    void setMap(Map *l_gameMap);
    void stopEntity(const EntityID &l_entity, const Axis &l_axis);
    void setDirection(const EntityID &l_entity, const Direction &l_dir);

private:
    const sf::Vector2f &getTileFriction(unsigned int l_elevation, unsigned int l_x, unsigned int l_y);
    void movementStep(float l_dt, C_Movable *l_movable, C_Position *l_position);

    Map *m_gameMap;
};
