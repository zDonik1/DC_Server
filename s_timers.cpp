#include "s_timers.h"
#include "systemmanager.h"

S_Timers::S_Timers(SystemManager *l_systemMgr)
    : S_Base(System::Timers, l_systemMgr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::State);
    req.turnOnBit((unsigned int)Component::Attacker);
    m_requiredComponents.push_back(req);

    req.clearBit((unsigned int)Component::Attacker);
    req.turnOnBit((unsigned int)Component::Health);
    m_requiredComponents.push_back(req);
}

S_Timers::~S_Timers()
{
}

void S_Timers::update(float l_dt)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        EntityState state = entities->getComponent<C_State>(entity, Component::State)->getState();
        if (state == EntityState::Attacking) {
            C_Attacker *attack = entities->getComponent<C_Attacker>(entity, Component::Attacker);
            attack->addToTimer(sf::seconds(l_dt));
            if (attack->getTimer().asMilliseconds() < attack->getAttackDuration()) {
                continue;
            }

            attack->reset();
            attack->setAttacked(false);
        }
        else if (state == EntityState::Hurt || state == EntityState::Dying) {
            C_Health *health = entities->getComponent<C_Health>(entity, Component::Health);
            health->addToTimer(sf::seconds(l_dt));
            if ((state == EntityState::Hurt && health->getTimer().asMilliseconds() < health->getHurtDuration()) ||
                    (state == EntityState::Dying && health->getTimer().asMilliseconds() < health->getDeathDuration()))
            {
                continue;
            }

            health->reset();
            if (state == EntityState::Dying) {
                Message msg((MessageType)EntityMessage::Respawn);
                msg.m_receiver = entity;
                m_systemManager->getMessageHandler()->dispatch(msg);
                health->resetHealth();
            }
        }
    }
}

void S_Timers::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
}

void S_Timers::notify(const Message &l_message)
{
}


