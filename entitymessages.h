#pragma once

enum class EntityMessage
{
    Move,
    Is_Moving,
    Frame_Change,
    State_Changed,
    Direction_Changed,
    Attack,
    Attack_Action,
    Being_Attacked,
    Hurt,
    Dead,
    Respawn,
    Remove_Entity,
    Switch_State
};
