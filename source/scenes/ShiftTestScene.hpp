//--------------------------------------------------------------------------------
// ShiftTestScene.hpp
//--------------------------------------------------------------------------------
// <insert description here>
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include "text/TextWriter.hpp"
#include "text/Mode3GlyphWriter.hpp"

class ShiftTestScene final : public IScene
{
    TextWriter<Mode3GlyphWriter> writer;

public:
    ShiftTestScene();
    virtual void update() override {}
};
