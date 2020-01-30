//--------------------------------------------------------------------------------
// Player.cpp
//--------------------------------------------------------------------------------
// The class for the player
//--------------------------------------------------------------------------------
#include "Player.hpp"

#include "data/sprites/player.hpp"
#include "data/sprites/player-melee.hpp"
#include "graphics.hpp"
#include "GameScene.hpp"
#include "math/movetowards.hpp"
#include <algorithm>

// Don't worry, this is safe
constexpr s32f8 JumpSpeed = -3.5;
constexpr auto DecaySpeed = JumpSpeed / 2;

constexpr int PlayerPriority = 4;

static SinglePaletteAllocator palette(data::sprites::player.png.palette);

Player::Player() : meleeAnimator(data::sprites::player_melee.png, SpriteSize::s16x32_4bpp, 2) {}

void Player::init(s32f8 x, s32f8 y)
{
    playerPtr = ObjectTilePointer(SpriteSize::s16x32_4bpp);

    // Copy the player's data to the main palette
    playerPtr.setData(data::sprites::player.png.tiles);

    // Allocate to the palette
    palPtr = SinglePalettePointer(palette);

    // Initialize the position
    pos = vec2(x, y);

    // Initialize the health
    health = maxHealth = 8;
    invCounter = 0;
    money = 0;

    inAir = false;
}

void Player::update()
{
    vel.y += Gravity;
    vel.x = std::clamp(vel.x, s32f8(-16), s32f8(16));
    vel.y = std::clamp(vel.y, s32f8(-16), s32f8(16));

    listenToCommands();
    respondToGravityAndCollision();
    observeMelee();

    if (invCounter > 0) invCounter--;
}

void Player::listenToCommands()
{
    if (inAir)
    {
        if (key_released(KEY_A) && vel.y < DecaySpeed)
            vel.y = DecaySpeed;
    }
    else
    {
        if (key_hit(KEY_A))
        {
            inAir = true;
            vel.y = JumpSpeed;
        }
    }
    
    // Move according to the directional keys
    moveTowards(vel.x, s32f8(key_tri_horz()), s32f8(1.5 / 16));

    // Activate the melee attack
    if (key_hit(KEY_B)) triggerMelee();

    // For the melee attack
    if (key_hit(KEY_RIGHT)) goingLeft = false;
    else if (key_hit(KEY_LEFT)) goingLeft = true;
}

void Player::respondToGravityAndCollision()
{
    auto res = gameScene().map.movementSimulation(pos, vel, PlayerWidth, PlayerHeight);
    pos += vel + res;
    if (res.x != 0) vel.x = 0;
    if (res.y < 0)
    {
        vel.y = 0;
        inAir = false;
    }
    else if (res.y == 0) inAir = true;
    if (inAir && res.y > 0) vel.y = 0;
}

void Player::pushGraphics()
{
    auto dp = vec2<int>(pos) - gameScene().camera;

    // Push the sprite, but only if it's not offscreen or the invis counter is acting
    if (dp.x > -16 && dp.x < 240 && dp.y > -32 && dp.y < 160)
    {
        auto flip = goingLeft ? SpriteFlip::Horizontal : SpriteFlip::None;

        if (!(invCounter & 2))
            graphics::oam.pushRegular(dp, SpriteSize::s16x32_4bpp, playerPtr.getTileId(),
                palPtr.getPalette(), 0, flip, PlayerPriority);
        if (meleeAnimator.isVisible())
        {
            auto dps = goingLeft ? vec2(-16, 0) : vec2(PlayerWidth, 0);
            graphics::oam.pushRegular(dp + dps, SpriteSize::s16x32_4bpp, meleeAnimator.getTileId(),
                palPtr.getPalette(), 0, flip, PlayerPriority);
        }
    }
}

void Player::heal(int amount)
{
    if (amount == 0) return;
    if (amount < 0) damage(-amount);
    health += amount;
    if (health > maxHealth)
        health = maxHealth;
}

void Player::damage(int amount)
{
    if (amount == 0) return;
    if (amount < 0) heal(-amount);

    if (invCounter > 0) return;
    if (health > amount)
    {
        health -= amount;
        invCounter = 120;
    }
    else
    {
        // TODO: die
        for (;;); // Just hang up
    }
}

void Player::giveMoney(int amount)
{
    ASSERT(amount > 0);
    money += amount;
    gameScene().hud.notifyMoneyChange(money);
}

void Player::triggerMelee()
{
    // Triggers the melee attack (use the animation counter for it)
    if (!meleeAnimator.isVisible())
    {
        using namespace data::sprites;
        meleeAnimator.setAnimationPose(player_melee.png.poses.Default, true);
        meleeAnimator.setVisible(true);
    }
}

void Player::observeMelee()
{
    // Triggers the melee attack in the right frame
    meleeAnimator.update();
    if (meleeAnimator.isVisible())
    {
        auto& scene = gameScene();
        for (auto& actor : scene.getActors())
            actor.respondToMelee(scene);
    }
}

GameScene& Player::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, player));
#pragma GCC diagnostic pop
}
