#include "world.h"

#define SLEEP_TIME 1

int main()
{
    World world;
    sf::Clock clock;

    while (world.isRunning()) {
        world.update(clock.restart());
        sf::sleep(sf::milliseconds(SLEEP_TIME));
    }

    return 0;
}
