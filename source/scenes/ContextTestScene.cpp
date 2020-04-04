//--------------------------------------------------------------------------------
// ContextTestScene.cpp
//--------------------------------------------------------------------------------
// <insert description here>
//--------------------------------------------------------------------------------
#include "ContextTestScene.hpp"

#include "data/fonts/monogram_extended.hpp"
#include "colors.hpp"
#include <cctype>

context_t testContext(context_t ctx, void* arg);

STACKPTR std::byte ctxStack[512] IWRAM_DATA;

ContextTestScene::ContextTestScene() : IScene(), writer(data::fonts::monogram_extended, vid_mem),
    curContext(context_new(ctxStack+sizeof(ctxStack), testContext, this)), x(0), framesToWait(0)
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;
}

void ContextTestScene::update()
{
    if (curContext && framesToWait-- == 0)
        curContext = context_switch(curContext);

    if (!curContext) vid_mem[SCREEN_WIDTH * 56 + x++] = colors::Green;
}

context_t testContext(context_t ctx, void* arg)
{
    static constexpr const char Str[] = "This is a test of writing a\nstring step by step using\nmy context API.";

    char construct[sizeof(Str)];
    auto& scene = *reinterpret_cast<ContextTestScene*>(arg);

    for (u32 i = 0; i < sizeof(construct)-1; i++)
    {
        construct[i] = Str[i];
        construct[i+1] = 0;

        scene.write(construct);
        scene.framesToWait = 6;

        if (construct[i] == ' ' || construct[i] == '\n') continue;
        ctx = context_switch(ctx);
    }

    scene.framesToWait = 0;
    return ctx;
}