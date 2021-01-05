//--------------------------------------------------------------------------------
// Projectiles.cpp
//--------------------------------------------------------------------------------
// The class that manages the player and enemy projectiles
//--------------------------------------------------------------------------------
#include "Projectiles.hpp"

#include "ProjectileDefinition.hpp"

constexpr auto OfsY = 16;
void Projectiles::computeClusters(u32* clusterPos)
{
    // If we have too few sprites, no clustering is necessary, we can save a lot of cycles
    u32 maxObjsInCluster = MaxObjs - graphics::oam.getObjCount();
    if (numPlayerProjectiles + numEnemyProjectiles <= maxObjsInCluster)
    {
        clusterPos[0] = 512;
        return;
    }

    // Here, we need to do two things, to determine the projectile clusters *and* to push them to OAM
    // First step is, then, to define how many clusters we will have
    // Get the number of keys (ala counting sort)
    u16 arrBeg[SCREEN_HEIGHT + OfsY];
    s16 arrSim[SCREEN_HEIGHT + OfsY];
    memset32(arrBeg, 0, sizeof(arrBeg)/sizeof(u32));
    memset32(arrSim, 0, sizeof(arrSim)/sizeof(u32));

    auto insertPos = [&](int y, int h)
    {
        u32 yy = y + OfsY;
        if (yy > SCREEN_HEIGHT + OfsY) return;
        arrBeg[yy]++;
        arrSim[yy]++;
        if (yy+h > SCREEN_HEIGHT + OfsY) return;
        arrSim[yy+h]--;
    };

    for (u32 i = 0; i < numPlayerProjectiles; i++)
    {
        const auto& ptype = PlayerProjectileTypes[playerProjectiles[i].type];
        insertPos(playerProjectiles[i].pos.y - ptype.halfSize.y, 2 * ptype.halfSize.y);
    }
    for (u32 i = 0; i < numEnemyProjectiles; i++)
    {
        const auto& ptype = EnemyProjectileTypes[enemyProjectiles[i].type];
        insertPos(enemyProjectiles[i].pos.y - ptype.halfSize.y, 2 * ptype.halfSize.y);
    }

    // Now, we're going to compute the cumulative sum and use it to define the cluster
    u32 curVal = 0;
    s32 curSim = 0;
    u32 curCluster = 0;

    for (u32 i = 0; i < SCREEN_HEIGHT + OfsY; i++)
    {
        curVal += arrBeg[i];
        curSim += arrSim[i];

        // If the number of objects becomes too big, we create a new cluster
        if (curVal > maxObjsInCluster)
        {
            curVal = curSim;
            clusterPos[curCluster++] = i;
            if (curCluster == MaxClusters - 1) return; // The last cluster is the "catch all" one
        }
    }

    // Define a very large limit for the last cluster
    clusterPos[curCluster] = 512;
}

// Disable loop unrolling
#pragma GCC optimize ("no-unroll-loops")
#pragma GCC optimize ("no-peel-loops")

void Projectiles::fillOAM(const u32* clusterPos)
{
    constexpr auto MaxPriorities = 15;
    // Now, we're going to do a counting sort by priority (we can use an u8 here)
    u8 priosByCluster[MaxClusters][MaxPriorities+1];
    memset32(priosByCluster, 0, sizeof(priosByCluster)/sizeof(u32));

    auto insertPrioProj = [&](int y, int h, int prio)
    {
        u32 yy = y + OfsY;
        if (yy > SCREEN_HEIGHT + OfsY) return;

        u32 i = 0;
        while (yy >= clusterPos[i]) i++;
        do priosByCluster[i][prio+1]++;
        while (yy+h > clusterPos[i++]);
    };

    for (u32 i = 0; i < numPlayerProjectiles; i++)
    {
        const auto& ptype = PlayerProjectileTypes[playerProjectiles[i].type];
        insertPrioProj(playerProjectiles[i].pos.y - ptype.halfSize.y,
            2 * ptype.halfSize.y, ptype.attrs.prio);
    }

    for (u32 i = 0; i < numEnemyProjectiles; i++)
    {
        const auto& ptype = EnemyProjectileTypes[enemyProjectiles[i].type];
        insertPrioProj(enemyProjectiles[i].pos.y - ptype.halfSize.y,
            2 * ptype.halfSize.y, ptype.attrs.prio);
    }

    // Now, sum everything, using the non-naive sum
    for (u32 j = 0; j < MaxClusters; j++)
    {
#if 1
        auto ptr = reinterpret_cast<u32*>(&priosByCluster[j][0]);
        // The ptr has in this order: a[3] | a[2] | a[1] | a[0]

        u32 curSum = 0;
        for (u32 i = 0; i < (MaxPriorities+1) / sizeof(u32); i++)
        {
            u32 val = ptr[i];
            val += val << 8;    // a[3] + a[2] | a[2] + a[1] | a[1] + a[0] | a[0]
            val += val << 16;   // a[3] + a[2] + a[1] + a[0] | a[2] + a[1] + a[0] | a[1] + a[0] | a[0]
            val += curSum;
            ptr[i] = val;
            curSum = val >> 24; // a[3] + a[2] + a[1] + a[0]
            curSum += curSum << 8;
            curSum += curSum << 16;
        }
#else
        for (u32 i = 0; i < MaxPriorities; i++)
            priosByCluster[j][i+1] += priosByCluster[j][i];
#endif
    }

    // Now we post it to OAM
    projectileClusters[0] = 0;
    for (u32 i = 0; i < MaxClusters; i++)
        projectileClusters[i+1] = projectileClusters[i] + priosByCluster[i][MaxPriorities];
    
    auto sendToOam = [&](const Projectile& proj, const ProjectileType& ptype, u16 attr2add)
    {
        auto dp = vec2<s16>(proj.pos - ptype.halfSize); 
        u32 yy = OfsY + dp.y, h = 2 * ptype.halfSize.y;
        if (yy > SCREEN_HEIGHT + OfsY) return;

        // This loop ensures that you'll get the correct clusters, since they run from
        // [0, clusterPos[0]), [clusterPos[0], clusterPos[1]), ... and so on
        u32 i = 0;
        while (yy >= clusterPos[i]) i++;
        do
        {
            int oid = priosByCluster[i][ptype.attrs.prio]++;
            oid += projectileClusters[i];
            projectileOAM[oid].attr0 = ptype.attrs.attr0 | ATTR0_Y(dp.y&255);
            projectileOAM[oid].attr1 = ptype.attrs.attr1 | ATTR1_X(dp.x&511);
            projectileOAM[oid].attr2 = ptype.attrs.attr2 + attr2add;
        } while (yy+h > clusterPos[i++]);
    };

    u16 attr2add = ATTR2_ID(playerTilePtr.getTileId()) | ATTR2_PALBANK(playerPalPtr.getPalette());
    for (u32 i = 0; i < numPlayerProjectiles; i++)
        sendToOam(playerProjectiles[i], PlayerProjectileTypes[playerProjectiles[i].type], attr2add);

    attr2add = ATTR2_ID(enemyTilePtr.getTileId()) | ATTR2_PALBANK(enemyPalPtr.getPalette());
    for (u32 i = 0; i < numEnemyProjectiles; i++)
        sendToOam(enemyProjectiles[i], EnemyProjectileTypes[enemyProjectiles[i].type], attr2add);
}