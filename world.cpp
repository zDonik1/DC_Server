#include "world.h"

World::World()
    : m_commandThread(&World::commandLine, this)
    , m_server(&World::handlePacket, this)
    , m_entities(nullptr)
    , m_running(false)
    , m_map(&m_entities)
    , m_tick(0)
    , m_tps(0)
{
    if (!m_server.start()) {
        return;
    }

    m_running = true;
    m_systems.setEntityManager(&m_entities);
    m_entities.setSystemManager(&m_systems);
    m_map.loadMap("media/Maps/map1.map");
    m_systems.getSystem<S_Collision>(System::Collision)->setMap(&m_map);
    m_systems.getSystem<S_Movement>(System::Movement)->setMap(&m_map);
    m_systems.getSystem<S_Network>(System::Network)->registerServer(&m_server);
    m_server.bindTimeoutHandler(&World::clientLeave, this);
    m_commandThread.launch();
}

World::~World()
{
    m_entities.setSystemManager(nullptr);
}

void World::update(const sf::Time &l_time)
{
    if (!m_server.isRunning()) {
        m_running = false;
        return;
    }

    m_serverTime += l_time;
    m_snapshotTimer += l_time;
    m_tpsTime += l_time;

    m_server.update(l_time);
    m_systems.update(l_time.asSeconds());
    m_server.getMutex().unlock();
    if (m_snapshotTimer.asMilliseconds() >= SNAPSHOT_INTERVAL) {
        sf::Packet snapshot;
        m_systems.getSystem<S_Network>(System::Network)->createSnapshot(snapshot);
        m_server.broadcast(snapshot);
        m_snapshotTimer = sf::milliseconds(0);
    }

    if (m_tpsTime >= sf::milliseconds(1000)) {
        m_tps = m_tick;
        m_tick = 0;
        m_tpsTime = sf::milliseconds(0);
    }
    else {
        ++m_tick;
    }
}

void World::handlePacket(sf::IpAddress &l_ip, const PortNumber &l_port, const PacketID &l_id, sf::Packet &l_packet, Server *l_server)
{
    ClientID id = l_server->getClientID(l_ip, l_port);
    PacketType type = (PacketType)l_id;
    if (id >= 0) {
        if (type == PacketType::Disconnect) {
            std::cout << m_entities.getComponent<C_Name>(m_systems.getSystem<S_Network>(System::Network)->getEntityID(id), Component::Name)->getName()
                      << " disconnected" << std::endl;
            clientLeave(id);
            l_server->removeClient(l_ip, l_port);
        }
        else if (type == PacketType::Message) {

        }
        else if (type == PacketType::Player_Update) {
            m_systems.getSystem<S_Network>(System::Network)->updatePlayer(l_packet, id);
        }
    }
    else {
        if (type != PacketType::Connect) {
            return;
        }

        std::string nickname;
        if (!(l_packet >> nickname)) {
            return;
        }

        std::cout << nickname << " connected" << std::endl;
        ClientID cid = l_server->addClient(l_ip, l_port);
        if (cid == -1) {
            sf::Packet packet;
            stampPacket(PacketType::Disconnect, packet);
            l_server->send(l_ip, l_port, packet);
            return;
        }

        sf::Lock lock(m_server.getMutex());
        sf::Int32 eid = m_entities.addEntity("Player");
        if (eid == -1) {
            return;
        }

        m_systems.getSystem<S_Network>(System::Network)->registerClientID(eid, cid);
        C_Position *pos = m_entities.getComponent<C_Position>(eid, Component::Position);
        pos->setPosition(64.f, 64.f);
        m_entities.getComponent<C_Name>(eid, Component::Name)->setName(nickname);
        sf::Packet packet;
        stampPacket(PacketType::Connect, packet);
        packet << eid;
        packet << pos->getPosition().x << pos->getPosition().y;
        if (!l_server->send(cid, packet)) {
            std::cout << "Unable to send to connect packet!" << std::endl;
            return;
        }
    }
}

void World::clientLeave(const ClientID &l_client)
{
    sf::Lock lock(m_server.getMutex());
    S_Network *network = m_systems.getSystem<S_Network>(System::Network);
    m_entities.removeEntity(network->getEntityID(l_client));
}

void World::commandLine()
{
    while (m_server.isRunning()) {
        std::string str;
        std::getline(std::cin, str);
        if (str == "terminate") {
            m_server.stop();
            m_running = false;
            break;
        }
        else if (str == "disconnectall") {
            std::cout << "Disconnecting all clients..." << std::endl;
            m_server.disconnectAll();
            sf::Lock lock(m_server.getMutex());
            m_entities.purge();
        }
        else if (str.find("tps") != std::string::npos) {
            std::cout << "TPS: " << m_tps << std::endl;
        }
        else if (str == "clients") {
            std::cout << m_server.getClientCount() << " clients online:" << std::endl;
            std::cout << m_server.getClientList() << std::endl;
        }
        else if (str == "entities") {
            std::cout << "Current entity count: " << m_entities.getEntityCount() << std::endl;
        }
    }
}

bool World::isRunning()
{
    return m_running;
}
