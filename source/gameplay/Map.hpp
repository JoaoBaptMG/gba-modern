//--------------------------------------------------------------------------------
// Map.hpp
//--------------------------------------------------------------------------------
// Class that handles one of the backgrounds in order to provide a tilemap
//--------------------------------------------------------------------------------
#pragma once

#include "tonc.h"
#include "data/MapData.hpp"
#include "Position.hpp"
#include "util/flags-enum.hpp"

constexpr u32 TileSize = 16;
constexpr s32f8 Gravity = 0.11;

class GameScene;

enum CollisionResult { None = 0, Left = 1, Right = 2, Top = 4, Bottom = 8 };
FLAGS_ENUM(CollisionResult)

class Map final
{
    MapData curMap;
    u16 prevtx, prevty;
    u16 svx, svy;

    void loadTileset(const TilesetData* tileset);
    GameScene& gameScene();

public:
    Map() : curMap() {}
    void init();
    void vblank();
    void update();

    void loadMap(const MapData& map);
    void copyFullScreen();
    void loadActors();

    auto width() const { return curMap.width; }
    auto height() const { return curMap.height; }

    // Collision routines
    CollisionResult movementSimulation(Position& pos, s32f8 width, s32f8 height) const;
    bool horizontalStripeCollision(s32f8 x, s32f8 y, s32f8 width) const;
    bool verticalStripeCollision(s32f8 x, s32f8 y, s32f8 height) const;
};
