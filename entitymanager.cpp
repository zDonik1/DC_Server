#include "entitymanager.h"
#include "systemmanager.h"

EntityManager::EntityManager(SystemManager *l_sysMgr)
    : m_idCounter(0)
    , m_systemManager(l_sysMgr)
{
}

EntityManager::~EntityManager()
{
    purge();
}

int EntityManager::getEntityCount() const
{
    return static_cast<int>(m_entities.size());
}

void EntityManager::setSystemManager(SystemManager *l_systemMgr)
{
    m_systemManager = l_systemMgr;
}

int EntityManager::addEntity(const Bitmask &l_mask, int l_id)
{
    unsigned int entity = (l_id != -1 ? static_cast<unsigned int>(l_id) : m_idCounter);
    if (!m_entities.emplace(entity, EntityData()).second) {
        return -1;
    }

    if (l_id == -1) {
        ++m_idCounter;
    }

    for (int i = 0; i < N_COMPONENT_TYPES; ++i) {
        if (l_mask.getBit(static_cast<unsigned int>(i))) {
            addComponent(entity, static_cast<Component>(i));
        }
    }

    m_systemManager->entityModified(entity, l_mask);
    m_systemManager->addEvent(entity, static_cast<EventID>(EntityEvent::Spawned));

    return static_cast<int>(entity);
}

int EntityManager::addEntity(const std::string &l_entityFile)
{
    int entityID = -1;

    std::ifstream file;
    file.open(Utils::getWorkingDirectory() + "media/Entities/" + l_entityFile + ".entity");
    if (!file.is_open()) {
        std::cout << "ERROR::ENTITYMANAGER::ADDENTITY::ENTITY FILE " + l_entityFile + " FAILED TO LOAD" << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') {
            continue;
        }

        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Name") {

        }
        else if (type == "Attributes") {
            if (entityID != -1) {
                continue;
            }

            Bitset set = 0;
            Bitmask mask;
            keystream >> set;
            mask.setMask(set);
            entityID = addEntity(mask);
            if (entityID == -1) {
                return -1;
            }
        }
        else if (type == "Component") {
            if (entityID == -1) {
                continue;
            }

            unsigned int c_id = 0;
            keystream >> c_id;
            C_Base *component = getComponent<C_Base>(static_cast<unsigned int>(entityID), static_cast<Component>(c_id));
            if (!component) {
                continue;
            }

            keystream >> *component;
        }
    }

    file.close();
    m_entities.at(entityID).m_type = l_entityFile;
    return entityID;
}

bool EntityManager::removeEntity(const EntityID &l_id)
{
    auto itr = m_entities.find(l_id);
    if (itr == m_entities.end()) {
        return false;
    }

    Message msg(static_cast<MessageType>(EntityMessage::Remove_Entity));
    msg.m_receiver = static_cast<int>(l_id);
    msg.m_int = static_cast<int>(l_id);
    m_systemManager->getMessageHandler()->dispatch(msg);

    // Removing components
    while (itr->second.m_components.begin() != itr->second.m_components.end()) {
        delete itr->second.m_components.back();
        itr->second.m_components.pop_back();
    }

    m_entities.erase(itr);
    m_systemManager->removeEntity(l_id);

    return true;
}

bool EntityManager::hasEntity(const EntityID &l_id)
{
    return (m_entities.find(l_id) != m_entities.end());
}

bool EntityManager::addComponent(const EntityID &l_entity, const Component &l_component)
{
    auto itr = m_entities.find(l_entity);
    if (itr == m_entities.end()) {
        return false;
    }

    if (itr->second.m_bitmask.getBit(static_cast<unsigned int>(l_component))) {
        return false;
    }

    auto itr2 = m_cFactory.find(l_component);
    if (itr2 == m_cFactory.end()) {
        return false;
    }

    C_Base *component = itr2->second();
    itr->second.m_components.emplace_back(component);
    itr->second.m_bitmask.turnOnBit(static_cast<unsigned int>(l_component));

    m_systemManager->entityModified(l_entity, itr->second.m_bitmask);

    return true;
}

bool EntityManager::removeComponent(const EntityID &l_entity, const Component &l_component)
{
    auto itr = m_entities.find(l_entity);
    if (itr == m_entities.end()) {
        return false;
    }

    if (!itr->second.m_bitmask.getBit(static_cast<unsigned int>(l_component))) {
        return false;
    }

    auto &container = itr->second.m_components;
    auto component = std::find_if(container.begin(), container.end(),
            [&l_component] (C_Base *c) {
                return c->getType() == l_component;
            });

    if (component == container.end()) {
        return false;
    }

    delete (*component);
    container.erase(component);
    itr->second.m_bitmask.clearBit(static_cast<unsigned int>(l_component));

    m_systemManager->entityModified(l_entity, itr->second.m_bitmask);

    return true;
}

bool EntityManager::hasComponent(const EntityID &l_entity, const Component &l_component)
{
    auto itr = m_entities.find(l_entity);
    if (itr == m_entities.end()) {
        return false;
    }

    return itr->second.m_bitmask.getBit(static_cast<unsigned int>(l_component));
}

void EntityManager::purge()
{
    m_systemManager->purgeEntities();
    for (auto &entity : m_entities) {
        for (auto &component : entity.second.m_components) {
            delete component;
        }

        entity.second.m_components.clear();
        entity.second.m_bitmask.clear();
    }

    m_entities.clear();
    m_idCounter = 0;
}
