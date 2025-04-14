#include "world/world.h"

World::World()
{
}

void World::update()
{
    m_chunkMap.update();
}