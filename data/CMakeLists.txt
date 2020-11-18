# Resources
# Sprite Files
set(SPR_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/sprites/big-lurker.png
	${CMAKE_CURRENT_SOURCE_DIR}/sprites/bitmask-test.png
	${CMAKE_CURRENT_SOURCE_DIR}/sprites/enemy-projectiles.png
	${CMAKE_CURRENT_SOURCE_DIR}/sprites/hud.png
	${CMAKE_CURRENT_SOURCE_DIR}/sprites/player.png
	${CMAKE_CURRENT_SOURCE_DIR}/sprites/player-projectiles.png
	${CMAKE_CURRENT_SOURCE_DIR}/sprites/small-explosion.png
	${CMAKE_CURRENT_SOURCE_DIR}/sprites/waving-enemy.png
	${CMAKE_CURRENT_SOURCE_DIR}/sprites/wobbling-enemy.png)
set(SPR_S
	${CMAKE_CURRENT_BINARY_DIR}/sprites/big-lurker.s
	${CMAKE_CURRENT_BINARY_DIR}/sprites/bitmask-test.s
	${CMAKE_CURRENT_BINARY_DIR}/sprites/enemy-projectiles.s
	${CMAKE_CURRENT_BINARY_DIR}/sprites/hud.s
	${CMAKE_CURRENT_BINARY_DIR}/sprites/player.s
	${CMAKE_CURRENT_BINARY_DIR}/sprites/player-projectiles.s
	${CMAKE_CURRENT_BINARY_DIR}/sprites/small-explosion.s
	${CMAKE_CURRENT_BINARY_DIR}/sprites/waving-enemy.s
	${CMAKE_CURRENT_BINARY_DIR}/sprites/wobbling-enemy.s)
set(SPR_HPP
	${CMAKE_CURRENT_BINARY_DIR}/sprites/big-lurker.hpp
	${CMAKE_CURRENT_BINARY_DIR}/sprites/bitmask-test.hpp
	${CMAKE_CURRENT_BINARY_DIR}/sprites/enemy-projectiles.hpp
	${CMAKE_CURRENT_BINARY_DIR}/sprites/hud.hpp
	${CMAKE_CURRENT_BINARY_DIR}/sprites/player.hpp
	${CMAKE_CURRENT_BINARY_DIR}/sprites/player-projectiles.hpp
	${CMAKE_CURRENT_BINARY_DIR}/sprites/small-explosion.hpp
	${CMAKE_CURRENT_BINARY_DIR}/sprites/waving-enemy.hpp
	${CMAKE_CURRENT_BINARY_DIR}/sprites/wobbling-enemy.hpp)
# Background Files
set(BKG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/backgrounds/level1.png)
set(BKG_S
	${CMAKE_CURRENT_BINARY_DIR}/backgrounds/level1.s)
set(BKG_HPP
	${CMAKE_CURRENT_BINARY_DIR}/backgrounds/level1.hpp)
# Font Files
set(FNT_FILES
	fonts/monogram_extended.ttf)
# Audio Settings
set(AUS_FILES
	audio.json)
# Sound Files
set(SND_FILES
	sounds/player-dmg.wav
	sounds/player-heal.wav
	sounds/player-shot.wav)
	
#build sprites
list(APPEND SPR_FILES_L ${SPR_FILES})
list(APPEND SPR_S_L ${SPR_S})
list(APPEND SPR_HPP_L ${SPR_HPP})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sprites)
foreach(x y z IN ZIP_LISTS SPR_FILES_L SPR_S_L SPR_HPP_L)
	execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools sprite-export ${x} ${y} ${z})
endforeach()

#build backgrounds
list(APPEND BKG_FILES_L ${BKG_FILES})
list(APPEND BKG_S_L ${BKG_S})
list(APPEND BKG_HPP_L ${BKG_HPP})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/backgrounds)
foreach(x y z IN ZIP_LISTS BKG_FILES_L BKG_S_L BKG_HPP_L)
	execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools background-export ${x} ${y} ${z})
endforeach()

add_library(resources OBJECT
	${SPR_S}
	${SPR_HPP}
	${BKG_S}
	${BKG_HPP})