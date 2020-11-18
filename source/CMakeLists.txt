cmake_minimum_required (VERSION 3.16)

project (gbaModern-game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
# C Files
set(GAME_C_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/math/gcd.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/libc-replacement.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/memmove.iwram.c
	${CMAKE_CURRENT_SOURCE_DIR}/util/random.iwram.c PARENT_SCOPE)
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
	${CMAKE_CURRENT_SOURCE_DIR}/util/virtual.cpp PARENT_SCOPE)
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
	${CMAKE_CURRENT_SOURCE_DIR}/util/uintDigits.s PARENT_SCOPE)

