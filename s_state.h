#pragma once

#include "s_base.h"
#include "c_state.h"
#include "direction.h"

class S_State : public S_Base
{
public:
    S_State(SystemManager *l_systemMgr);

    void update(float l_dt);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
    void notify(const Message &l_message);

    EntityState getState(const EntityID &l_entity) const;
    void changeState(const EntityID &l_entity, const EntityState &l_state, const bool &l_force);
};
