#include "s_state.h"
#include "systemmanager.h"

S_State::S_State(SystemManager *l_systemMgr)
    : S_Base(System::State, l_systemMgr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::State);
    m_requiredComponents.push_back(req);

    m_systemManager->getMessageHandler()->subscribe(EntityMessage::Move, this);
    m_systemManager->getMessageHandler()->subscribe(EntityMessage::Switch_State, this);
}

void S_State::update(float l_dt)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        C_State *state = entities->getComponent<C_State>(entity, Component::State);
        if (state->getState() == EntityState::Walking) {
            Message msg((MessageType)EntityMessage::Is_Moving);
            msg.m_receiver = entity;
            m_systemManager->getMessageHandler()->dispatch(msg);
        }
    }
}

void S_State::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
    switch (l_event) {
    case EntityEvent::Became_Idle:
        changeState(l_entity, EntityState::Idle, false);
        break;
    }
}

void S_State::notify(const Message &l_message)
{
    if (!hasEntity(l_message.m_receiver)) {
        return;
    }

    EntityMessage m = (EntityMessage)l_message.m_type;
    switch (m) {
    case EntityMessage::Move:
    {
        C_State *state = m_systemManager->getEntityManager()->getComponent<C_State>(l_message.m_receiver, Component::State);
        if (state->getState() == EntityState::Dying) {
            return;
        }

        EntityEvent e;
        if (l_message.m_int == (int)Direction::Up) {
            e = EntityEvent::Moving_Up;
        }
        else if (l_message.m_int == (int)Direction::Down) {
            e = EntityEvent::Moving_Down;
        }
        else if (l_message.m_int == (int)Direction::Left) {
            e = EntityEvent::Moving_Left;
        }
        else if (l_message.m_int == (int)Direction::Right) {
            e = EntityEvent::Moving_Right;
        }

        m_systemManager->addEvent(l_message.m_receiver, (EventID)e);
        changeState(l_message.m_receiver, EntityState::Walking, false);
    }
        break;

    case EntityMessage::Switch_State:
        changeState(l_message.m_receiver, (EntityState)l_message.m_int, false);
        break;
    }
}

 EntityState S_State::getState(const EntityID &l_entity) const
{
    return m_systemManager->getEntityManager()->getComponent<C_State>(l_entity, Component::State)->getState();
}

void S_State::changeState(const EntityID &l_entity, const EntityState &l_state, const bool &l_force)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    C_State *state = entities->getComponent<C_State>(l_entity, Component::State);
    if (!l_force && state->getState() == EntityState::Dying) {
        return;
    }

    state->setState(l_state);
    Message msg((MessageType)EntityMessage::State_Changed);
    msg.m_receiver = l_entity;
    msg.m_int = (int)l_state;
    m_systemManager->getMessageHandler()->dispatch(msg);
}
