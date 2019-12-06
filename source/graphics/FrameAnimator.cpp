//--------------------------------------------------------------------------------
// FrameAnimator.cpp
//--------------------------------------------------------------------------------
// An animator that places all the frames in the VRAM and changes the tile
// IDs to do the animation
//--------------------------------------------------------------------------------
#include "FrameAnimator.hpp"

FrameAnimatorBase::FrameAnimatorBase(const u16* frameTileIndices, const AnimationFrame* animationFrames, u16 frameTime)
    : frameTileIndices(frameTileIndices), animationFrames(animationFrames), frameTime(frameTime), frameCount(0),
      curFrame(0), repeatFrame(0), endFrame(0) {}

void FrameAnimatorBase::setAnimationPose(const AnimationPose &pose)
{
    // Set all the variables to the pose's parameters
    curFrame = pose.init;
    repeatFrame = pose.repeat;
    endFrame = pose.end;

    // And reset the state
    frameCount = 0;
}

void FrameAnimatorBase::update()
{
    // Update the frame count and then the animation frame
    frameCount++;
    if (frameCount == frameTime)
    {
        frameCount = 0;

        curFrame++;
        if (curFrame > endFrame)
            curFrame = repeatFrame;
    }
}