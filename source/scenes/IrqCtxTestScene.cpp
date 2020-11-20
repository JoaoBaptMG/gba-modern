//--------------------------------------------------------------------------------
// IrqCtxTestScene.cpp
//--------------------------------------------------------------------------------
// <insert description here>
//--------------------------------------------------------------------------------
#include "IrqCtxTestScene.hpp"

#include <tonc.h>
#include "util/random.h"
#include "colors.hpp"

static STACKPTR std::byte ctxStack[512] IWRAM_DATA;
static context_t oldContext;
static context_t testContext(context_t ctx, void* arg);
static void testIrq();

IrqCtxTestScene::IrqCtxTestScene() : IScene(), curContext(context_new(ctxStack+sizeof(ctxStack), testContext, this))
{
    REG_DISPCNT = DCNT_MODE0;
    REG_DISPSTAT |= DSTAT_VCT_IRQ | DSTAT_VCT(80);
    irq_add(II_VCOUNT, testIrq);

    oldContext = context_t();
}

void IrqCtxTestScene::update()
{
    curContext = context_switch2(curContext, &oldContext);
}

static const u16 ColorList[] = 
{
    colors::DarkBlue, colors::Cyan, colors::Red, colors::Yellow, colors::Green,
    colors::Azure, colors::Beige, colors::Blue, colors::Violet, colors::Brown,
    colors::Magenta, colors::Indigo, colors::IndianRed, colors::Lavender, colors::White,
    colors::Black
};

constexpr auto ColorListSize = sizeof(ColorList) / sizeof(ColorList[0]);

context_t testContext(context_t ctx, void* arg)
{
    int i = 0;
    for (;;)
    {
        pal_bg_mem[0] = ColorList[i];
        i = (i+1) & 15;
    }
}

void testIrq()
{
    context_switch_irq(oldContext);
}
