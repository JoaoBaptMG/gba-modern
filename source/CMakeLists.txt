cmake_minimum_required (VERSION 3.16)

project (gbaModern-game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#need to figure out precompiled headers here too? Object stack is a reference?

# Source files
# C Files
set(GAME_C_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/math/gcd.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/libc-replacement.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/memmove.iwram.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/random.iwram.c)
# CPP Files
set(GAME_CPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/audio/audio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Background.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/collisionCircleBitmaskGenerator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Enemy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/EnemyProjectiles.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Explosions.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/GameScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Hud.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Level.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Player.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/PlayerProjectiles.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Projectile.iwram.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/levels/level1/bigLurker.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/levels/level1/level.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/levels/level1/wavingEnemy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/levels/level1/wobblingEnemies.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/BuddyObjectAllocator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/FrameAnimator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/graphics.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/HblankDma.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/OamManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/OamManager.iwram.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/StreamAnimator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/trapdiv0.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/memory/allocator.iwram.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/scenes/BitmaskTestScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/scenes/ContextTestScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/scenes/Mode3TextTestScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/scenes/TileTextTestScene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/dynamic.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/profile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/virtual.cpp)
# S Files
set(GAME_S_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/crt0.s
	${CMAKE_CURRENT_SOURCE_DIR}/audio/audioMix.s
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/collision.s
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/collisionBitmask.s
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Projectile.s
	${CMAKE_CURRENT_SOURCE_DIR}/math/clz.s
	${CMAKE_CURRENT_SOURCE_DIR}/math/lmul.s
	${CMAKE_CURRENT_SOURCE_DIR}/math/sdiv32.s
	${CMAKE_CURRENT_SOURCE_DIR}/math/sqrt32.s
	${CMAKE_CURRENT_SOURCE_DIR}/math/sqrt64.s
	${CMAKE_CURRENT_SOURCE_DIR}/math/udiv32.s
	${CMAKE_CURRENT_SOURCE_DIR}/text/mode3PutGlyph.s
	${CMAKE_CURRENT_SOURCE_DIR}/text/tile4bppPutGlyph.s
	${CMAKE_CURRENT_SOURCE_DIR}/util/context.s
	${CMAKE_CURRENT_SOURCE_DIR}/util/memops.s
	${CMAKE_CURRENT_SOURCE_DIR}/util/uintDigits.s)
set(GAME_HPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/colors.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/audio/audio.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/data/AnimatedPng.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/data/BackgroundData.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/data/BitmaskData.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/data/DataPng.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/data/Font.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/data/Sound.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Background.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/collision.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/EnemyProjectiles.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Explosions.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/GameScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Level.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/PlayerProjectiles.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/Projectile.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/vecUtils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/levels/level1/bigLurker.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/levels/level1/wavingEnemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/gameplay/levels/level1/wobblingEnemies.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/AnimationPose.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/BuddyObjectAllocator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/FrameAnimator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/graphics.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/HBlankDma.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/OamManager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/ObjectTilePointer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/PalettePointer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/SpriteSize.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/StillImagePointer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/graphics/StreamAnimator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/fixed.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/fixedmath.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/gcem-degrees.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/movetowards.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/stdfixed.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/vec2.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/memory/allocator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/scenes/BitmaskTestScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/scenes/ContextTestScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/scenes/IScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/scenes/Mode3TextTestScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/scenes/TileTextTestScene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/text/aslatin1.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/text/mGBADebugging.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/text/Mode3GlyphWriter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/text/StringBuilder.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/text/TextWriter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/text/Tile4bppGlyphWriter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/AllocatorPointer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/bit-pointers.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/flags-enum.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/gba-assert.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/generateTable.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/integer-sequence-utils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/LightBitset.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/util/ObjectStack.hpp)

add_library(GAME_OBJECTS OBJECT
	${GAME_C_FILES}
	${GAME_CPP_FILES}
	${GAME_S_FILES}
	${GAME_HPP_FILES})

target_include_directories(GAME_OBJECTS PRIVATE
	${CMAKE_SOURCE_DIR}/external/tonc/tonclib/include)