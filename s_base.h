#pragma once

#include <vector>
#include "entitymanager.h"
#include "entityevent.h"
#include "observer.h"

using EntityList = std::vector<EntityID>;
using Requirements = std::vector<Bitmask>;

class SystemManager;

class S_Base : public Observer
{
public:
    S_Base(const System &l_id, SystemManager *l_systemMgr);
    virtual ~S_Base();

    bool addEntity(const EntityID &l_entity);
    bool hasEntity(const EntityID &l_entity);
    bool removeEntity(const EntityID &l_entity);

    System getID();

    bool fitsRequirements(const Bitmask &l_bits);
    void purge();

    virtual void update(float l_dt) = 0;
    virtual void handleEvent(const EntityID &l_entity, const EntityEvent &l_event) = 0;

protected:
    System m_id;
    Requirements m_requiredComponents;
    EntityList m_entities;

    SystemManager *m_systemManager;
};
