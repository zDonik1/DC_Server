#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <fstream>
#include "c_base.h"
#include "bitmask.h"
#include "utilities.h"

using EntityID = unsigned int;
using ComponentContainer = std::vector<C_Base*>;

struct EntityData
{
    Bitmask m_bitmask;
    std::string m_type;
    ComponentContainer m_components;
};

using ComponentContainer = std::vector<C_Base*>;
using EntityContainer = std::unordered_map<EntityID, EntityData>;
using ComponentFactory = std::unordered_map<Component, std::function<C_Base *(void)>>;

class SystemManager;
struct SharedContext;

class EntityManager
{
public:
    EntityManager(SystemManager *l_sysMgr);
    ~EntityManager();

    int getEntityCount() const;

    void setSystemManager(SystemManager *l_systemMgr);

    int addEntity(const Bitmask &l_mask, int l_id = -1);
    int addEntity(const std::string &l_entityFile);
    bool removeEntity(const EntityID &l_id);
    bool hasEntity(const EntityID &l_id);

    bool addComponent(const EntityID &l_entity, const Component &l_component);

    template<class T>
    T *getComponent(const EntityID &l_entity, const Component &l_component)
    {
        auto itr = m_entities.find(l_entity);
        if (itr == m_entities.end()) {
            return nullptr;
        }

        if (!itr->second.m_bitmask.getBit(static_cast<unsigned int>(l_component))) {
            return nullptr;
        }

        auto &container = itr->second.m_components;
        auto component = std::find_if(container.begin(), container.end(),
                                      [&l_component] (C_Base *c) {
                return c->getType() == l_component;
            });

        return (component != container.end() ? dynamic_cast<T*>(*component) : nullptr);
    }

    bool removeComponent(const EntityID &l_entity, const Component &l_component);
    bool hasComponent(const EntityID &l_entity, const Component &l_component);

    void purge();

protected:
    template<class T>
    void addComponentType(const Component& l_id)
    {
        m_cFactory[l_id] = [] () -> C_Base *{ return new T(); };
    }

    unsigned int m_idCounter;
    EntityContainer m_entities;
    ComponentFactory m_cFactory;

    SystemManager *m_systemManager;
};
