//--------------------------------------------------------------------------------
// Mode3TextTestScene.hpp
//--------------------------------------------------------------------------------
// A test scene for text input
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include "text/TextWriter.hpp"
#include "text/Mode3GlyphWriter.hpp"

class Mode3TextTestScene final : public IScene
{
    TextWriter<Mode3GlyphWriter> writer;

public:
    Mode3TextTestScene();
};
