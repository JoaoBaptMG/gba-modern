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
	${CMAKE_CURRENT_SOURCE_DIR}/fonts/monogram_extended.ttf)

set(FNT_S
	${CMAKE_CURRENT_BINARY_DIR}/fonts/monogram_extended.s)

set(FNT_HPP
	${CMAKE_CURRENT_BINARY_DIR}/fonts/monogram_extended.hpp)

# Audio Settings
set(AUS_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/audio.json)

set(AUS_HPP
	${CMAKE_CURRENT_BINARY_DIR}/audio-settings.hpp)

set(AUS_JSON
	${CMAKE_CURRENT_BINARY_DIR}/audio-settings.json)

# Sound Files
set(SND_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/sounds/player-dmg.wav
	${CMAKE_CURRENT_SOURCE_DIR}/sounds/player-heal.wav
	${CMAKE_CURRENT_SOURCE_DIR}/sounds/player-shot.wav)

set(SND_S
	${CMAKE_CURRENT_BINARY_DIR}/sounds/player-dmg.s
	${CMAKE_CURRENT_BINARY_DIR}/sounds/player-heal.s
	${CMAKE_CURRENT_BINARY_DIR}/sounds/player-shot.s)

set(SND_HPP
	${CMAKE_CURRENT_BINARY_DIR}/sounds/player-dmg.hpp
	${CMAKE_CURRENT_BINARY_DIR}/sounds/player-heal.hpp
	${CMAKE_CURRENT_BINARY_DIR}/sounds/player-shot.hpp)
	
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

#build fonts
list(APPEND FNT_FILES_L ${FNT_FILES})
list(APPEND FNT_S_L ${FNT_S})
list(APPEND FNT_HPP_L ${FNT_HPP})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fonts)
foreach(x y z IN ZIP_LISTS FNT_FILES_L FNT_S_L FNT_HPP_L)
	execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools font-export ${x} ${y} ${z})
endforeach()

#build audio settings
list(APPEND AUS_FILES_L ${AUS_FILES})
list(APPEND AUS_HPP_L ${AUS_HPP})
list(APPEND AUS_JSON_L ${AUS_JSON})

foreach(x y z IN ZIP_LISTS AUS_FILES AUS_HPP AUS_JSON)
	execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools audio-export-settings ${x} ${y} ${z})
endforeach()

#build sound files
list(APPEND SND_FILES_L ${SND_FILES})
list(APPEND SND_S_L ${SND_S})
list(APPEND SND_HPP_L ${SND_HPP})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sounds)
foreach(x y z IN ZIP_LISTS SND_FILES_L SND_S_L SND_HPP_L)
	execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools sound-export ${x} ${y} ${z} ${CMAKE_BINARY_DIR}/data/audio-settings.json)
endforeach()

add_library(RESOURCES OBJECT
	${SPR_S}
	${SPR_HPP}
	${BKG_S}
	${BKG_HPP}
	${FNT_S}
	${FNT_HPP}
	${AUS_HPP}
	${AUS_JSON}
	${SND_S}
	${SND_HPP})