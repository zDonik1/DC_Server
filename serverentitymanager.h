#pragma once

#include "entitymanager.h"
#include "c_position.h"
#include "c_state.h"
#include "c_movable.h"
#include "c_collidable.h"
#include "c_controller.h"
#include "c_client.h"
#include "c_health.h"
#include "c_name.h"
#include "c_attacker.h"
#include "entitysnapshot.h"

class ServerEntityManager : public EntityManager
{
public:
    ServerEntityManager(SystemManager *l_systemMgr);
    ~ServerEntityManager();

    void dumpEntityInfo(sf::Packet &l_packet);
};

