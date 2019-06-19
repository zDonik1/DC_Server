#pragma once

#include "s_base.h"
#include "c_attacker.h"
#include "c_health.h"
#include "s_state.h"

class S_Timers : public S_Base
{
public:
    S_Timers(SystemManager *l_systemMgr);
    ~S_Timers();

    void update(float l_dt);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
    void notify(const Message &l_message);
};

