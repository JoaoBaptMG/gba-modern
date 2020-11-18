//--------------------------------------------------------------------------------
// ContextTestScene.hpp
//--------------------------------------------------------------------------------
// <insert description here>
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include "../text/TextWriter.hpp"
#include "../text/Mode3GlyphWriter.hpp"

#include <tonc.h>
#include "../util/context.h"

class ContextTestScene final : public IScene
{
    TextWriter<Mode3GlyphWriter> writer;
    context_t curContext;
    u32 x;

public:
    ContextTestScene();
    virtual void update() override;

    inline void write(const char* str) { writer.write(4, 16, str, CLR_WHITE); }

    u32 framesToWait;
};
