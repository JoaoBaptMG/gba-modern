//--------------------------------------------------------------------------------
// TileTextTestScene.hpp
//--------------------------------------------------------------------------------
// A test scene for text input
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include "text/TextWriter.hpp"
#include "text/Tile4bppGlyphWriter.hpp"

class TileTextTestScene final : public IScene
{
    TextWriter<Tile4bppGlyphWriter> writer;

public:
    TileTextTestScene();
};
