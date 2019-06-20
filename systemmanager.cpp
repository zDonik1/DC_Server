#include "systemmanager.h"
#include "entitymanager.h"

SystemManager::SystemManager(Server *l_server)
    : m_entityManager(nullptr)
    , m_server(l_server)
{
}

SystemManager::~SystemManager()
{
    purgeSystems();
}

void SystemManager::setEntityManager(EntityManager *l_entityMgr)
{
    if (!m_entityManager) {
        m_entityManager = l_entityMgr;
    }
}

EntityManager *SystemManager::getEntityManager()
{
    return m_entityManager;
}

MessageHandler *SystemManager::getMessageHandler()
{
    return &m_messages;
}

void SystemManager::addEvent(const EntityID &l_entity, const EventID &l_event)
{
    m_events[l_entity].addEvent(l_event);
}

void SystemManager::update(float l_dt)
{
    for (auto &itr : m_systems) {
        sf::Lock lock(m_server->getMutex());
        itr.second->update(l_dt);
    }

    handleEvents();
}

void SystemManager::handleEvents()
{
    for (auto &event : m_events) {
        EventID id = 0;
        while (event.second.processEvents(id)) {
            for (auto &system : m_systems) {
                if (system.second->hasEntity(event.first)) {
                    system.second->handleEvent(event.first, static_cast<EntityEvent>(id));
                }
            }
        }
    }
}

void SystemManager::entityModified(const EntityID &l_entity, const Bitmask &l_bits)
{
    for (auto &s_itr : m_systems) {
        S_Base *system = s_itr.second;
        if (system->fitsRequirements(l_bits)) {
            if (!system->hasEntity(l_entity)) {
                system->addEntity(l_entity);
            }
        }
        else {
            if (system->hasEntity(l_entity)) {
                system->removeEntity(l_entity);
            }
        }
    }
}

void SystemManager::removeEntity(const EntityID &l_entity)
{
    for (auto &system : m_systems) {
        system.second->removeEntity(l_entity);
    }
}

void SystemManager::purgeEntities()
{
    for (auto &system : m_systems) {
        system.second->purge();
    }
}

void SystemManager::purgeSystems()
{
    for (auto &system : m_systems) {
        delete system.second;
    }
    m_systems.clear();
}



