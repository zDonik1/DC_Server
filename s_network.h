#pragma once

#include "s_base.h"
#include "networkdefinitions.h"
#include "server.h"
#include "serverentitymanager.h"
#include "c_client.h"
#include "s_state.h"
#include "s_movement.h"

struct PlayerInput
{
    int m_movedX;
    int m_movedY;
    bool m_attacking;

    PlayerInput()
        : m_movedX(0)
        , m_movedY(0)
        , m_attacking(false)
    {
    }
};

using PlayerInputContainer = std::unordered_map<EntityID, PlayerInput>;

class S_Network : public S_Base
{
public:
    S_Network(SystemManager *l_systemMgr);
    ~S_Network();

    void update(float l_dt);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
    void notify(const Message &l_message);

    bool registerClientID(const EntityID &l_entity, const ClientID &l_client);
    void registerServer(Server *l_server);
    ClientID getClientID(const EntityID &l_entity);
    EntityID getEntityID(const ClientID &l_client);

    void createSnapshot(sf::Packet &l_packet);
    void updatePlayer(sf::Packet &l_packet, const ClientID &l_client);

private:
    PlayerInputContainer m_playerInput;
    Server *m_server;
};

