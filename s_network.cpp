#include "s_network.h"
#include "systemmanager.h"

S_Network::S_Network(SystemManager *l_systemMgr)
    : S_Base(System::Network, l_systemMgr)
{
    Bitmask req;
    req.turnOnBit((unsigned int)Component::Client);
    m_requiredComponents.push_back(req);

    MessageHandler *messageHandler = m_systemManager->getMessageHandler();
    messageHandler->subscribe(EntityMessage::Remove_Entity, this);
    messageHandler->subscribe(EntityMessage::Hurt, this);
    messageHandler->subscribe(EntityMessage::Respawn, this);
}

S_Network::~S_Network()
{
}

void S_Network::update(float l_dt)
{
    EntityManager *entities = m_systemManager->getEntityManager();
    for (auto &entity : m_entities) {
        auto &player = m_playerInput[entity];
        if (player.m_movedX || player.m_movedY) {
            if (player.m_movedX) {
                Message msg((MessageType)EntityMessage::Move);
                msg.m_receiver = entity;
                if (player.m_movedX > 0) {
                    msg.m_int = (int)Direction::Right;
                }
                else {
                    msg.m_int = (int)Direction::Left;
                }
            }

            if (player.m_movedY) {
                Message msg((MessageType)EntityMessage::Move);
                msg.m_receiver = entity;
                if (player.m_movedY > 0) {
                    msg.m_int = (int)Direction::Down;
                }
                else {
                    msg.m_int = (int)Direction::Up;
                }
                m_systemManager->getMessageHandler()->dispatch(msg);
            }
        }
        if (player.m_attacking) {
            Message msg((MessageType)EntityMessage::Attack);
            msg.m_receiver = entity;
            m_systemManager->getMessageHandler()->dispatch(msg);
        }
    }
}

void S_Network::handleEvent(const EntityID &l_entity, const EntityEvent &l_event)
{
}

void S_Network::notify(const Message &l_message)
{
    if (!hasEntity(l_message.m_receiver)) {
        return;
    }

    EntityMessage m = EntityMessage(l_message.m_type);
    if (m == EntityMessage::Remove_Entity) {
        m_playerInput.erase(l_message.m_receiver);
        return;
    }

    if (m == EntityMessage::Hurt) {
        sf::Packet packet;
        stampPacket(PacketType::Hurt, packet);
        packet << l_message.m_receiver;
        m_server->broadcast(packet);
        return;
    }

    if (m == EntityMessage::Respawn) {
        C_Position *position = m_systemManager->getEntityManager()->getComponent<C_Position>(l_message.m_receiver, Component::Position);
        if (!position) {
            return;
        }

        position->setPosition(64.f, 64.f);
        position->setElevation(1);
    }
}

bool S_Network::registerClientID(const EntityID &l_entity, const ClientID &l_client)
{
    if (!hasEntity(l_entity)) {
        return false;
    }

    m_systemManager->getEntityManager()->getComponent<C_Client>(l_entity, Component::Client)->setClientID(l_client);
    return true;
}

void S_Network::registerServer(Server *l_server)
{
    m_server = l_server;
}

ClientID S_Network::getClientID(const EntityID &l_entity)
{
    if (!hasEntity(l_entity)) {
        return (ClientID)Network::NullID;
    }

    return m_systemManager->getEntityManager()->getComponent<C_Client>(l_entity, Component::Client)->getClientID();
}

EntityID S_Network::getEntityID(const ClientID &l_client)
{
    EntityManager *e = m_systemManager->getEntityManager();
    auto entity = std::find_if(m_entities.begin(), m_entities.end(), [&e, &l_client] (EntityID &id)
    {
        return e->getComponent<C_Client>(id, Component::Client)->getClientID() == l_client;
    });

    return (entity != m_entities.end() ? *entity : (EntityID)Network::NullID);
}

void S_Network::createSnapshot(sf::Packet &l_packet)
{
    sf::Lock lock(m_server->getMutex());
    ServerEntityManager *e = (ServerEntityManager*)m_systemManager->getEntityManager();
    stampPacket(PacketType::Snapshot, l_packet);
    l_packet << sf::Int32(e->getEntityCount());
    if (e->getEntityCount()) {
        e->dumpEntityInfo(l_packet);
    }
}

void S_Network::updatePlayer(sf::Packet &l_packet, const ClientID &l_client)
{
    sf::Lock lock(m_server->getMutex());
    EntityID eid = getEntityID(l_client);
    if (eid == -1) {
        return;
    }

    if (!hasEntity(eid)) {
        return;
    }

    sf::Int8 entity_message;
    m_playerInput[eid].m_attacking = false;
    while (l_packet >> entity_message) {
        switch (entity_message) {
        case sf::Int8(EntityMessage::Move):
        {
            sf::Int32 x = 0, y = 0;
            l_packet >> x >> y;
            m_playerInput[eid].m_movedX = x;
            m_playerInput[eid].m_movedY = y;
            break;
        }

        case sf::Int8(EntityMessage::Attack):
        {
            sf::Int8 attackState;
            l_packet >> attackState;
            if (attackState) {
                m_playerInput[eid].m_attacking = true;
            }
            break;
        }
        }

        sf::Int8 delim = 0;
        if (!(l_packet >> delim) || delim != (sf::Int8)Network::PlayerUpdateDelim) {
            std::cout << "Faulty update!" << std::endl;
            break;
        }
    }
}

