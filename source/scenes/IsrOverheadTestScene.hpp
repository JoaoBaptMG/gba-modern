//--------------------------------------------------------------------------------
// IsrOverheadTestScene.hpp
//--------------------------------------------------------------------------------
// Measures the overhead of the current interrupt service routine
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include "text/TextWriter.hpp"
#include "text/Mode3GlyphWriter.hpp"

class IsrOverheadTestScene final : public IScene
{
    TextWriter<Mode3GlyphWriter> writer;

public:
    IsrOverheadTestScene();
    virtual void update() override;
};
