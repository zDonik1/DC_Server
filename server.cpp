#include "server.h"

Server::Server(void (*l_handler)(sf::IpAddress &, const PortNumber &, const PacketID &, sf::Packet &, Server *))
    : m_listenThread(&Server::listen, this)
{
    m_packetHandler = std::bind(l_handler, std::placeholders::_1, std::placeholders::_2,
                                std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    setup();
}

Server::~Server()
{
    stop();
}

void Server::bindTimeoutHandler(void (*l_handler)(const ClientID))
{
    m_timeoutHandler = std::bind(l_handler, std::placeholders::_1);
}

bool Server::send(const ClientID &l_id, sf::Packet &l_packet)
{
    sf::Lock lock(m_mutex);
    auto itr = m_clients.find(l_id);
    if (itr == m_clients.end()) {
        return false;
    }

    if (m_outgoing.send(l_packet, itr->second.m_clientIP, itr->second.m_clientPORT) != sf::Socket::Done) {
        std::cout << "Error sending a packet..." << std::endl;
        return false;
    }

    m_totalSent += l_packet.getDataSize();
    return true;
}

bool Server::send(sf::IpAddress &l_ip, const PortNumber &l_port, sf::Packet &l_packet)
{
    if (m_outgoing.send(l_packet, l_ip, l_port) != sf::Socket::Done) {
        return false;
    }

    m_totalSent += l_packet.getDataSize();
    return true;
}

void Server::broadcast(sf::Packet &l_packet, const ClientID &l_ignore)
{
    sf::Lock lock(m_mutex);
    for (auto &itr : m_clients) {
        if (itr.first != l_ignore) {
            if (m_outgoing.send(l_packet, itr.second.m_clientIP, itr.second.m_clientPORT) != sf::Socket::Done) {
                std::cout << "Error broadcasting a packet to client: " << itr.first << std::endl;
                continue;
            }

            m_totalSent += l_packet.getDataSize();
        }
    }
}

void Server::listen()
{
    sf::IpAddress ip;
    PortNumber port;
    sf::Packet packet;
    std::cout << "Beginning to listen..." << std::endl;
    while (m_running) {
        packet.clear();
        sf::Socket::Status status = m_incoming.receive(packet, ip, port);
        if (status != sf::Socket::Done) {
            if (m_running) {
                std::cout << "Error receiving a packet from: " << ip << ":" << port << ". Code: " << status << std::endl;
                continue;
            }
            else {
                std::cout << "Socket unbound." << std::endl;
                break;
            }
        }

        m_totalReceived += packet.getDataSize();

        PacketID p_id;
        if (!(packet >> p_id)) {
            continue;
        }

        PacketType id = (PacketType)p_id;
        if (id < PacketType::Disconnect || id >= PacketType::OutOfBounds) {
            continue;
        }

        if (id == PacketType::Heartbeat) {
            sf::Lock lock(m_mutex);
            for (auto &itr : m_clients) {
                if (itr.second.m_clientIP != ip || itr.second.m_clientPORT != port) {
                    continue;
                }

                if (!itr.second.m_heartbeatWaiting) {
                    std::cout << "Invalid heartbeat packet received!" << std::endl;
                    break;
                }

                itr.second.m_ping = m_serverTime.asMilliseconds() - itr.second.m_heartbeatSent.asMilliseconds();
                itr.second.m_lastHeartbeat = m_serverTime;
                itr.second.m_heartbeatWaiting = false;
                itr.second.m_heartbeatRetry = 0;
                break;
            }
        }
        else if (m_packetHandler) {
            m_packetHandler(ip, port, (PacketID)id, packet, this);
        }
    }
}

void Server::update(const sf::Time &l_time)
{
    m_serverTime += l_time;
    if (m_serverTime.asMilliseconds() < 0) {
        m_serverTime -= sf::milliseconds((int)Network::HighestTimestamp);
        sf::Lock lock(m_mutex);
        for (auto &itr : m_clients) {
            itr.second.m_lastHeartbeat = sf::milliseconds(std::abs(itr.second.m_lastHeartbeat.asMilliseconds() - (int)Network::HighestTimestamp));
        }
    }

    sf::Lock lock(m_mutex);
    for (auto itr = m_clients.begin(); itr != m_clients.end();) {
        sf::Int32 elapsed = m_serverTime.asMilliseconds() - itr->second.m_lastHeartbeat.asMilliseconds();
        if (elapsed >= HEARTBEAT_INTERVAL) {
            if (elapsed >= (int)Network::ClientTimeout || itr->second.m_heartbeatRetry > HEARTBEAT_RETRIES) {
                std::cout << "Client " << itr->first << " has timed out." << std::endl;
                if (m_timeoutHandler) {
                    m_timeoutHandler(itr->first);
                }
                itr = m_clients.erase(itr);
                continue;
            }

            if (!itr->second.m_heartbeatWaiting || (elapsed >= HEARTBEAT_INTERVAL * (itr->second.m_heartbeatRetry + 1))) {
                if (itr->second.m_heartbeatRetry >= 3) {
                    std::cout << "Re-try(" << itr->second.m_heartbeatRetry << ") heartbeat for client " << itr->first << std::endl;
                }

                sf::Packet heartbeat;
                stampPacket(PacketType::Heartbeat, heartbeat);
                heartbeat << m_serverTime.asMilliseconds();
                send(itr->first, heartbeat);
                if (itr->second.m_heartbeatRetry == 0) {
                    itr->second.m_heartbeatSent = m_serverTime;
                }
                itr->second.m_heartbeatWaiting = true;
                ++itr->second.m_heartbeatRetry;

                m_totalSent += heartbeat.getDataSize();
            }
        }
        ++itr;
    }
}

ClientID Server::addClient(const sf::IpAddress &l_ip, const PortNumber &l_port)
{
    sf::Lock lock(m_mutex);
    if (m_clients.empty()) {
        ClientID id = m_lastID;
        ClientInfo info(l_ip, l_port, m_serverTime);
        m_clients.insert(std::make_pair(id, info));
        ++m_lastID;
        return id;
    }
    for (auto &itr : m_clients) {
        if (itr.second.m_clientIP == l_ip && itr.second.m_clientPORT == l_port) {
            return ClientID(Network::NullID);
        }

        ClientID id = m_lastID;
        ClientInfo info(l_ip, l_port, m_serverTime);
        m_clients.insert(std::make_pair(id, info));
        ++m_lastID;
        return id;
    }
}

ClientID Server::getClientID(const sf::IpAddress &l_ip, const PortNumber &l_port)
{
    sf::Lock lock(m_mutex);
    for (auto &itr : m_clients) {
        if (itr.second.m_clientIP == l_ip && itr.second.m_clientPORT == l_port) {
            return itr.first;
        }
    }

    return ClientID(Network::NullID);
}

bool Server::hasClient(const ClientID &l_id)
{
    return (m_clients.find(l_id) != m_clients.end());
}

bool Server::hasClient(const sf::IpAddress &l_ip, const PortNumber &l_port)
{
    return (getClientID(l_ip, l_port) >= 0);
}

bool Server::getClientInfo(const ClientID &l_id, ClientInfo &l_info)
{
    sf::Lock lock(m_mutex);
    for (auto &itr : m_clients) {
        if (itr.first == l_id) {
            l_info = itr.second;
            return true;
        }
    }

    return false;
}

bool Server::removeClient(const ClientID &l_id)
{
    sf::Lock lock(m_mutex);
    auto itr = m_clients.find(l_id);
    if (itr == m_clients.end()) {
        return false;
    }

    sf::Packet p;
    stampPacket(PacketType::Disconnect, p);
    send(l_id, p);
    m_clients.erase(itr);
    return true;
}

bool Server::removeClient(const sf::IpAddress &l_ip, const PortNumber &l_port)
{
    sf::Lock lock(m_mutex);
    for (auto itr = m_clients.begin(); itr != m_clients.end(); ++itr) {
        if (itr->second.m_clientIP == l_ip && itr->second.m_clientPORT == l_port) {
            sf::Packet p;
            stampPacket(PacketType::Disconnect, p);
            send(itr->first, p);
            m_clients.erase(itr);
            return true;
        }
    }

    return false;
}

void Server::disconnectAll()
{
    if (!m_running) {
        return;
    }

    sf::Packet p;
    stampPacket(PacketType::Disconnect, p);
    broadcast(p);
    sf::Lock lock(m_mutex);
    m_clients.clear();
}

bool Server::start()
{
    if (m_running) {
        return false;
    }

    if (m_incoming.bind((unsigned short)Network::ServerPort) != sf::Socket::Done) {
        return false;
    }

    m_outgoing.bind(sf::Socket::AnyPort);
    setup();
    std::cout << "Incoming port: " << m_incoming.getLocalPort() << ". Outgoing port: " << m_outgoing.getLocalPort() << std::endl;
    m_listenThread.launch();
    m_running = true;
    return true;
}

bool Server::stop()
{
    if (!m_running) {
        return false;
    }

    disconnectAll();
    m_running = false;
    m_incoming.unbind();
    return true;
}

bool Server::isRunning()
{
    return m_running;
}

unsigned int Server::getClientCount()
{
    unsigned int count = 0;
    for (auto itr : m_clients) {
        ++count;
    }

    return count;
}

std::string Server::getClientList()
{
    std::string clientList = "";
    std::string delimiter = "--------------------------------------";
    clientList = delimiter + "\n";
    clientList += "ID \t";
    clientList += "Client IP:PORT \t \t";
    clientList += "Ping \n";
    clientList += delimiter + "\n";
    for (auto &client : m_clients) {
        clientList += std::to_string(client.first) + "\t";
        clientList += client.second.m_clientIP.toString() + ":" + std::to_string(client.second.m_clientPORT) + "\t \t";
        clientList += std::to_string(client.second.m_ping) + "ms. \n";
    }
    clientList += delimiter + "\n";
    clientList += "Total data sent: " + std::to_string(m_totalSent / 1000) + "kB. \n";
    clientList += "Total data received: " + std::to_string(m_totalReceived / 1000) + "kB";
    return clientList;
}

sf::Mutex &Server::getMutex()
{
    return m_mutex;
}

void Server::setup()
{
    m_lastID = 0;
    m_running = false;
    m_totalSent = 0;
    m_totalReceived = 0;
}

