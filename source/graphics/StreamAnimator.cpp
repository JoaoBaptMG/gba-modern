//--------------------------------------------------------------------------------
// StreamAnimator.cpp
//--------------------------------------------------------------------------------
// A class that animates a sprite by DMA'ing the next frame into the sprite's
// tile slot after each animation
//--------------------------------------------------------------------------------
#include "StreamAnimator.hpp"

// Initializes all the variables
StreamAnimator::StreamAnimator(const void *animationFrames, SpriteSize spriteSize, u16 frameTime)
    : animationFrames((const TILE*)animationFrames), logNumBlocks(SizeUtils::logBlocks(spriteSize)), frameTime(frameTime),
      frameCount(0), curFrame(0), repeatFrame(0), endFrame(0), tilePointer() {}

void StreamAnimator::setAnimationPose(const AnimationPose &pose)
{
    // Set all the variables to the pose's parameters
    curFrame = pose.init;
    repeatFrame = pose.repeat;
    endFrame = pose.end;

    // And reset the state
    frameCount = 0;
    sendNewFrame();
}

void StreamAnimator::clearAnimationPose()
{
    // Set all the variables to default ones
    curFrame = repeatFrame = endFrame = frameCount = 0;
    tilePointer = ObjectTilePointer();
}

void StreamAnimator::update()
{
    // Update the frame count and then the animation frame
    frameCount++;
    if (frameCount == frameTime)
    {
        frameCount = 0;

        curFrame++;
        if (curFrame > endFrame)
            curFrame = repeatFrame;
        sendNewFrame();
    }
}

void StreamAnimator::sendNewFrame()
{
    // Don't send nothing if we're not visible
    if (!isVisible()) return;

    // Send the new frame size
    auto frame = animationFrames + (curFrame << logNumBlocks);
    auto dest = &tile_mem_obj[0][getTileId()];
    graphics::romCopyCommand32(dest, frame, (sizeof(TILE)/sizeof(u32)) << logNumBlocks);
}

void StreamAnimator::setVisible(bool visible)
{
    // Check state changes
    if (!isVisible() && visible) // Allocate a new place
    {
        tilePointer = ObjectTilePointer(logNumBlocks);
        sendNewFrame();
    }
    else if (isVisible() && !visible) // Free the place
        tilePointer = ObjectTilePointer();
}
