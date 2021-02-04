//--------------------------------------------------------------------------------
// MsgBox.cpp
//--------------------------------------------------------------------------------
// Provides the message box that will send information to the player
//--------------------------------------------------------------------------------
#include "MsgBox.hpp"

#include "UserInterface.hpp"
#include "UserInterfaceDefs.hpp"
#include "data/fonts/Sweet16.hpp"
#include <cctype>

constexpr auto InitX = 2;
constexpr auto InitY = 16;
constexpr auto LineWidth = 8 * uidefs::MsgBoxTileWidth - 4 - 2 * InitX;

MsgBox::MsgBox() : curText(), msgWriter(&UI_TILE_BANK[uidefs::MsgBoxTiles], 8*uidefs::MsgBoxTileHeight), 
    x(), y(), period(8), curWait(0), openState(0)
{
    constexpr auto TileBase = uidefs::MsgBoxTileBase;

    // Set the required tiles
    for (int x = 0; x < uidefs::MsgBoxTileWidth; x++)
        for (int y = 0; y < uidefs::MsgBoxTileHeight; y++)
            UI_SCREEN[TileBase + uidefs::TilePos(x, y)] = uidefs::MsgBoxTileHeight * x + y;
}

void MsgBox::displayMessage(const char* text, int period)
{
    // Clear the area
    clearMessageTiles();

    // Set the initial X and Y;
    x = InitX, y = InitY;

    curText = text;
    this->period = period;
    curWait = 0;
    curColor = 8;
}

void MsgBox::close()
{
    curText = nullptr;
}

void MsgBox::update()
{
    // There are four main states here
    // MsgBox open and putting glyphs: curText != nullptr && fullyOpen()
    // MsgBox opening: curText != nullptr && !fullyOpen()
    // MsgBox closing: curText == nullptr && !fullyClosed()
    // MsgBox closed: curText == nullptr && fullyClosed()

    // There is text to show
    if (curText)
    {
        if (fullyOpen())
        {
            if (*curText && curWait++ < period)
            {
                putGlyph();     // Place a new glyph on the places required
                curWait = 0;
            }
        }
        else
        {
            openState++; // just open the message box
            if (fullyOpen()) showBox();
        }
    }
    else if (!fullyClosed()) // Just close the box if there's nothing to see
    {
        if (fullyOpen()) hideBox();
        openState--;
    }
}

void MsgBox::clearMessageTiles()
{
    memset32(&UI_TILE_BANK[uidefs::MsgBoxTiles], 0, uidefs::NumMsgBoxTiles * sizeof(TILE)/sizeof(u32));
}

void MsgBox::showBox()
{
    for (int x = 0; x < uidefs::MsgBoxTileWidth; x++)
        for (int y = 0; y < uidefs::MsgBoxTileHeight; y++)
            UI_SCREEN[uidefs::MsgBoxTileBase + uidefs::TilePos(x, y)] = SE_PALBANK(15) | 
                (uidefs::MsgBoxTiles + uidefs::MsgBoxTileHeight * x + y);
}

void MsgBox::hideBox()
{
    constexpr auto TileId = SE_PALBANK(15) | (uidefs::UserInterfaceTiles - 1);

    for (int y = 0; y < uidefs::MsgBoxTileHeight; y++)
        memset32(&UI_SCREEN[uidefs::MsgBoxTileBase] + uidefs::TilePos(0, y),
            TileId | (TileId << 16), uidefs::MsgBoxTileWidth * sizeof(u16)/sizeof(u32));
}

void MsgBox::putGlyph()
{
    if (*curText)
    {
        constexpr auto& font = data::fonts::Sweet16.ttf;
        const auto& spaceSp = font.glyphFor(' ').advanceX;

        // Keep this in a loop, that will be broken once the next character is put in
        for (;; curText++)
        {
            if (*curText == '\n')
            {
                x = InitX;
                y += font.verticalStride;
            }
            else if (*curText == ' ')
            {
                while (*curText == ' ')
                {
                    x += spaceSp;
                    curText++;
                }

                // A simple and crude word detection
                auto nextText = curText;
                auto testX = x;
                while (*nextText && *nextText != ' ')
                {
                    if (*nextText > ' ')
                    {
                        testX += font.glyphFor(*nextText).advanceX;
                        if (testX > InitX + LineWidth)
                        {
                            x = InitX;
                            y += font.verticalStride;
                            break;
                        }
                    }
                    nextText++;
                }

                curText--;
            }
            else if (*curText >= '\1' && *curText <= '\7')
                curColor = 8 + *curText - '\1';
            else
            {
                const auto& glyph = font.glyphFor(*curText);
                msgWriter.putGlyph(x, y, glyph, curColor);
                x += glyph.advanceX;

                if (x > InitX + LineWidth)
                {
                    x = InitX;
                    y += font.verticalStride;
                }

                curText++;
                return;
            }
        }
    }
}

bool MsgBox::fullyOpen() const { return openState == uidefs::MsgBoxMaxOpenState; }

UserInterface& MsgBox::userInterface()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<UserInterface*>(
        reinterpret_cast<std::byte*>(this) - offsetof(UserInterface, msgBox));
#pragma GCC diagnostic pop
}