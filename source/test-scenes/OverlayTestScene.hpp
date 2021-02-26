//--------------------------------------------------------------------------------
// OverlayTestScene.hpp
//--------------------------------------------------------------------------------
// A scene to test the use of overlays
//--------------------------------------------------------------------------------
#pragma once

#include "scenes/IScene.hpp"

#include "text/TextWriter.hpp"
#include "text/Mode3GlyphWriter.hpp"

class OverlayTestScene final : public IScene
{
    TextWriter<Mode3GlyphWriter> writer;

public:
    OverlayTestScene();
    virtual void update() override {}
};
