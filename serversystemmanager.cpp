#include "serversystemmanager.h"

ServerSystemManager::ServerSystemManager(Server *l_server)
    : SystemManager(l_server)
{
    addSystem<S_Network>(System::Network);
    addSystem<S_State>(System::State);
    addSystem<S_Control>(System::Control);
    addSystem<S_Movement>(System::Movement);
    addSystem<S_Timers>(System::Timers);
    addSystem<S_Collision>(System::Collision);
    addSystem<S_Combat>(System::Combat);
}

ServerSystemManager::~ServerSystemManager()
{
}
