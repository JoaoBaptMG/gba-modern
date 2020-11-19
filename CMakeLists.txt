# Projects Settings
cmake_minimum_required (VERSION 3.16)
include(ExternalProject)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$CMAKE_SOURCE_DIR}")
add_definitions("-fpermissive")
project (gbaModern)
message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}.")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compilation flags
set(ARCH "-mthumb -mthumb-interwork")
set(CFLAGS "-g -Wall -O3 -ffunction-sections -fdata-sections -mcpu=arm7tdmi -mtune=arm7tdmi -flto ${ARCH}")
set(CPPFLAGS "${CFLAGS} -std=c++17 -fno-rtti -fno-exceptions")
set(ASFLAGS "-g ${ARCH}")
set(LDFLAGS	"-g ${ARCH} -Wl,--gc-sections -Wl,-Map,gba.map")


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
if(EXISTS "${CMAKE_BINARY_DIR}/tools/tools")
	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated, probably fails if tools aren't already built?'
	foreach(SPRITE ${SPR_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools sprite-export
		${CMAKE_SOURCE_DIR}/data/sprites/${SPRITE}.png
		${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.s
		${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.hpp)
	endforeach()
	foreach(SPRITE ${SPR_FILES})
		add_custom_command(OUTPUT ${SPRITE}-command
			DEPENDS tools
			COMMAND ${CMAKE_BINARY_DIR}/tools/tools sprite-export
				${CMAKE_SOURCE_DIR}/data/sprites/${SPRITE}.png
				${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.s
				${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.hpp
				${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.hpp
			COMMENT "Exporting Sprite ${SPRITE}")
		add_custom_target(${SPRITE}-image ALL
			DEPENDS tools ${SPRITE}-command
			SOURCES ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.s
				${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.hpp
			COMMENT "Building ${SPRITE}")
		list(APPEND SPRITES_L ${SPRITE}-image)
		list(APPEND SPRITES_S ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.s)
		list(APPEND SPRITES_HPP ${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.hpp)
	endforeach()

	add_custom_target(SPRITES ALL
		DEPENDS ${SPRITES_L} ${SPRITES_S} ${SPRITES_HPP}
		SOURCES ${SPRITES_S} ${SPRITES_HPP}
		COMMENT "Building SPRITES")

	#build backgrounds
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/backgrounds)
	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated, probably fails if tools aren't already built?'
	foreach(BACKGROUND ${BKG_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools background-export
		${CMAKE_SOURCE_DIR}/data/backgrounds/${BACKGROUND}.png
		${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.s
		${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.hpp)
	endforeach()
	foreach(BACKGROUND ${BKG_FILES})
		add_custom_command(OUTPUT ${BACKGROUND}-command
			DEPENDS tools
			COMMAND ${CMAKE_BINARY_DIR}/tools/tools background-export
				${CMAKE_SOURCE_DIR}/data/backgrounds/${BACKGROUND}.png
				${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.s
				${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.hpp
			COMMENT "Exporting Sprite ${BACKGROUND}")
		add_custom_target(${BACKGROUND}-background ALL
			DEPENDS tools ${BACKGROUND}-command
			SOURCES ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.s
				${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.hpp
			COMMENT "Building ${BACKGROUND}")
		list(APPEND BACKGROUNDS_L ${BACKGROUND}-background)
		list(APPEND BACKGROUNDS_S ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.s)
		list(APPEND BACKGROUNDS_HPP ${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.hpp)
	endforeach()

	add_custom_target(BACKGROUNDS ALL
		DEPENDS ${BACKGROUNDS_L} ${BACKGROUNDS_S} ${BACKGROUNDS_HPP}
		SOURCES ${BACKGROUNDS_S} ${BACKGROUNDS_HPP}
		COMMENT "Building BACKGROUNDS")

	#build fonts
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/fonts)
	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated, probably fails if tools aren't already built?'
	foreach(FONT ${FNT_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools font-export
		${CMAKE_SOURCE_DIR}/data/fonts/${FONT}.ttf
		${CMAKE_BINARY_DIR}/data/fonts/${FONT}.s
		${CMAKE_BINARY_DIR}/data/fonts/${FONT}.hpp)
	endforeach()
	foreach(FONT ${FNT_FILES})
		add_custom_command(OUTPUT ${FONT}-command
			DEPENDS tools
			COMMAND ${CMAKE_BINARY_DIR}/tools/tools font-export
				${CMAKE_SOURCE_DIR}/data/fonts/${FONT}.ttf
				${CMAKE_BINARY_DIR}/data/fonts/${FONT}.s
				${CMAKE_BINARY_DIR}/data/fonts/${FONT}.hpp
			COMMENT "Exporting Font ${FONT}")
		add_custom_target(${FONT}-font ALL
			DEPENDS tools ${FONT}-command
			SOURCES ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.s
				${CMAKE_BINARY_DIR}/data/fonts/${FONT}.hpp
			COMMENT "Building ${FONT}")
		list(APPEND FONTS_L ${FONT}-font)
		list(APPEND FONTS_S ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.s)
		list(APPEND FONTS_HPP ${CMAKE_BINARY_DIR}/data/fonts/${FONT}.hpp)
	endforeach()

	add_custom_target(FONTS ALL
		DEPENDS ${FONTS_L} ${FONTS_S} ${FONTS_HPP}
		SOURCES ${FONTS_S} ${FONTS_HPP}
		COMMENT "Building FONTS")

	#build audio settings
	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated, probably fails if tools aren't already built?'
	foreach(SETTING ${AUS_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools audio-export-settings
		${CMAKE_SOURCE_DIR}/data/${SETTING}.json
		${CMAKE_BINARY_DIR}/data/${SETTING}.hpp
		${CMAKE_BINARY_DIR}/data/${SETTING}.json)
	endforeach()
	foreach(SETTING ${AUS_FILES})
		add_custom_command(OUTPUT ${SETTING}-command
			DEPENDS tools
			COMMAND ${CMAKE_BINARY_DIR}/tools/tools audio-export-settings
				${CMAKE_SOURCE_DIR}/data/${SETTING}.json
				${CMAKE_BINARY_DIR}/data/${SETTING}.hpp
				${CMAKE_BINARY_DIR}/data/${SETTING}.json
			COMMENT "Exporting Audio Settings ${SETTING}")
		add_custom_target(${SETTING}-aus ALL
			DEPENDS tools ${SETTING}-command
			SOURCES ${CMAKE_BINARY_DIR}/data/${SETTING}.hpp
				${CMAKE_BINARY_DIR}/data/${SETTING}.json
			COMMENT "Building ${SETTING}")
		list(APPEND AUS_L ${SETTING}-aus)
		list(APPEND AUS_HPP ${CMAKE_BINARY_DIR}/data/${SETTING}.hpp)
		list(APPEND AUS_JSON ${CMAKE_BINARY_DIR}/data/${SETTING}.json)
	endforeach()

	add_custom_target(AUS ALL
		DEPENDS ${AUS_L} ${AUS_HPP} ${AUS_JSON}
		SOURCES ${AUS_HPP} ${AUS_JSON}
		COMMENT "Building AUS (Audio Settings)")

	#build sound files
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/sounds)
	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated, probably fails if tools aren't already built?'
	foreach(SOUND ${SND_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools sound-export
		${CMAKE_SOURCE_DIR}/data/sounds/${SOUND}.wav
		${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s
		${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp
		${CMAKE_BINARY_DIR}/data/audio-settings.json)
	endforeach()
	foreach(SOUND ${SND_FILES})
		add_custom_command(OUTPUT ${SOUND}-command
			DEPENDS tools AUS
			COMMAND ${CMAKE_BINARY_DIR}/tools/tools sound-export
				${CMAKE_SOURCE_DIR}/data/sounds/${SOUND}.wav
				${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s
				${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp
				${CMAKE_BINARY_DIR}/data/audio-settings.json
			COMMENT "Exporting Sound ${SOUND}")
		add_custom_target(${SOUND}-font ALL
			DEPENDS tools ${SOUND}-command
			SOURCES ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s
				${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp
			COMMENT "Building ${SOUND}")
		list(APPEND SOUNDS_L ${SOUND}-font)
		list(APPEND SOUNDS_S ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s)
		list(APPEND SOUNDS_HPP ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp)
	endforeach()

	add_custom_target(SOUNDS ALL
		DEPENDS AUS ${SOUNDS_L} ${SOUNDS_S} ${SOUNDS_HPP}
		SOURCES ${SOUNDS_S} ${SOUNDS_HPP}
		COMMENT "Building SOUNDS")

	add_custom_target(RESOURCES ALL
		DEPENDS SPRITES
			BACKGROUNDS
			FONTS
			AUS
			SOUNDS
		SOURCES ${SPRITES_S} ${SPRITES_HPP}
			${BACKGROUNDS_S} ${BACKGROUNDS_HPP}
			${FONTS_S} ${FONTS_HPP}
			${AUS_HPP} ${AUS_JSON}
			${SOUNDS_S} ${SOUNDS_HPP})
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

	# Source files
	#build/%.iwram.o: %.iwram.cpp
	add_custom_target(IWRAM_CPP_OBJECT_FILES ALL
		DEPENDS RESOURCES)
	#	${CMAKE_CXX_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@

	#build/%.iwram.o: %.iwram.c
	add_custom_target(IWRAM_C_OBJECT_FILES ALL
		DEPENDS RESOURCES)
	#	${CMAKE_C_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@

	#build/%.o: %.cpp
	add_custom_target(CPP_OBJECTS ALL
		DEPENDS RESOURCES)
	#	${CMAKE_CXX_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

	#build/%.o: %.c
	add_custom_target(C_OBJECTS ALL
		DEPENDS RESOURCES)
	#	${CMAKE_C_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -c $< -o $@

	#build/%.o: %.s
	add_custom_target(ASSEMBLY_OBJECTS ALL
		DEPENDS RESOURCES)
	#	${CMAKE_C_COMPILER} -MMD -MP -MF $(@:.o=.d) -x assembler-with-cpp $(ASFLAGS) -c $< -o $@

	#bin/game.elf: build/data/audio-settings.hpp $(OFILES)
	# Get the right linker
	#ifeq ($(strip $(CPPFILES)),)
	#	ARMLD := $(ARMCC)
	#else
	#	ARMLD := $(ARMCPP)
	#endif
	#	@$(ARMLD) $(LDFLAGS) -specs=gba.specs $(filter-out %crt0.o, $(OFILES)) $(LIBPATHS) $(LIBRARIES) -o $@

	add_custom_target(OBJECT_DUMP ALL
		DEPENDS IWRAM_CPP_OBJECT_FILES IWRAM_C_OBJECT_FILES CPP_OBJECTS C_OBJECTS ASSEMBLY_OBJECTS)
	#	@${CMAKE_OBJDUMP} -dCS $@ > $@.dump

	#bin/game.gba:
	add_custom_target(ROM_BUILD ALL
		DEPENDS OBJECT_DUMP
		COMMENT "Building ROM")
	#	@${CMAKE_OBJCOPY} -O binary $(@:.gba=.elf) $@

	add_custom_target(ROM ALL
		DEPENDS ROM_BUILD
		COMMENT "Sanitising final file")
	#	@tools/tools rom-sanitize $@ gba.json $@

endif()