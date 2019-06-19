#pragma once

#include "s_base.h"
#include "c_position.h"
#include "c_attacker.h"
#include "c_health.h"
#include "s_state.h"
#include "c_movable.h"

class S_Combat : public S_Base
{
public:
    S_Combat(SystemManager *l_systemMgr);
    ~S_Combat();

    void update(float l_dt);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
    void notify(const Message &l_message);
};

