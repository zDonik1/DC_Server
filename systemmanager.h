#pragma once

#include <SFML/Graphics.hpp>
#include "eventqueue.h"
#include "messagehandler.h"
#include "s_base.h"

using SystemContainer = std::unordered_map<System, S_Base*>;
using EntityEventContainer = std::unordered_map<EntityID, EventQueue>;

class EntityManager;

class SystemManager
{
public:
    SystemManager(Server *l_server);
    virtual ~SystemManager();

    void setEntityManager(EntityManager* l_entityMgr);
    EntityManager *getEntityManager();
    MessageHandler *getMessageHandler();

    template<class T>
    T *getSystem(const System &l_system)
    {
        auto itr = m_systems.find(l_system);
        return (itr != m_systems.end() ? dynamic_cast<T*>(itr->second) : nullptr);
    }

    template<class T>
    bool addSystem(const System &l_id)
    {
        if (m_systems.find(l_id) != m_systems.end()) {
            return false;
        }

        m_systems[l_id] = new T(this);
        return true;
    }

    void addEvent(const EntityID &l_entity, const EventID &l_event);

    void update(float l_dt);
    void handleEvents();

    void entityModified(const EntityID &l_entity, const Bitmask &l_bits);
    void removeEntity(const EntityID &l_entity);

    void purgeEntities();
    void purgeSystems();

protected:
    SystemContainer m_systems;
    EntityManager *m_entityManager;
    EntityEventContainer m_events;
    MessageHandler m_messages;
    Server *m_server;
};
