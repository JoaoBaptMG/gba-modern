# Projects Settings
cmake_minimum_required (VERSION 3.16)
include(ExternalProject)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$CMAKE_SOURCE_DIR}")
add_definitions("-fpermissive")
project (gbaModern)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compilation flags
#set(ARCH "-mthumb -mthumb-interwork")
#set(CFLAGS "-g -Wall -O3 -ffunction-sections -fdata-sections -mcpu=arm7tdmi -mtune=arm7tdmi -flto ${ARCH}")
#set(CPPFLAGS "${CFLAGS} -std=c++17 -fno-rtti -fno-exceptions")
#set(ASFLAGS "-g ${ARCH}")
#set(LDFLAGS	"-g ${ARCH} -Wl,--gc-sections -Wl,-Map,gba.map")


# Libraries
#set(TONC_PATH "${CMAKE_SOURCE_DIR}/external/tonc/tonclib")
#set(LIBRARIES_ "-nodefaultlibs -ltonc")
#set(LIBDIRS ${TONC_PATH})
#set(INCDIRS "${CMAKE_SOURCE_DIR}/external/gcem/include")

# External Project is used to generate tools, 
# so we can use the normal gcc and g++ compilers,
# because we use ARM compilers to build the ROM
ExternalProject_Add(tools
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools
	BINARY_DIR ${CMAKE_BINARY_DIR}/tools
	INSTALL_COMMAND cmake -E echo "Skipping install step.")

# Resource File Definitions
# Sprite Files - Assumed to be .png
set(SPR_FILES
	big-lurker
	bitmask-test
	enemy-projectiles
	hud
	player
	player-projectiles
	small-explosion
	waving-enemy
	wobbling-enemy)

# Background Files - Assumed to be .png
set(BKG_FILES
	level1)

# Font Files - Assumed to be .ttf
set(FNT_FILES
	monogram_extended)

# Audio Settings - Assumed to be .json
set(AUS_FILES
	audio)

# Sound Files - Assumed to be .wav
set(SND_FILES
	player-dmg
	player-heal
	player-shot)

#build sprites
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/sprites)
foreach(SPRITE ${SPR_FILES})
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.s ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.hpp
		DEPENDS tools
		COMMAND ${CMAKE_BINARY_DIR}/tools/tools sprite-export ${CMAKE_SOURCE_DIR}/data/sprites/${SPRITE}.png ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.s ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.hpp
		BYPRODUCTS ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.s ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.hpp
		COMMENT "Exporting Sprite ${SPRITE}")
	add_custom_target(${SPRITE} ALL
		DEPENDS tools ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.s ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.hpp
		COMMENT "Building ${SPRITE}")
	list(APPEND SPRITES_L ${SPRITE})
endforeach()

add_custom_target(SPRITES ALL
	DEPENDS ${SPRITES_L})

#build backgrounds
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/backgrounds)
foreach(BACKGROUND ${BKG_FILES})
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.s ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.hpp
		DEPENDS SPRITES
		COMMAND ${CMAKE_BINARY_DIR}/tools/tools background-export ${CMAKE_SOURCE_DIR}/data/backgrounds/${BACKGROUND}.png ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.s ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.hpp
		BYPRODUCTS ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.s ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.hpp
		COMMENT "Exporting Background Sprite ${SPRITE}")
	add_custom_target(${BACKGROUND} ALL
		DEPENDS tools ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.s ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.hpp
		COMMENT "Building ${BACKGROUND}")
	list(APPEND BACKGROUNDS_L ${BACKGROUND})
endforeach()

add_custom_target(BACKGROUNDS ALL
	DEPENDS ${BACKGROUNDS_L})

#build fonts
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/fonts)
foreach(FONT ${FNT_FILES})
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.s ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.hpp
		DEPENDS BACKGROUNDS
		COMMAND ${CMAKE_BINARY_DIR}/tools/tools font-export ${CMAKE_SOURCE_DIR}/data/fonts/${FONT}.ttf ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.s ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.hpp
		BYPRODUCTS ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.s ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.hpp
		COMMENT "Exporting Font ${FONT}")
	add_custom_target(${FONT} ALL
		DEPENDS tools ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.s ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.hpp
		COMMENT "Building ${FONT}")
	list(APPEND FONTS_L ${FONT})
endforeach()

add_custom_target(FONTS ALL
	DEPENDS ${FONTS_L})

#build audio settings
foreach(SETTING ${AUS_FILES})
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/data/${SETTING}.hpp ${CMAKE_BINARY_DIR}/data/${SETTING}.json
		DEPENDS FONTS
		COMMAND ${CMAKE_BINARY_DIR}/tools/tools audio-export-settings ${CMAKE_SOURCE_DIR}/data/${SETTING}.json ${CMAKE_BINARY_DIR}/data/${SETTING}.hpp ${CMAKE_BINARY_DIR}/data/${SETTING}.json
		BYPRODUCTS ${CMAKE_BINARY_DIR}/data/${SETTING}.hpp ${CMAKE_BINARY_DIR}/data/${SETTING}.json
		COMMENT "Exporting Audio Setting ${SETTING}")
	add_custom_target(${SETTING} ALL
		DEPENDS tools ${CMAKE_BINARY_DIR}/data/${SETTING}.hpp ${CMAKE_BINARY_DIR}/data/${SETTING}.json
		COMMENT "Building ${SETTING}")
	list(APPEND AUS_L ${SETTING})
endforeach()

add_custom_target(AUS ALL
	DEPENDS ${AUS_L})

#build sound files
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/sounds)
foreach(SOUND ${SND_FILES})
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp
		DEPENDS AUS
		COMMAND ${CMAKE_BINARY_DIR}/tools/tools sound-export ${CMAKE_SOURCE_DIR}/data/sounds/${SOUND}.wav ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp ${CMAKE_BINARY_DIR}/data/audio-settings.json
		BYPRODUCTS ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp
		COMMENT "Exporting Sound ${SOUND}")
	add_custom_target(${SOUND} ALL
		DEPENDS tools ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp
		COMMENT "Building ${SOUND}")
	list(APPEND SND_L ${SOUND})
endforeach()

add_custom_target(SOUNDS ALL
	DEPENDS ${SND_L})

#add_library(RESOURCES OBJECT
#	${SPR_S})
#	${BKG_S}
#	${BKG_HPP}
#	${FNT_S}
#	${FNT_HPP}
#	${AUS_HPP}
#	${AUS_JSON}
#	${SND_S}
#	${SND_HPP})
#add_subdirectory(source)


# Helper variables
#add_library(OBJECT_FILES OBJECT
#	$<TARGET_OBJECTS:GAME_OBJECTS>
#	${exported}
#	SPRITES)
#	${SPRITES_S}
#	${SPRITES_HPP})
#	$<TARGET_OBJECTS:SPRITES>)
#DFILES := $(OFILES:.o=.d)
#LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
#INCLUDE	:= $(foreach dir,$(LIBDIRS),-isystem $(dir)/include) $(foreach dir,$(INCDIRS), -isystem $(dir)) -iquote build -iquote source

#find_package(LibSampleRate REQUIRED)

# Get the right linker
#ifeq ($(strip $(CPPFILES)),)
#	ARMLD := $(ARMCC)
#else
#	ARMLD := $(ARMCPP)
#endif

#bin/game.elf: build/data/audio-settings.hpp $(OFILES)
#	@$(ARMLD) $(LDFLAGS) -specs=gba.specs $(filter-out %crt0.o, $(OFILES)) $(LIBPATHS) $(LIBRARIES) -o $@
#	@${CMAKE_OBJDUMP} -dCS $@ > $@.dump

# Source files
#build/%.iwram.o: %.iwram.cpp
#	@mkdir -p $(@D)
#	${CMAKE_CXX_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@
add_custom_target(IWRAM_CPP_OBJECT_FILES ALL
	DEPENDS tools SPRITES BACKGROUNDS FONTS SOUNDS)

#build/%.iwram.o: %.iwram.c
#	@mkdir -p $(@D)
#	${CMAKE_C_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@
add_custom_target(IWRAM_C_OBJECT_FILES ALL
	DEPENDS tools SPRITES BACKGROUNDS FONTS SOUNDS)

#build/%.o: %.cpp
#	@mkdir -p $(@D)
#	${CMAKE_CXX_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -c $< -o $@
add_custom_target(CPP_OBJECTS ALL
	DEPENDS tools SPRITES BACKGROUNDS FONTS SOUNDS)

#build/%.o: %.c
#	@mkdir -p $(@D)
#	${CMAKE_C_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -c $< -o $@
add_custom_target(C_OBJECTS ALL
	DEPENDS tools SPRITES BACKGROUNDS FONTS SOUNDS)

#build/%.o: %.s
#	@mkdir -p $(@D)
#	${CMAKE_C_COMPILER} -MMD -MP -MF $(@:.o=.d) -x assembler-with-cpp $(ASFLAGS) -c $< -o $@
add_custom_target(ASSEMBLY_OBJECTS ALL
	DEPENDS tools SPRITES BACKGROUNDS FONTS SOUNDS
	)#COMMAND ${CMAKE_C_COMPILER} -MMD -MP -MF)

#bin/game.gba:
#	@mkdir -p bin
#	@${CMAKE_OBJCOPY} -O binary $(@:.gba=.elf) $@
#	@tools/tools rom-sanitize $@ gba.json $@
add_custom_target(ROM_BUILD ALL
	DEPENDS IWRAM_CPP_OBJECT_FILES IWRAM_C_OBJECT_FILES CPP_OBJECTS C_OBJECTS ASSEMBLY_OBJECTS
	COMMENT "Building ROM")

add_custom_target(ROM ALL
	DEPENDS ROM_BUILD
	COMMENT "Sanitising final file")