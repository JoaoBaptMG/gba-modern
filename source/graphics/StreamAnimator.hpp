//--------------------------------------------------------------------------------
// StreamAnimator.hpp
//--------------------------------------------------------------------------------
// A class that animates a sprite by DMA'ing the next frame into the sprite's
// tile slot after each animation
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "graphics/SpriteSize.hpp"
#include "data/AnimationFrame.hpp"
#include "graphics/AnimationPose.hpp"
#include "graphics/ObjectTilePointer.hpp"

// The StreamAnimator starts as invisible
class StreamAnimator final
{
    // The pointer to the tile data
    const TILE* animationTiles;
    // The pointer to the frame data
    const AnimationFrame* animationFrames;
    // The size of the sprite requested, used in the calculations
    u16 logNumBlocks;
    // The number of GBA frames to wait between each animation frame
    u16 frameTime;
    // The GBA count, and important properties of the animation
    u16 frameCount, curFrame, repeatFrame, endFrame;
    // The actual tile pointer
    ObjectTilePointer tilePointer;

    // Schedule a new tile to be placed
    void sendNewFrame(); 

public:
    // Constructor and destructor
    StreamAnimator(const void* animationTiles, const AnimationFrame* animationFrames, SpriteSize spriteSize, u16 frameTime);
    ~StreamAnimator() {}

    // Helper templated constructor
    template <typename Png>
    StreamAnimator(const Png& png, SpriteSize spriteSize, u16 frameTime)
        : StreamAnimator(png.tiles, png.animationFrames, spriteSize, frameTime) {}

    // Sets an animation pose
    void setAnimationPose(const AnimationPose& pose);

    // Update the animation
    void update();

    // Gets the animation tile ID for that day
    u16 getTileId() const { return tilePointer.getTileId(); }

    // Set visible and is visible
    bool isVisible() const { return getTileId() != graphics::NoTile; }
    void setVisible(bool visible);

    // Getters
    auto getFrameTime() const { return frameTime; }
    auto getFrameCount() const { return frameCount; }
    auto getCurFrame() const { return curFrame; }
    auto getRepeatFrame() const { return repeatFrame; }
    auto getEndFrame() const { return endFrame; }
};
