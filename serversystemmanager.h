#pragma once

#include "systemmanager.h"
#include "s_movement.h"
#include "s_collision.h"
#include "s_state.h"
#include "s_control.h"
#include "s_network.h"
#include "s_combat.h"
#include "s_timers.h"

class ServerSystemManager : public SystemManager
{
public:
    ServerSystemManager();
    ~ServerSystemManager();
};

