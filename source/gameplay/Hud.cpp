//--------------------------------------------------------------------------------
// Hud.cpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter and the boss meter
//--------------------------------------------------------------------------------
#include "Hud.hpp"

#include "graphics.hpp"
#include "GameScene.hpp"
#include "Player.hpp"
#include "data/sprites/hud.hpp"
#include "data/sprites/numbers.hpp"
#include "util/uintDigits.h"

using namespace data::sprites;

constexpr int MoneyFade = 16;
constexpr int MoneyDuration = 300;

constexpr int UIPriority = 2;

static SinglePaletteAllocator palette(hud.png.palette);

void Hud::init()
{
    // Allocate the tiles for icon and the bits
    iconPtr = ObjectTilePointer(SpriteSize::s16x16_4bpp);
    bitsPtr = ObjectTilePointer(SpriteSize::s16x16_4bpp);
    numberPtrs[0] = ObjectTilePointer(SizeUtils::logBlocks(SpriteSize::s8x8_4bpp) + 3);
    numberPtrs[1] = ObjectTilePointer(SizeUtils::logBlocks(SpriteSize::s8x8_4bpp) + 1);

    // Load the graphics
    iconPtr.setData(hud.png.tiles, sizeof(hud.png.tiles)/2);
    bitsPtr.setData(hud.png.tiles + sizeof(hud.png.tiles)/2, sizeof(hud.png.tiles)/2);
    numberPtrs[0].setData(numbers.png.tiles, sizeof(TILE)*8);
    numberPtrs[1].setData(numbers.png.tiles + sizeof(TILE)*8, sizeof(TILE)*2);

    palPtr = SinglePalettePointer(palette);

    money = 0;
    moneyDisplayCounter = 0;
}

void Hud::pushGraphics()
{
    // Push the icon
    graphics::oam.pushRegular(vec2(8, 8), SpriteSize::s16x16_4bpp, iconPtr.getTileId(), palPtr.getPalette(), 0, UIPriority);

    const Player& player = gameScene().player;
    int health = player.getHealth(), maxHealth = player.getMaxHealth();

    vec2 cur(24, 12);
    // Push the health bits
    for (int i = 0; i < maxHealth; i++)
    {
        u16 tileId = bitsPtr.getTileId();
        if (i >= health) tileId++;
        graphics::oam.pushRegular(cur, SpriteSize::s8x8_4bpp, tileId, palPtr.getPalette(), 0, UIPriority);
        cur.x += 8;
    }

    // Push the tip
    graphics::oam.pushRegular(cur, SpriteSize::s8x8_4bpp, bitsPtr.getTileId()+2, palPtr.getPalette(), 0, UIPriority);

    // Push the numbers
    if (moneyDisplayCounter > 0 && ++moneyDisplayCounter == MoneyDuration) moneyDisplayCounter = 0;
    auto visible = moneyDisplayCounter != 0;
    if (visible) displayMoney();
}

void Hud::notifyMoneyChange(u16 newMoney)
{
    if (money != newMoney)
    {
        money = newMoney;
        createNewDigits();
    }

    moneyDisplayCounter = std::clamp((int)moneyDisplayCounter, 1, MoneyFade);
}

void Hud::createNewDigits()
{
    auto end = uintDigits(moneyDigits, money);
    while (end < moneyDigits + sizeof(moneyDigits))
        *end++ = -1;
}

void Hud::displayMoney()
{
    vec2 cur(227, 8);
    if (moneyDisplayCounter < MoneyFade)
        cur.y = 16 * moneyDisplayCounter / MoneyFade - 8;
    else if (moneyDisplayCounter > MoneyDuration - MoneyFade)
        cur.y = 16 * (MoneyDuration - moneyDisplayCounter) / MoneyFade - 8;

    for (int i = 0; i < 4; i++)
    {
        int digit = moneyDigits[i];
        if (digit == -1) break;

        int tileId;
        if (digit < 8)
            tileId = numberPtrs[0].getTileId() + digit;
        else tileId = numberPtrs[1].getTileId() + digit - 8;

        graphics::oam.pushRegular(cur, SpriteSize::s8x8_4bpp, tileId, palPtr.getPalette(), 0, UIPriority);
        cur.x -= 6;
    }
}

GameScene& Hud::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, hud));
#pragma GCC diagnostic pop
}
