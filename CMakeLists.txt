# Projects Settings
cmake_minimum_required (VERSION 3.16)

include(ExternalProject)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$CMAKE_SOURCE_DIR}")

project (gba-modern)

message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}.") #this is needed to actually load the toolchain file from the config for some reason lol
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compilation flags	
# Architecture? flags
# -mthumb Requests that the compiler targets the T32 instruction set.
# -mthumb-interwork Generate code that supports calling between the ARM and Thumb instruction sets	
set(ARCH -mthumb -mthumb-interwork)

# C flags
# -g requests that the compiler and linker generate and retain source-level dubgging/symbol infomration
# -Wall enables all compiler's warning messages.
# -O3 optimisation level 3,, enables various optimisation flags
# -ffunction-sections generates a separate ELF section for each function in the source file.
#	The unused section elimination feature of the linker can then remove unused functions at link time.
#	The output section for each function has the same name as the function that generates the section, but with a . text.
# -fdata-sections Place each function or data item into its own section in the output file if the target supports arbitrary sections.
#	The name of the function or the name of the data item determines the section's name in the output file.
# -mcpu=arm7tdmi This specifies the name of the target ARM processor. GCC uses this name to derive the name of the target ARM architecture (as if specified by -march)
#	and the ARM processor type for which to tune for performance (as if specified by -mtune). Where this option is used in conjunction with -march or -mtune, those options
#	take precedence over the appropriate part of this option.
# -mtune=arm7tdmi This option specifies the name of the target ARM processor for which GCC should tune the performance of the code. For some ARM implementations better
#	performance can be obtained by using this option. 
# -flto Enables or disables link time optimization. -flto outputs bitcode wrapped in an ELF file for link time optimization.
#	The primary use for files containing bitcode is for link time optimization. 
set(CFLAGS -g -Wall -O3 -ffunction-sections -fdata-sections -mcpu=arm7tdmi -mtune=arm7tdmi -flto ${ARCH})

# CPP flags
# -std=c++17 Set the c++ standard to 17
# -fno-rtti Disable generation of information about every class with virtual functions for use by the C++ runtime type identification features (`dynamic_cast' and `typeid').
#	If you don't use those parts of the language, you can save some space by using this flag. Note that exception handling uses the same information, but it will generate it as needed.
#	The `dynamic_cast' operator can still be used for casts that do not require runtime type information, i.e. casts to void * or to unambiguous base classes.
# -fno-exceptions Compiling with -fno-exceptions disables exceptions support and uses the variant of C++ libraries without exceptions. Use of try, catch, or throw results in an error message.
#	Linking objects that have been compiled with -fno-exceptions automatically selects the libraries without exceptions.
#	You can use the linker option --no_exceptions to diagnose whether the objects being linked contain exceptions.
set(CPPFLAGS ${CFLAGS} -std=c++17 -fno-rtti -fno-exceptions)

# assembler flags
# -g requests that the compiler and linker generate and retain source-level dubgging/symbol infomration
set(ASFLAGS -g ${ARCH}) #-g seems to break this?

# LD flags
# -g requests that the compiler and linker generate and retain source-level dubgging/symbol infomration
# -Wl,--gc-section Once the objects and static libraries are created with these options, the linker can perform the dead code elimination.
#	You can do this by setting the -Wl,--gc-sections option to gcc command (Which we do)
#	This will perform a garbage collection of code and data never referenced.
# -Wl,--Map,gba.map Generate a Linker Memory Map?
set(LDFLAGS	-g ${ARCH} -Wl,--gc-sections -Wl,-Map,gba.map)

# Dependencies
#	gcem is handled as a submodule so it is fine
#	GBA gcc is needed
set(GCC_PLATFORM "x86_64-linux") #just assuming this for now, toolchain stuff messes things up a little
set(GCC_PREFIX "10-2020q2")
set(GCC_NAME "gcc-arm-none-eabi-10-2020-q2-preview")
set(GCC_URL "https://developer.arm.com/-/media/Files/downloads/gnu-rm/${GCC_PREFIX}/${GCC_NAME}-${GCC_PLATFORM}.tar.bz2")
set(GCC_MD5 "2d19775c5aa091b7f55a68727c5d7d76")
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/gcc)
ExternalProject_Add(gbaGCC
	URL ${GCC_URL}
	URL_HASH MD5=${GCC_MD5}
	DOWNLOAD_NO_EXTRACT TRUE
	DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/external
	UPDATE_COMMAND tar xjf ${CMAKE_SOURCE_DIR}/external/${GCC_NAME}-${GCC_PLATFORM}.tar.bz2 -C ${CMAKE_SOURCE_DIR}/external/gcc --strip-components 1
	CONFIGURE_COMMAND cmake -E echo "Skipping configure step."
	BUILD_COMMAND cmake -E echo "Skipping BUILD step."
	INSTALL_COMMAND cmake -E echo "Skipping install step.")

#	TONC is needed
set(TONC_URL "http://www.coranac.com/files/tonc-code.zip")
set(TONC_MD5 "3f9ebdbbe7989b33c2a788c5b5a1bfed")
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/tonc)
ExternalProject_Add(Tonc
	URL ${TONC_URL}
	URL_HASH MD5=${TONC_MD5}
	DOWNLOAD_NO_EXTRACT TRUE
	DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/external
	UPDATE_COMMAND unzip -qq -n ${CMAKE_SOURCE_DIR}/external/tonc-code.zip -d ${CMAKE_SOURCE_DIR}/external/tonc
	CONFIGURE_COMMAND cmake -E echo "Skipping configure step."
	BUILD_COMMAND cmake -E echo "Skipping BUILD step."
	INSTALL_COMMAND cmake -E echo "Skipping install step."
	DEPENDS gbaGCC)

#	LibSampleRate is needed
set(LSRC_VERSION "0.1.9")
set(LSRC_URL "http://www.mega-nerd.com/SRC/libsamplerate-${LSRC_VERSION}.tar.gz")
set(LSRC_MD5 "2b78ae9fe63b36b9fbb6267fad93f259")
set(LSRC_INSTALL_FOLDER ${CMAKE_SOURCE_DIR}/external/libsamplerate-install/)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/libsamplerate)
ExternalProject_Add(LibSampleRateSource
	URL ${LSRC_URL}
	URL_HASH MD5=${LSRC_MD5}
	DOWNLOAD_NO_EXTRACT TRUE
	DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/external
	UPDATE_COMMAND tar xzf ${CMAKE_SOURCE_DIR}/external/libsamplerate-${LSRC_VERSION}.tar.gz -C ${CMAKE_SOURCE_DIR}/external/libsamplerate --strip-components 1
	CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/external/libsamplerate/configure --prefix ${LSRC_INSTALL_FOLDER}
	BUILD_COMMAND make
	INSTALL_COMMAND make install
	DEPENDS Tonc)

# Libraries
set(TONC_PATH "${CMAKE_SOURCE_DIR}/external/tonc/tonclib")
set(LIBRARIES_ "-nodefaultlibs -ltonc")
set(LIBDIRS ${TONC_PATH})
set(INCDIRS "${CMAKE_SOURCE_DIR}/external/gcem/include")

# External Project is used to generate tools, 
# so we can use the normal gcc and g++ compilers,
# because we use ARM compilers to build the ROM
ExternalProject_Add(tools
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools
	BINARY_DIR ${CMAKE_BINARY_DIR}/tools
	INSTALL_COMMAND cmake -E echo "Skipping install step."
	DEPENDS LibSampleRateSource)

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
	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated
	foreach(SPRITE ${SPR_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools sprite-export
		${CMAKE_SOURCE_DIR}/data/sprites/${SPRITE}.png
		${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.s
		${CMAKE_BINARY_DIR}/data/sprites/${SPRITE}.hpp)
	endforeach()
	foreach(SPRITE ${SPR_FILES})
		# Generate again when we are building, need to clear the files first still, otherwise there may be errors
		# Adding BYPRODUCTS seems to break finding the generated files
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
	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated
	foreach(BACKGROUND ${BKG_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools background-export
		${CMAKE_SOURCE_DIR}/data/backgrounds/${BACKGROUND}.png
		${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.s
		${CMAKE_BINARY_DIR}/data/backgrounds/${BACKGROUND}.hpp)
	endforeach()
	foreach(BACKGROUND ${BKG_FILES})
		# Generate again when we are building, need to clear the files first still, otherwise there may be errors
		# Adding BYPRODUCTS seems to break finding the generated files
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
	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated
	foreach(FONT ${FNT_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools font-export
		${CMAKE_SOURCE_DIR}/data/fonts/${FONT}.ttf
		${CMAKE_BINARY_DIR}/data/fonts/${FONT}.s
		${CMAKE_BINARY_DIR}/data/fonts/${FONT}.hpp)
	endforeach()
	foreach(FONT ${FNT_FILES})
		# Generate again when we are building, need to clear the files first still, otherwise there may be errors
		# Adding BYPRODUCTS seems to break finding the generated files
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
	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated
	foreach(SETTING ${AUS_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools audio-export-settings
		${CMAKE_SOURCE_DIR}/data/${SETTING}.json
		${CMAKE_BINARY_DIR}/data/${SETTING}.hpp
		${CMAKE_BINARY_DIR}/data/${SETTING}.json)
	endforeach()
	foreach(SETTING ${AUS_FILES})
		# Generate again when we are building, need to clear the files first still, otherwise there may be errors
		# Adding BYPRODUCTS seems to break finding the generated files
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
	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated
	foreach(SOUND ${SND_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools sound-export
		${CMAKE_SOURCE_DIR}/data/sounds/${SOUND}.wav
		${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s
		${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp
		${CMAKE_BINARY_DIR}/data/audio-settings.json)
	endforeach()
	foreach(SOUND ${SND_FILES})
		# Generate again when we are building, need to clear the files first still, otherwise there may be errors
		# Adding BYPRODUCTS seems to break finding the generated files
		add_custom_command(OUTPUT ${SOUND}-command
			DEPENDS tools AUS
			COMMAND ${CMAKE_BINARY_DIR}/tools/tools sound-export
				${CMAKE_SOURCE_DIR}/data/sounds/${SOUND}.wav
				${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s
				${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp
				${CMAKE_BINARY_DIR}/data/audio-settings.json
			COMMENT "Exporting Sound ${SOUND}")
		add_custom_target(${SOUND}-sound ALL
			DEPENDS tools ${SOUND}-command
			SOURCES ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s
				${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp
			COMMENT "Building ${SOUND}")
		list(APPEND SOUNDS_L ${SOUND}-sound)
		list(APPEND SOUNDS_S ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.s)
		list(APPEND SOUNDS_HPP ${CMAKE_BINARY_DIR}/data/sounds/${SOUND}.hpp)
	endforeach()

	add_custom_target(SOUNDS ALL
		DEPENDS AUS ${SOUNDS_L} ${SOUNDS_S} ${SOUNDS_HPP}
		SOURCES ${SOUNDS_S} ${SOUNDS_HPP}
		COMMENT "Building SOUNDS")

	add_custom_target(RESOURCES_ASSEMBLY ALL
		DEPENDS SPRITES
			BACKGROUNDS
			FONTS
			AUS
			SOUNDS
		SOURCES ${SPRITES_S}
			${BACKGROUNDS_S}
			${FONTS_S}
			${SOUNDS_S})

	add_custom_target(RESOURCES_HPP ALL
		DEPENDS SPRITES
			BACKGROUNDS
			FONTS
			AUS
			SOUNDS
		SOURCES ${SPRITES_HPP}
			${BACKGROUNDS_HPP}
			${FONTS_HPP}
			${AUS_HPP}
			${SOUNDS_HPP})

	add_custom_target(RESOURCE_OBJECTS ALL
		DEPENDS RESOURCES_ASSEMBLY RESOURCES_HPP		
		COMMENT "Building RESOURCE_OBJECTS")
	
	# Source files
	# IWRAM C Files
	set(IWRAM_C_FILES
		/util/memmove.iwram
		/util/random.iwram)

	# IWRAM CPP Files
	set(IWRAM_CPP_FILES
		/gameplay/Projectile.iwram
		/graphics/OamManager.iwram
		/memory/allocator.iwram)
	
	# C Files
	set(GAME_C_FILES
		/math/gcd
		/util/libc-replacement
		/util/memmove.iwram
		/util/random.iwram)

	# CPP Files
	set(GAME_CPP_FILES
		${CMAKE_SOURCE_DIR}/source/main.cpp
		${CMAKE_SOURCE_DIR}/source/audio/audio.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Background.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/collisionCircleBitmaskGenerator.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Enemy.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/EnemyProjectiles.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Explosions.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/GameScene.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Hud.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Level.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Player.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/PlayerProjectiles.cpp		
		${CMAKE_SOURCE_DIR}/source/gameplay/levels/level1/bigLurker.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/levels/level1/level.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/levels/level1/wavingEnemy.cpp
		${CMAKE_SOURCE_DIR}/source/gameplay/levels/level1/wobblingEnemies.cpp
		${CMAKE_SOURCE_DIR}/source/graphics/BuddyObjectAllocator.cpp
		${CMAKE_SOURCE_DIR}/source/graphics/FrameAnimator.cpp
		${CMAKE_SOURCE_DIR}/source/graphics/graphics.cpp
		${CMAKE_SOURCE_DIR}/source/graphics/HblankDma.cpp
		${CMAKE_SOURCE_DIR}/source/graphics/OamManager.cpp		
		${CMAKE_SOURCE_DIR}/source/graphics/StreamAnimator.cpp
		${CMAKE_SOURCE_DIR}/source/math/trapdiv0.cpp		
		${CMAKE_SOURCE_DIR}/source/scenes/BitmaskTestScene.cpp
		${CMAKE_SOURCE_DIR}/source/scenes/ContextTestScene.cpp
		${CMAKE_SOURCE_DIR}/source/scenes/Mode3TextTestScene.cpp
		${CMAKE_SOURCE_DIR}/source/scenes/TileTextTestScene.cpp
		${CMAKE_SOURCE_DIR}/source/util/dynamic.cpp
		${CMAKE_SOURCE_DIR}/source/util/profile.cpp
		${CMAKE_SOURCE_DIR}/source/util/virtual.cpp)

	# S Files
	set(GAME_S_FILES
		${CMAKE_SOURCE_DIR}/source/crt0.s
		${CMAKE_SOURCE_DIR}/source/audio/audioMix.s
		${CMAKE_SOURCE_DIR}/source/gameplay/collision.s
		${CMAKE_SOURCE_DIR}/source/gameplay/collisionBitmask.s
		${CMAKE_SOURCE_DIR}/source/gameplay/Projectile.s
		${CMAKE_SOURCE_DIR}/source/math/clz.s
		${CMAKE_SOURCE_DIR}/source/math/lmul.s
		${CMAKE_SOURCE_DIR}/source/math/sdiv32.s
		${CMAKE_SOURCE_DIR}/source/math/sqrt32.s
		${CMAKE_SOURCE_DIR}/source/math/sqrt64.s
		${CMAKE_SOURCE_DIR}/source/math/udiv32.s
		${CMAKE_SOURCE_DIR}/source/text/mode3PutGlyph.s
		${CMAKE_SOURCE_DIR}/source/text/tile4bppPutGlyph.s
		${CMAKE_SOURCE_DIR}/source/util/context.s
		${CMAKE_SOURCE_DIR}/source/util/memops.s
		${CMAKE_SOURCE_DIR}/source/util/uintDigits.s)

		# HPP Files - Is this even needed? Don't think so
	set(GAME_HPP_FILES
		${CMAKE_SOURCE_DIR}/source/colors.hpp
		${CMAKE_SOURCE_DIR}/source/audio/audio.hpp
		${CMAKE_SOURCE_DIR}/source/data/AnimatedPng.hpp
		${CMAKE_SOURCE_DIR}/source/data/BackgroundData.hpp
		${CMAKE_SOURCE_DIR}/source/data/BitmaskData.hpp
		${CMAKE_SOURCE_DIR}/source/data/DataPng.hpp
		${CMAKE_SOURCE_DIR}/source/data/Font.hpp
		${CMAKE_SOURCE_DIR}/source/data/Sound.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Background.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/collision.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Enemy.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/EnemyProjectiles.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Explosions.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/GameScene.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Hud.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Level.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Player.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/PlayerProjectiles.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/Projectile.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/vecUtils.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/levels/level1/bigLurker.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/levels/level1/wavingEnemy.hpp
		${CMAKE_SOURCE_DIR}/source/gameplay/levels/level1/wobblingEnemies.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/AnimationPose.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/BuddyObjectAllocator.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/FrameAnimator.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/graphics.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/HBlankDma.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/OamManager.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/ObjectTilePointer.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/PalettePointer.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/SpriteSize.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/StillImagePointer.hpp
		${CMAKE_SOURCE_DIR}/source/graphics/StreamAnimator.hpp
		${CMAKE_SOURCE_DIR}/source/math/fixed.hpp
		${CMAKE_SOURCE_DIR}/source/math/fixedmath.hpp
		${CMAKE_SOURCE_DIR}/source/math/gcem-degrees.hpp
		${CMAKE_SOURCE_DIR}/source/math/movetowards.hpp
		${CMAKE_SOURCE_DIR}/source/math/stdfixed.hpp
		${CMAKE_SOURCE_DIR}/source/math/vec2.hpp
		${CMAKE_SOURCE_DIR}/source/memory/allocator.hpp
		${CMAKE_SOURCE_DIR}/source/scenes/BitmaskTestScene.hpp
		${CMAKE_SOURCE_DIR}/source/scenes/ContextTestScene.hpp
		${CMAKE_SOURCE_DIR}/source/scenes/IScene.hpp
		${CMAKE_SOURCE_DIR}/source/scenes/Mode3TextTestScene.hpp
		${CMAKE_SOURCE_DIR}/source/scenes/TileTextTestScene.hpp
		${CMAKE_SOURCE_DIR}/source/text/aslatin1.hpp
		${CMAKE_SOURCE_DIR}/source/text/mGBADebugging.hpp
		${CMAKE_SOURCE_DIR}/source/text/Mode3GlyphWriter.hpp
		${CMAKE_SOURCE_DIR}/source/text/StringBuilder.hpp
		${CMAKE_SOURCE_DIR}/source/text/TextWriter.hpp
		${CMAKE_SOURCE_DIR}/source/text/Tile4bppGlyphWriter.hpp
		${CMAKE_SOURCE_DIR}/source/util/AllocatorPointer.hpp
		${CMAKE_SOURCE_DIR}/source/util/bit-pointers.hpp
		${CMAKE_SOURCE_DIR}/source/util/flags-enum.hpp
		${CMAKE_SOURCE_DIR}/source/util/gba-assert.hpp
		${CMAKE_SOURCE_DIR}/source/util/generateTable.hpp
		${CMAKE_SOURCE_DIR}/source/util/integer-sequence-utils.hpp
		${CMAKE_SOURCE_DIR}/source/util/LightBitset.hpp
		${CMAKE_SOURCE_DIR}/source/util/ObjectStack.hpp)

	#add_library(GAME_OBJECTS OBJECT
	#	${GAME_C_FILES}
	#	${GAME_CPP_FILES}
	#	${GAME_S_FILES}
	#	${GAME_HPP_FILES})

	#target_include_directories(GAME_OBJECTS PRIVATE
	#	${CMAKE_SOURCE_DIR}/external/tonc/tonclib/include)


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

	set(ARMCC "${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-gcc")
	set(ARMCPP "${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-g++")
	set(ARMOC "${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-objcopy")
	set(ARMOD "${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-objdump")

	set(LibSampleRate_DIR ${CMAKE_SOURCE_DIR}/external/libsamplerate-install)
	find_package(LibSampleRate REQUIRED)

	# Source files
	#build/%.o: %.c
	#	${CMAKE_C_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -c $< -o $@
	foreach (FILE ${GAME_C_FILES})
		string(REPLACE "/" "-" FILE_COM ${FILE})
		message(STATUS ${FILE_COM})
		add_custom_command(OUTPUT c-${FILE_COM}-object-command
			COMMAND ${ARMCC} -MMD -MP -MF ${CMAKE_BINARY_DIR}/source${FILE}.d ${CFLAGS} ${INCDIR} -c ${CMAKE_SOURCE_DIR}/source${FILE}.c -o ${CMAKE_BINARY_DIR}/source${FILE}.o
			DEPENDS RESOURCE_OBJECTS
			COMMENT "Building C File Object ${FILE}")
		add_custom_target(c-${FILE_COM}-object ALL
			DEPENDS c-${FILE_COM}-object-command 
			#SOURCES ${CMAKE_BINARY_DIR}/source${FILE}.o ${CMAKE_BINARY_DIR}/source${FILE}.d
			COMMENT "Built ${FILE} Object")
		list(APPEND C_OBJECTS_DEPENDS c-${FILE_COM}-object)
		list(APPEND C_OBJECTS_SOURCES ${CMAKE_BINARY_DIR}/source${FILE}.o ${CMAKE_BINARY_DIR}/source${FILE}.d)
	endforeach()
	
	add_custom_target(C_OBJECT_FILES ALL
		DEPENDS C_OBJECTS_DEPENDS)

	#build/%.iwram.o: %.iwram.c
	#	${CMAKE_C_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@
	foreach (FILE ${IWRAM_C_FILES})
		string(REPLACE "/" "-" FILE_COM ${FILE})
		message(STATUS ${FILE_COM})
		add_custom_command(OUTPUT iwram-${FILE_COM}-object-command
			COMMAND ${ARMCC} -MMD -MP -MF ${CMAKE_BINARY_DIR}/source${FILE}.d ${CFLAGS} ${INCDIR} -fno-lto -marm -mlong-calls -c ${CMAKE_SOURCE_DIR}/source${FILE}.c -o ${CMAKE_BINARY_DIR}/source${FILE}.o
			DEPENDS C_OBJECT_FILES
			COMMENT "Building IWRAM C File Object ${FILE}")
		add_custom_target(iwram-${FILE_COM}-object ALL
			DEPENDS iwram-${FILE_COM}-object-command 
			#SOURCES ${CMAKE_BINARY_DIR}/source${FILE}.o ${CMAKE_BINARY_DIR}/source${FILE}.d
			COMMENT "Built ${FILE} Object")
		list(APPEND IWRAM_C_OBJECTS_DEPENDS iwram-${FILE_COM}-object)
		list(APPEND IWRAM_C_OBJECTS_SOURCES ${CMAKE_BINARY_DIR}/source${FILE}.o ${CMAKE_BINARY_DIR}/source${FILE}.d)
	endforeach()
		
	add_custom_target(IWRAM_C_OBJECT_FILES ALL
		DEPENDS IWRAM_C_OBJECTS_DEPENDS)
	
	#build/%.o: %.cpp
	add_custom_target(CPP_OBJECTS ALL
		DEPENDS IWRAM_C_OBJECT_FILES)
	#	${CMAKE_CXX_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -c $< -o $@
	
	#build/%.iwram.o: %.iwram.cpp
	add_custom_target(IWRAM_CPP_OBJECT_FILES ALL
		DEPENDS CPP_OBJECTS)
	#	${CMAKE_CXX_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@
	
	# Build Resource Assembly File Objects
	#-MP means we construct a phony target for each dependency other than the main file, causing it to depend on nothing
	#-MMD and -MF file, specifies a file to write the dependencies to, it overrides the default dependecy output thanks to -MMD
	#-x assembler-with-cpp indicates that the assembly code contains C directives and armclang must run the C preprocessor
	#-c output_file writes all comments passed through to the output file
	#-o output file
	#${ARMCC} -MMD -MP -MF ${FILE}.d -x assembler-with-cpp ${ASFLAGS} -c ${FILE}.s -o ${FILE}.o
	foreach (FILE ${SPR_FILES})
		add_custom_command(OUTPUT ${FILE}-object-command
			COMMAND ${ARMCC} -MMD -MP -MF ${CMAKE_BINARY_DIR}/data/sprites/${FILE}.d -x assembler-with-cpp ${ASFLAGS} -c ${CMAKE_BINARY_DIR}/data/sprites/${FILE}.s -o ${CMAKE_BINARY_DIR}/data/sprites/${FILE}.o
			DEPENDS IWRAM_CPP_OBJECT_FILES
			COMMENT "Building Assembly File Object ${FILE}")
		add_custom_target(${FILE}-object ALL
			DEPENDS ${FILE}-object-command
			SOURCES ${CMAKE_BINARY_DIR}/data/sprites/${FILE}.o ${CMAKE_BINARY_DIR}/data/sprites/${FILE}.d
			COMMENT "Built ${FILE} Object")
		list(APPEND ASSEMBLY_OBJECTS_DEPENDS ${FILE}-object)
		list(APPEND ASSEMBLY_OBJECTS_SOURCES ${CMAKE_BINARY_DIR}/data/sprites/${FILE}.o ${CMAKE_BINARY_DIR}/data/sprites/${FILE}.d)
	endforeach()
	foreach (FILE ${BKG_FILES})
		add_custom_command(OUTPUT ${FILE}-object-command
			COMMAND ${ARMCC} -MMD -MP -MF ${CMAKE_BINARY_DIR}/data/backgrounds/${FILE}.d -x assembler-with-cpp ${ASFLAGS} -c ${CMAKE_BINARY_DIR}/data/backgrounds/${FILE}.s -o ${CMAKE_BINARY_DIR}/data/backgrounds/${FILE}.o
			DEPENDS IWRAM_CPP_OBJECT_FILES
			COMMENT "Building Assembly File Object ${FILE}")
		add_custom_target(${FILE}-object ALL
			DEPENDS ${FILE}-object-command
			SOURCES ${CMAKE_BINARY_DIR}/data/backgrounds/${FILE}.o ${CMAKE_BINARY_DIR}/data/backgrounds/${FILE}.d
			COMMENT "Built ${FILE} Object")
		list(APPEND ASSEMBLY_OBJECTS_DEPENDS ${FILE}-object)
		list(APPEND ASSEMBLY_OBJECTS_SOURCES ${CMAKE_BINARY_DIR}/data/backgrounds/${FILE}.o ${CMAKE_BINARY_DIR}/data/backgrounds/${FILE}.d)
	endforeach()
	foreach (FILE ${FNT_FILES})
		add_custom_command(OUTPUT ${FILE}-object-command
			COMMAND ${ARMCC} -MMD -MP -MF ${CMAKE_BINARY_DIR}/data/fonts/${FILE}.d -x assembler-with-cpp ${ASFLAGS} -c ${CMAKE_BINARY_DIR}/data/fonts/${FILE}.s -o ${CMAKE_BINARY_DIR}/data/fonts/${FILE}.o
			DEPENDS IWRAM_CPP_OBJECT_FILES
			COMMENT "Building Assembly File Object ${FILE}")
		add_custom_target(${FILE}-object ALL
			DEPENDS ${FILE}-object-command
			SOURCES ${CMAKE_BINARY_DIR}/data/fonts/${FILE}.o ${CMAKE_BINARY_DIR}/data/fonts/${FILE}.d
			COMMENT "Built ${FILE} Object")
		list(APPEND ASSEMBLY_OBJECTS_DEPENDS ${FILE}-object)
		list(APPEND ASSEMBLY_OBJECTS_SOURCES ${CMAKE_BINARY_DIR}/data/fonts/${FILE}.o ${CMAKE_BINARY_DIR}/data/fonts/${FILE}.d)
	endforeach()
	foreach (FILE ${SND_FILES})
		add_custom_command(OUTPUT ${FILE}-object-command
			COMMAND ${ARMCC} -MMD -MP -MF ${CMAKE_BINARY_DIR}/data/sounds/${FILE}.d -x assembler-with-cpp ${ASFLAGS} -c ${CMAKE_BINARY_DIR}/data/sounds/${FILE}.s -o ${CMAKE_BINARY_DIR}/data/sounds/${FILE}.o
			DEPENDS IWRAM_CPP_OBJECT_FILES
			COMMENT "Building Assembly File Object ${FILE}")
		add_custom_target(${FILE}-object ALL
			DEPENDS ${FILE}-object-command
			SOURCES ${CMAKE_BINARY_DIR}/data/sounds/${FILE}.o ${CMAKE_BINARY_DIR}/data/sounds/${FILE}.d
			COMMENT "Built ${FILE} Object")
		list(APPEND ASSEMBLY_OBJECTS_DEPENDS ${FILE}-object)
		list(APPEND ASSEMBLY_OBJECTS_SOURCES ${CMAKE_BINARY_DIR}/data/sounds/${FILE}.o ${CMAKE_BINARY_DIR}/data/sounds/${FILE}.d)
	endforeach()
	add_custom_target(ASSEMBLY_OBJECTS ALL
		DEPENDS ${ASSEMBLY_OBJECTS_DEPENDS}
		SOURCES ${ASSEMBLY_OBJECTS_SOURCES})
	
	# Build Source Assembly File Objects

	#bin/game.elf: build/data/audio-settings.hpp $(OFILES)
	# Get the right linker
	#ifeq ($(strip $(CPPFILES)),)
	#	ARMLD := $(ARMCC)
	#else
	#	ARMLD := $(ARMCPP)
	#endif
	#	@$(ARMLD) $(LDFLAGS) -specs=gba.specs $(filter-out %crt0.o, $(OFILES)) $(LIBPATHS) $(LIBRARIES) -o $@

	add_custom_target(OBJECT_DUMP ALL
		DEPENDS IWRAM_CPP_OBJECT_FILES IWRAM_C_OBJECT_FILES CPP_OBJECTS C_OBJECT_FILES ASSEMBLY_OBJECTS
	#	-d --disassmble Display the assembler mnemonics for the machine instructions from the input file.
	#		This option only disassembles those sections which are expected to contain instructions.
	#		If the optional symbol argument is given, then display the assembler mnemonics starting at symbol.
	#		If symbol is a function name then disassembly will stop at the end of the function,
	#		otherwise it will stop when the next symbol is encountered. If there are no matches for symbol then nothing will be displayed.
	#	-C --demangle Decode (demangle) low-level symbol names into user-level names.
	#		Besides removing any initial underscore prepended by the system, this makes C++ function names readable.
	#		Different compilers have different mangling styles.
	#		The optional demangling style argument can be used to choose an appropriate demangling style for your compiler.
	#	-S --source Display source code intermixed with disassembly, if possible. Implies -d.
	#	INFILE .elf
	#	OUTFILE .elf.dump
	#	COMMAND @${CMAKE_OBJDUMP} -dCS ${CMAKE_BINARY_DIR}/gba-modern.elf > ${CMAKE_BINARY_DIR}/gba-modern.elf.dump
		)

	#bin/game.gba:
	add_custom_target(ROM_BUILD ALL
		DEPENDS OBJECT_DUMP
		COMMENT "Building ROM"
	#	-O binary Write the output file using binary.
	#	INFILE .elf
	#	OUTFILE .gba
	#	@${CMAKE_OBJCOPY} -O binary ${CMAKE_BINARY_DIR}/gba-modern.elf ${CMAKE_BINARY_DIR}/gba-modern.gba
		)

	add_custom_target(ROM ALL
		DEPENDS ROM_BUILD
		COMMENT "Sanitising final file"
		#COMMAND ${CMAKE_BINARY_DIR}/tools/tools rom-sanitize ${CMAKE_BINARY_DIR}/gba-modern.gba gba.json ${CMAKE_BINARY_DIR}/gba-modern.gba
		)
endif()