#include "serverentitymanager.h"

ServerEntityManager::ServerEntityManager(SystemManager *l_systemMgr)
    : EntityManager(l_systemMgr)
{
    addComponentType<C_Position>(Component::Position);
    addComponentType<C_State>(Component::State);
    addComponentType<C_Movable>(Component::Movable);
    addComponentType<C_Controller>(Component::Controller);
    addComponentType<C_Collidable>(Component::Collidable);
    addComponentType<C_Client>(Component::Client);
    addComponentType<C_Health>(Component::Health);
    addComponentType<C_Name>(Component::Name);
    addComponentType<C_Attacker>(Component::Attacker);
}

ServerEntityManager::~ServerEntityManager()
{
}

void ServerEntityManager::dumpEntityInfo(sf::Packet &l_packet)
{
    for (auto &entity : m_entities) {
        l_packet << sf::Int32(entity.first);
        EntitySnapshot snapshot;
        snapshot.m_type = entity.second.m_type;
        const auto &mask = entity.second.m_bitmask;
        if (mask.getBit((unsigned int)Component::Position)) {
            C_Position *p = getComponent<C_Position>(entity.first, Component::Position);
            snapshot.m_position = p->getPosition();
            snapshot.m_elevation = p->getElevation();
        }
        if (mask.getBit((unsigned int)Component::Movable)) {
            C_Movable *m = getComponent<C_Movable>(entity.first, Component::Movable);
            snapshot.m_velocity = m->getVelocity();
            snapshot.m_acceleration = m->getAcceleration();
            snapshot.m_direction = sf::Uint8(m->getDirection());
        }
        if (mask.getBit((unsigned int)Component::State)) {
            C_State *s = getComponent<C_State>(entity.first, Component::State);
            snapshot.m_state = sf::Uint8(s->getState());
        }
        if (mask.getBit((unsigned int)Component::Health)) {
            C_Health *h = getComponent<C_Health>(entity.first, Component::Health);
            snapshot.m_health = h->getHeatlh();
        }
        if (mask.getBit((unsigned int)Component::Name)) {
            C_Name *n = getComponent<C_Name>(entity.first, Component::Name);
            snapshot.m_name = n->getName();
        }

        l_packet << snapshot;
    }
}
