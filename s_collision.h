#pragma once

#include "s_base.h"
#include "c_position.h"
#include "c_collidable.h"

struct TileInfo;
class Map;

struct CollisionElement
{
    CollisionElement(float l_area, TileInfo *l_info, const sf::FloatRect &l_bounds)
        : m_area(l_area)
        , m_tile(l_info)
        , m_tileBounds(l_bounds)
    {
    }

    float m_area;
    TileInfo *m_tile;
    sf::FloatRect m_tileBounds;
};

using Collisions = std::vector<CollisionElement>;

class S_Collision : public S_Base
{
public:
    S_Collision(SystemManager *l_systemMgr);

    void setMap(Map *l_map);
    void update(float l_dt);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
    void notify(const Message &l_message);

private:
    void checkOutOfBounds(C_Position *l_pos, C_Collidable *l_col);
    void mapCollisions(const EntityID &l_entity, C_Position *l_pos, C_Collidable *l_col);
    void entityCollisions();

    Map *m_gameMap;
};
