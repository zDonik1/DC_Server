#include "s_control.h"
#include "systemmanager.h"

S_Control::S_Control(SystemManager *l_systemMgr)
    : S_Base(System::Control, l_systemMgr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::Position);
    req.turnOnBit((unsigned int)Component::Movable);
    req.turnOnBit((unsigned int)Component::Controller);
    m_requiredComponents.push_back(req);
    req.clear();
}

void S_Control::update(float l_dt)
{

}

void S_Control::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
    switch (l_event) {
    case EntityEvent::Moving_Left:
        moveEntity(l_entity, Direction::Left);
        break;

    case EntityEvent::Moving_Right:
        moveEntity(l_entity, Direction::Right);
        break;

    case EntityEvent::Moving_Up:
        moveEntity(l_entity, Direction::Up);
        break;

    case EntityEvent::Moving_Down:
        moveEntity(l_entity, Direction::Down);
        break;
    }
}

void S_Control::notify(const Message &l_message)
{

}

void S_Control::moveEntity(const EntityID &l_entity, const Direction &l_dir)
{
    C_Movable *mov = m_systemManager->getEntityManager()->getComponent<C_Movable>(l_entity, Component::Movable);
    mov->move(l_dir);
}
