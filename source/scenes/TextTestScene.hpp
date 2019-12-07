//--------------------------------------------------------------------------------
// TextTestScene.hpp
//--------------------------------------------------------------------------------
// A test scene for text input
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include "text/TextWriter.hpp"

class TextTestScene final : public IScene
{
    TextWriter writer;

public:
    TextTestScene();
};
