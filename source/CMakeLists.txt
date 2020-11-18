cmake_minimum_required (VERSION 3.16)

project (gbaModern-game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
# C Files
set(C_FILES
	math/gcd.c
	util/libc-replacement.c
	util/memmove.iwram.c
	util/random.iwram.c)
# CPP Files
set(CPP_FILES
	main.cpp
	audio/audio.cpp
	gameplay/Background.cpp
	gameplay/collisionCircleBitmaskGenerator.cpp
	gameplay/Enemy.cpp
	gameplay/EnemyProjectiles.cpp
	gameplay/Explosions.cpp
	gameplay/GameScene.cpp
	gameplay/Hud.cpp
	gameplay/Level.cpp
	gameplay/Player.cpp
	gameplay/PlayerProjectiles.cpp
	gameplay/Projectile.iwram.cpp
	gameplay/levels/level1/bigLurker.cpp
	gameplay/levels/level1/level.cpp
	gameplay/levels/level1/wavingEnemy.cpp
	gameplay/levels/level1/wobblingEnemies.cpp
	graphics/BuddyObjectAllocator.cpp
	graphics/FrameAnimator.cpp
	graphics/graphics.cpp
	graphics/HblankDma.cpp
	graphics/OamManager.cpp
	graphics/OamManager.iwram.cpp
	graphics/StreamAnimator.cpp
	math/trapdiv0.cpp
	memory/allocator.iwram.cpp
	scenes/BitmaskTestScene.cpp
	scenes/ContextTestScene.cpp
	scenes/Mode3TextTestScene.cpp
	scenes/TileTextTestScene.cpp
	util/dynamic.cpp
	util/profile.cpp
	util/virtual.cpp)
# S Files
set(S_FILES
	crt0.s
	audio/audioMix.s
	gameplay/collision.s
	gameplay/collisionBitmask.s
	gameplay/Projectile.s
	math/clz.s
	math/lmul.s
	math/sdiv32.s
	math/sqrt32.s
	math/sqrt64.s
	math/udiv32.s
	text/mode3PutGlyph.s
	text/tile4bppPutGlyph.s
	util/context.s
	util/memops.s
	util/uintDigits.s)