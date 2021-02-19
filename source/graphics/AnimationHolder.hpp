//--------------------------------------------------------------------------------
// AnimationHolder.hpp
//--------------------------------------------------------------------------------
// Provides a type-erased data structure that holds an animation and allows it
// to hold any kind of animation (still, stream or frame) at runtime
//--------------------------------------------------------------------------------
#pragma once

#include "StillImagePointer.hpp"
#include "StreamAnimator.hpp"
#include "FrameAnimator.hpp"
#include "data/Sprite.hpp"
#include "util/PointerInterfaceContainer.hpp"
#include <type_traits>

class IAnimationHandle
{
public:
    virtual void update() {}
    virtual u16 getTileId() { return graphics::NoTile; }

    // Sets an animation pose
    virtual void setAnimationPose(const AnimationPose& pose) {}

    virtual ~IAnimationHandle() {}
};

class StillImageHandle final : public IAnimationHandle
{
    StillImagePointer image;

public:
    StillImageHandle(StillImageAllocator& alloc) : image(alloc) {}
    virtual u16 getTileId() override { return image.getTileId(); }
    virtual ~StillImageHandle() {}
};

class StreamAnimationHandle final : public IAnimationHandle
{
    StreamAnimator animator;

public:
    // Constructor
    StreamAnimationHandle(const void* animationTiles, const AnimationFrame* animationFrames, 
        SpriteSize spriteSize, u16 frameTime) : animator(animationTiles, animationFrames, spriteSize, frameTime)
    { 
        animator.setVisible(true);
    }

    // Helper templated constructor
    template <typename Png>
    StreamAnimationHandle(const Png& png, SpriteSize spriteSize, u16 frameTime)
        : StreamAnimationHandle(png.tiles, png.animationFrames, spriteSize, frameTime) {}

    virtual void setAnimationPose(const AnimationPose& pose) override { animator.setAnimationPose(pose); }
    virtual void update() override { animator.update(); }
    virtual u16 getTileId() override { return animator.getTileId(); }
    virtual ~StreamAnimationHandle() {}
};

template <const auto& Png, SpriteSize Size>
class FrameAnimationHandle final : public IAnimationHandle
{
    FrameAnimator<Png, Size> animator;

public:
    FrameAnimationHandle(FrameStore<Png, Size>& store, u16 frameTime) : animator(store, frameTime)
    {
        animator.setVisible(true);
    }

    virtual void setAnimationPose(const AnimationPose& pose) override { animator.setAnimationPose(pose); }
    virtual void update() override { animator.update(); }
    virtual u16 getTileId() override { return animator.getTileId(); }
    virtual ~FrameAnimationHandle() {}
};

namespace detail
{
    struct __poses {};
    inline static const Sprite<32, 0, 0, 16, __poses> dummySprite{};
}

constexpr std::size_t MaxAnimationSize = std::max({ sizeof(StillImageHandle), sizeof(StreamAnimationHandle),
    sizeof(FrameAnimationHandle<detail::dummySprite, SpriteSize::s16x16_4bpp>) });
using AnimationHolder = PointerInterfaceContainer<IAnimationHandle, MaxAnimationSize>;
