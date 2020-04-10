//--------------------------------------------------------------------------------
// Projectile.iwram.cpp
//--------------------------------------------------------------------------------
// Base struct for projectile management
//--------------------------------------------------------------------------------
#include "Projectile.hpp"

u32 updateProjectiles(Projectile* projectiles, u32 numProjectiles)
{
    // Update each position and check if it's offscreen
    for (u32 i = 0; i < numProjectiles;)
    {
        projectiles[i].pos += projectiles[i].vel;

        // If it's offscreen, swap with a final projectile
        auto pos = projectiles[i].pos;
        auto size = projectiles[i].size;
        if (pos.y < -size.y/2 || pos.y > SCREEN_HEIGHT + size.y/2 || pos.x < -size.x/2 || pos.x > SCREEN_WIDTH + size.x/2)
            projectiles[i] = projectiles[--numProjectiles];
        else i++;
    }

    return numProjectiles;
}
