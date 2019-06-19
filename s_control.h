#pragma once

#include "s_base.h"
#include "direction.h"
#include "c_movable.h"

class S_Control : public S_Base
{
public:
    S_Control(SystemManager *l_systemMgr);

    void update(float l_dt);
    void handleEvent(const EntityID &l_entity, const EntityEvent &l_event);
    void notify(const Message &l_message);

private:
    void moveEntity(const EntityID &l_entity, const Direction &l_dir);
};
