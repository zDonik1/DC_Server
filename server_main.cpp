#include "world.h"

int main()
{
    World world;
    sf::Clock clock;
    clock.restart();

    while (world.isRunning()) {
        world.update(clock.restart());
    }

    return 0;
}
