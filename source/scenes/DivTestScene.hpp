//--------------------------------------------------------------------------------
// DivTestScene.hpp
//--------------------------------------------------------------------------------
// A scene to test signed division results
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include "text/TextWriter.hpp"
#include "text/Mode3GlyphWriter.hpp"

class DivTestScene final : public IScene
{
    TextWriter<Mode3GlyphWriter> writer;

public:
    DivTestScene();
    virtual void update() override {}
};