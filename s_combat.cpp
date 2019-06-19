#include "s_combat.h"
#include "systemmanager.h"

S_Combat::S_Combat(SystemManager *l_systemMgr)
    : S_Base(System::Combat, l_systemMgr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::Position);
    req.turnOnBit((unsigned int)Component::Movable);
    req.turnOnBit((unsigned int)Component::State);
    req.turnOnBit((unsigned int)Component::Health);
    m_requiredComponents.push_back(req);

    req.clearBit((unsigned int)Component::Health);
    req.turnOnBit((unsigned int)Component::Attacker);
    m_requiredComponents.push_back(req);

    m_systemManager->getMessageHandler()->subscribe(EntityMessage::Being_Attacked, this);
}

S_Combat::~S_Combat()
{
}

void S_Combat::update(float l_dt)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        C_Attacker *attack = entities->getComponent<C_Attacker>(entity, Component::Attacker);
        if (!attack) {
            continue;
        }

        sf::Vector2f offset = attack->getOffset();
        sf::FloatRect AoA = attack->getAreaOfAttack();
        Direction dir = entities->getComponent<C_Movable>(entity, Component::Movable)->getDirection();
        sf::Vector2f position = entities->getComponent<C_Position>(entity, Component::Position)->getPosition();
        if (dir == Direction::Left) {
            offset.x -= AoA.width / 2;
        }
        else if (dir == Direction::Right) {
            offset.x += AoA.width / 2;
        }
        else if (dir == Direction::Up) {
            offset.y -= AoA.height / 2;
        }
        else if (dir == Direction::Down) {
            offset.y += AoA.height / 2;
        }

        /// TO DO: position -= sf::Vector2f(AoA.width / 2, AoA.height / 2);

        attack->setAreaPosition(position + offset);
    }
}

void S_Combat::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
}

void S_Combat::notify(const Message &l_message)
{
    if (!hasEntity(l_message.m_receiver) || !hasEntity(l_message.m_sender)) {
        return;
    }

    EntityManager *entities = m_systemManager->getEntityManager();
    EntityMessage m = (EntityMessage)l_message.m_type;
    switch (m) {
    case EntityMessage::Being_Attacked:
        C_Health *victim = entities->getComponent<C_Health>(l_message.m_receiver, Component::Health);
        C_Attacker *attacker = entities->getComponent<C_Attacker>(l_message.m_sender, Component::Attacker);
        if (!victim || !attacker) {
            return;
        }

        S_State *stateSystem = m_systemManager->getSystem<S_State>(System::State);
        if (stateSystem->getState(l_message.m_sender) != EntityState::Attacking) {
            return;
        }

        if (attacker->hasAttacked()) {
            return;
        }

        // Begin attacking
        victim->setHealth((victim->getHeatlh() > 1 ? victim->getHeatlh() - 1 : 0));
        attacker->setAttacked(true);
        if (!victim->getHeatlh()) {
            stateSystem->changeState(l_message.m_receiver, EntityState::Dying, true);
        }
        else {
            Message msg((MessageType)EntityMessage::Hurt);
            msg.m_receiver = l_message.m_receiver;
            m_systemManager->getMessageHandler()->dispatch(msg);
        }

        // Knockback
        Direction attackerDirection = entities->getComponent<C_Movable>(l_message.m_sender, Component::Movable)->getDirection();
        float knockback = attacker->getKnockback();
        sf::Vector2f knockbackVelocity;
        if (attackerDirection == Direction::Left || attackerDirection == Direction::Up) {
            knockback = -knockback;
        }

        if (attackerDirection == Direction::Left || attackerDirection == Direction::Right) {
            knockbackVelocity.x = knockback;
        }
        else {
            knockbackVelocity.y = knockback;
        }

        entities->getComponent<C_Movable>(l_message.m_receiver, Component::Movable)->setVelocity(knockbackVelocity);
        break;
    }
}


