#pragma once

#include <SFML/Network.hpp>

using PacketID = sf::Int8;

enum class PacketType
{
    Disconnect = -1,
    Connect,
    Heartbeat,
    Snapshot,
    Player_Update,
    Message,
    Hurt,
    OutOfBounds
};

void stampPacket(const PacketType &l_type, sf::Packet &l_packet);
