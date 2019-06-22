#include "world.h"

#define UPDATE_TIMESTEP 1

int main()
{
    World world;
    sf::Clock clock;
    sf::Time elapsed = clock.restart();

    while (world.isRunning()) {
        if (elapsed.asMilliseconds() >= UPDATE_TIMESTEP) {
            world.update(clock.restart());
            elapsed -= sf::milliseconds(UPDATE_TIMESTEP);
            sf::sleep(sf::milliseconds(UPDATE_TIMESTEP));
        }

        elapsed += clock.restart();
    }

    return 0;
}
