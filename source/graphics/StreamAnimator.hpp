//--------------------------------------------------------------------------------
// StreamAnimator.hpp
//--------------------------------------------------------------------------------
// A class that animates a sprite by DMA'ing the next frame into the sprite's
// tile slot after each animation
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "graphics/SpriteSize.hpp"
#include "graphics/AnimationPose.hpp"
#include "graphics/ObjectTilePointer.hpp"

// The StreamAnimator starts as invisible
class StreamAnimator final
{
    // The pointer to the frame data
    const TILE* animationFrames;
    // The size of the sprite requested, used in the calculations
    u16 logNumBlocks:15;
    // Whether it should clear or not the animation at the end
    u16 clearAnimation:1;
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
    StreamAnimator(const void* animationFrames, SpriteSize spriteSize, u16 frameTime);
    ~StreamAnimator() {}

    // Sets an animation pose
    void setAnimationPose(const AnimationPose& pose, bool clearAnim = false);

    // Clears all animation poses
    void clearAnimationPose();

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
