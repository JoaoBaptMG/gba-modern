//--------------------------------------------------------------------------------
// MsgBox.hpp
//--------------------------------------------------------------------------------
// Provides the message box that will send information to the player
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

#include "text/Tile4bppGlyphWriter.hpp"

class UserInterface;

class MsgBox final
{
    const char* curText;
    Tile4bppGlyphWriter msgWriter;
    u16 x, y, period, curWait;
    u16 openState, curColor;

    void clearMessageTiles();
    void showBox();
    void hideBox();
    void putGlyph();

    bool fullyOpen() const;
    bool fullyClosed() const { return openState == 0; }

    UserInterface& userInterface();

public:
    MsgBox();

    // Displays the string pointed to by text with the period (number of frames for each character)
    // with period - THIS ALSO CLEARS THE AREA, SO BE CAREFUL
    void displayMessage(const char* text, int period = 12);

    void close();
    void update();

    friend class UserInterface;
};
