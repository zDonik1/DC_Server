#pragma once

#include <unordered_map>
#include <functional>
#include <iostream>
#include "packettypes.h"
#include "networkdefinitions.h"

#define HEARTBEAT_INTERVAL 1000
#define HEARTBEAT_RETRIES 5

struct ClientInfo
{
    sf::IpAddress m_clientIP;
    PortNumber m_clientPORT;
    sf::Time m_lastHeartbeat;
    sf::Time m_heartbeatSent;
    bool m_heartbeatWaiting;
    unsigned short m_heartbeatRetry;
    unsigned int m_ping;

    ClientInfo(const sf::IpAddress &l_ip, const PortNumber &l_port, const sf::Time &l_heartbeat)
        : m_clientIP(l_ip)
        , m_clientPORT(l_port)
        , m_lastHeartbeat(l_heartbeat)
        , m_heartbeatWaiting(false)
        , m_heartbeatRetry(0)
        , m_ping(0)
    {
    }

    ClientInfo &operator =(const ClientInfo &l_rhs)
    {
        m_clientIP = l_rhs.m_clientIP;
        m_clientPORT = l_rhs.m_clientPORT;
        m_lastHeartbeat = l_rhs.m_lastHeartbeat;
        m_heartbeatSent = l_rhs.m_heartbeatSent;
        m_heartbeatWaiting = l_rhs.m_heartbeatWaiting;
        m_heartbeatRetry = l_rhs.m_heartbeatRetry;
        m_ping = l_rhs.m_ping;
        return *this;
    }
};

using Clients = std::unordered_map<ClientID, ClientInfo>;
class Server;
using PacketHandler = std::function<void(sf::IpAddress&, const PortNumber&, const PacketID&, sf::Packet&, Server*)>;
using TimeoutHandler = std::function<void(const ClientID&)>;

class Server
{
public:
    template<class T>
    Server(void(T::*l_handler)(sf::IpAddress&, const PortNumber&, const PacketID&, sf::Packet&, Server*), T *l_instance)
        : m_listenThread(&Server::listen, this)
    {
        m_packetHandler = std::bind(l_handler, l_instance, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        setup();
    }

    Server(void(*l_handler)(sf::IpAddress&, const PortNumber&, const PacketID&, sf::Packet&, Server*));
    ~Server();

    template<class T>
    void bindTimeoutHandler(void(T::*l_handler)(const ClientID&), T* l_instance)
    {
        m_timeoutHandler = std::bind(l_handler, l_instance, std::placeholders::_1);
    }
    void bindTimeoutHandler(void(*l_handler)(const ClientID));

    bool send(const ClientID &l_id, sf::Packet &l_packet);
    bool send(sf::IpAddress &l_ip, const PortNumber &l_port, sf::Packet &l_packet);
    void broadcast(sf::Packet &l_packet, const ClientID &l_ignore = ClientID(Network::NullID));

    void listen();
    void update(const sf::Time &l_time);

    ClientID addClient(const sf::IpAddress &l_ip, const PortNumber &l_port);
    ClientID getClientID(const sf::IpAddress &l_ip, const PortNumber &l_port);
    bool hasClient(const ClientID &l_id);
    bool hasClient(const sf::IpAddress &l_ip, const PortNumber &l_port);
    bool getClientInfo(const ClientID &l_id, ClientInfo &l_info);
    bool removeClient(const ClientID &l_id);
    bool removeClient(const sf::IpAddress &l_ip, const PortNumber &l_port);

    void disconnectAll();
    bool start();
    bool stop();

    bool isRunning();

    unsigned int getClientCount();
    std::string getClientList();

    sf::Mutex &getMutex();

private:
    void setup();

    ClientID m_lastID;

    sf::UdpSocket m_incoming;
    sf::UdpSocket m_outgoing;

    PacketHandler m_packetHandler;
    TimeoutHandler m_timeoutHandler;

    Clients m_clients;
    sf::Time m_serverTime;

    bool m_running;

    sf::Thread m_listenThread;
    sf::Mutex m_mutex;

    size_t m_totalSent;
    size_t m_totalReceived;
};

