//--------------------------------------------------------------------------------
// IrqCtxTestScene.hpp
//--------------------------------------------------------------------------------
// <insert description here>
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include "util/context.h"

class IrqCtxTestScene final : public IScene
{
    context_t curContext;

public:
    IrqCtxTestScene();
    virtual void update() override;
};