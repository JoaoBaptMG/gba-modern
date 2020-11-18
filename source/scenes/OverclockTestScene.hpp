//--------------------------------------------------------------------------------
// OverclockTestScene.hpp
//--------------------------------------------------------------------------------
// A test scene for testing the overclock register on different modes
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include "text/TextWriter.hpp"
#include "text/Mode3GlyphWriter.hpp"

class OverclockTestScene final : public IScene
{
    TextWriter<Mode3GlyphWriter> writer;

public:
    OverclockTestScene();
};
