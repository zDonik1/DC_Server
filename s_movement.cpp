#include "s_movement.h"
#include "systemmanager.h"
#include "map.h"

S_Movement::S_Movement(SystemManager *l_systemMgr)
    : S_Base(System::Movement, l_systemMgr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::Position);
    req.turnOnBit((unsigned int)Component::Movable);
    m_requiredComponents.push_back(req);
    req.clear();

    m_systemManager->getMessageHandler()->subscribe(EntityMessage::Is_Moving, this);

    m_gameMap = nullptr;
}

S_Movement::~S_Movement()
{
}

void S_Movement::update(float l_dt)
{
    if (!m_gameMap) {
        return;
    }

    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        C_Position *position = entities->getComponent<C_Position>(entity, Component::Position);
        C_Movable *movable = entities->getComponent<C_Movable>(entity, Component::Movable);
        movementStep(l_dt, movable, position);
        position->moveBy(movable->getVelocity() * l_dt);
    }
}

void S_Movement::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
    switch (l_event) {
    case EntityEvent::Colliding_X:
        stopEntity(l_entity, Axis::x);
        break;

    case EntityEvent::Colliding_Y:
        stopEntity(l_entity, Axis::y);
        break;

    case EntityEvent::Moving_Left:
        setDirection(l_entity, Direction::Left);
        break;

    case EntityEvent::Moving_Right:
        setDirection(l_entity, Direction::Right);
        break;

    case EntityEvent::Moving_Up:
    {
        C_Movable *mov = m_systemManager->getEntityManager()->getComponent<C_Movable>(l_entity, Component::Movable);
        if (mov->getVelocity().x == 0) {
            setDirection(l_entity, Direction::Up);
        }
    }
        break;

    case EntityEvent::Moving_Down:
    {
        C_Movable *mov = m_systemManager->getEntityManager()->getComponent<C_Movable>(l_entity, Component::Movable);
        if (mov->getVelocity().x == 0) {
            setDirection(l_entity, Direction::Down);
        }
    }
        break;
    }
}

void S_Movement::notify(const Message &l_message)
{
    EntityManager *eMgr = m_systemManager->getEntityManager();
    EntityMessage m = (EntityMessage)l_message.m_type;
    switch (m) {
    case EntityMessage::Is_Moving:
    {
        if (!hasEntity(l_message.m_receiver)) {
            return;
        }

        C_Movable *movable = eMgr->getComponent<C_Movable>(l_message.m_receiver, Component::Movable);
        if (movable->getVelocity() != sf::Vector2f(0.f, 0.f)) {
            return;
        }

        m_systemManager->addEvent(l_message.m_receiver, (EventID)EntityEvent::Became_Idle);
    }
        break;
    }
}

void S_Movement::setMap(Map *l_gameMap)
{
    m_gameMap = l_gameMap;
}

void S_Movement::stopEntity(const EntityID &l_entity, const Axis &l_axis)
{
    C_Movable *movable = m_systemManager->getEntityManager()->getComponent<C_Movable>(l_entity, Component::Movable);
    if (l_axis == Axis::x) {
        movable->setVelocity(sf::Vector2f(0.f, movable->getVelocity().y));
    }
    else if (l_axis == Axis::y) {
        movable->setVelocity(sf::Vector2f(movable->getVelocity().x, 0.f));
    }
}

void S_Movement::setDirection(const EntityID &l_entity, const Direction &l_dir)
{
    C_Movable *movable = m_systemManager->getEntityManager()->getComponent<C_Movable>(l_entity, Component::Movable);
    movable->setDirection(l_dir);

    Message msg((MessageType)EntityMessage::Direction_Changed);
    msg.m_receiver = l_entity;
    msg.m_int = (int)l_dir;
    m_systemManager->getMessageHandler()->dispatch(msg);
}

const sf::Vector2f &S_Movement::getTileFriction(unsigned int l_elevation, unsigned int l_x, unsigned int l_y)
{
    Tile *t = nullptr;
    while (!t && l_elevation >= 0) {
        t =  m_gameMap->getTile(l_x, l_y, l_elevation);
        --l_elevation;
    }

    return (t ? t->m_properties->m_friction : m_gameMap->getDefaultTile()->m_friction);
}

void S_Movement::movementStep(float l_dt, C_Movable *l_movable, C_Position *l_position)
{
    sf::Vector2f f_coefficient = getTileFriction(l_position->getElevation(),
                                                 floor(l_position->getPosition().x / Sheet::Tile_Size),
                                                 floor(l_position->getPosition().y / Sheet::Tile_Size));

    sf::Vector2f friction(l_movable->getSpeed().x * f_coefficient.x, l_movable->getSpeed().y * f_coefficient.y);

    l_movable->addVelocity(l_movable->getAcceleration() * l_dt);
    l_movable->setAcceleration(sf::Vector2f(0.f, 0.f));
    l_movable->applyFriction(friction * l_dt);

    float magnitude = sqrt((l_movable->getVelocity().x * l_movable->getVelocity().x) +
                           (l_movable->getVelocity().y * l_movable->getVelocity().y));

    if (magnitude <= l_movable->getMaxVelocity()) {
        return;
    }

    float max_V = l_movable->getMaxVelocity();
    l_movable->setVelocity(sf::Vector2f((l_movable->getVelocity().x / magnitude) * max_V,
                                        (l_movable->getVelocity().y / magnitude) * max_V));
}
