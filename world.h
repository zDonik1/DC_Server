#pragma once

#include "server.h"
#include "serverentitymanager.h"
#include "serversystemmanager.h"
#include "map.h"
#include "netsettings.h"

class World
{
public:
    World();
    ~World();

    void update(const sf::Time &l_time);
    void handlePacket(sf::IpAddress &l_ip, const PortNumber &l_port, const PacketID &l_id, sf::Packet &l_packet, Server *l_server);
    void clientLeave(const ClientID &l_client);
    void commandLine();

    bool isRunning();

private:
    sf::Time m_tpsTime;
    sf::Time m_serverTime;
    sf::Time m_snapshotTimer;
    sf::Thread m_commandThread;
    Server m_server;
    ServerSystemManager m_systems;
    ServerEntityManager m_entities;
    bool m_running;

    Map m_map;
    unsigned int m_tick;
    unsigned int m_tps;
};

