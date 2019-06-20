#include "s_base.h"
#include "systemmanager.h"


S_Base::S_Base(const System &l_id, SystemManager *l_systemMgr)
    : m_id(l_id)
    , m_systemManager(l_systemMgr)
{

}

S_Base::~S_Base()
{
    purge();
}

bool S_Base::addEntity(const EntityID &l_entity)
{
    if (hasEntity(l_entity)) {
        return false;
    }

    m_entities.emplace_back(l_entity);

    return true;
}

bool S_Base::hasEntity(const EntityID &l_entity)
{
    return std::find(m_entities.begin(), m_entities.end(), l_entity) != m_entities.end();
}

bool S_Base::removeEntity(const EntityID &l_entity)
{
    auto entity = std::find_if(m_entities.begin(), m_entities.end(), [&l_entity] (EntityID &id) {return id == l_entity;});

    if (entity == m_entities.end()) {
        return false;
    }

    m_entities.erase(entity);

    return true;
}

bool S_Base::fitsRequirements(const Bitmask &l_bits)
{
    return std::find_if(m_requiredComponents.begin(), m_requiredComponents.end(),
                        [&l_bits] (Bitmask &b) {return b.matches(l_bits, b.getMask());}) != m_requiredComponents.end();
}

void S_Base::purge()
{
    m_entities.clear();
}

