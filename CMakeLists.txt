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
set(ASFLAGS -g ${ARCH})

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
set(TONC_PATH "${CMAKE_SOURCE_DIR}/external/tonc/code/tonclib")
set(LIBRARIES_ "-nodefaultlibs -ltonc")
set(LIBDIRS ${TONC_PATH})
set(INCDIRS "${CMAKE_SOURCE_DIR}/external/gcem/include")

set(EXTERNAL_LIBS
	"${TONC_PATH}/lib")

set(EXTERNAL_INCLUDES "${INCDIRS} ${TONC_PATH}/include")


# External Project is used to generate tools, 
# so we can use the normal gcc and g++ compilers,
# because we use ARM compilers to build the ROM
ExternalProject_Add(tools
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools
	BINARY_DIR ${CMAKE_BINARY_DIR}/tools
	BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/tools
	INSTALL_COMMAND cmake -E echo "Skipping install step."
	DEPENDS LibSampleRateSource)

set(ARMCC "${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-gcc")
set(ARMCPP "${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-g++")
set(ARMOC "${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-objcopy")
set(ARMOD "${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-objdump")

set(LibSampleRate_DIR ${CMAKE_SOURCE_DIR}/external/libsamplerate-install)
find_package(LibSampleRate REQUIRED)

# Resource File Definitions
# Sprite Files - Assumed to be .png
set(SPR_FILES
	big-lurker bitmask-test
	enemy-projectiles hud
	player player-projectiles
	small-explosion waving-enemy
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
	player-dmg player-heal
	player-shot)

# Source files
# IWRAM C Files
set(IWRAM_C_FILES
	util/memmove.iwram util/random.iwram)

# IWRAM CPP Files
set(IWRAM_CPP_FILES
	gameplay/Projectile.iwram graphics/OamManager.iwram
	memory/allocator.iwram)
	
# C Files
set(GAME_C_FILES
	math/gcd util/libc-replacement)

# CPP Files
set(GAME_CPP_FILES
	main
	audio/audio
	gameplay/Background gameplay/collisionCircleBitmaskGenerator
	gameplay/Enemy gameplay/EnemyProjectiles
	gameplay/Explosions gameplay/GameScene
	gameplay/Hud gameplay/Level
	gameplay/Player gameplay/PlayerProjectiles		
	gameplay/levels/level1/bigLurker gameplay/levels/level1/level
	gameplay/levels/level1/wavingEnemy gameplay/levels/level1/wobblingEnemies
	graphics/BuddyObjectAllocator graphics/FrameAnimator
	graphics/graphics graphics/HblankDma
	graphics/OamManager graphics/StreamAnimator
	math/trapdiv0 scenes/BitmaskTestScene
	scenes/ContextTestScene scenes/Mode3TextTestScene
	scenes/TileTextTestScene util/dynamic
	util/profile util/virtual)

# S Files
set(GAME_S_FILES
	crt0 audio/audioMix
	gameplay/collision gameplay/collisionBitmask
	gameplay/Projectile math/clz
	math/lmul math/sdiv32
	math/sqrt32 math/sqrt64
	math/udiv32 text/mode3PutGlyph
	text/tile4bppPutGlyph util/context
	util/memops util/uintDigits)

# HPP Files - Is this even needed? Don't think so
set(GAME_HPP_FILES
	colors audio/audio
	data/AnimatedPng data/BackgroundData
	data/BitmaskData data/DataPng
	data/Font data/Sound
	gameplay/Background gameplay/collision
	gameplay/Enemy gameplay/EnemyProjectiles
	gameplay/Explosions gameplay/GameScene
	gameplay/Hud gameplay/Level
	gameplay/Player gameplay/PlayerProjectiles
	gameplay/Projectile gameplay/vecUtils
	gameplay/levels/level1/bigLurker gameplay/levels/level1/wavingEnemy
	gameplay/levels/level1/wobblingEnemies graphics/AnimationPose
	graphics/BuddyObjectAllocator graphics/FrameAnimator
	graphics/graphics graphics/HBlankDma
	graphics/OamManager graphics/ObjectTilePointer
	graphics/PalettePointer graphics/SpriteSize
	graphics/StillImagePointer graphics/StreamAnimator
	math/fixed math/fixedmath
	math/gcem-degrees math/movetowards
	math/stdfixed math/vec2
	memory/allocator scenes/BitmaskTestScene
	scenes/ContextTestScene scenes/IScene
	scenes/Mode3TextTestScene scenes/TileTextTestScene
	text/aslatin1 text/mGBADebugging
	text/Mode3GlyphWriter text/StringBuilder
	text/TextWriter text/Tile4bppGlyphWriter
	util/AllocatorPointer util/bit-pointers
	util/flags-enum util/gba-assert
	util/generateTable util/integer-sequence-utils
	util/LightBitset util/ObjectStack)

macro(export_resources)
	# Set Variables to make this easier
	set(RESOURCE_NAME ${ARGV0})
	set(PLURAL ${RESOURCE_NAME}S)
	set(TARGET_DEPENDENCIES ${ARGV1})
	set(TOOL_NAME ${ARGV2})
	set(FOLDER_NAME ${ARGV3})
	set(IN1 ${ARGV4})
	set(OUT1 ${ARGV5})
	set(OUT2 ${ARGV6})
	set(RESOURCE_FILES ${ARGV8})
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME})
	set(OBJECT_LIST_NAME ${PLURAL}_OBJECT_LIST)
	set(OUT1_LIST_NAME ${PLURAL}_OUT1_LIST)
	set(OUT2_LIST_NAME ${PLURAL}_OUT2_LIST)

	#Pregenerate so we can see what Resources will be built whenever the CMake is Generated
	foreach (FILE ${RESOURCE_FILES})
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools ${TOOL_NAME}
		${CMAKE_SOURCE_DIR}/data/${FOLDER_NAME}${FILE}${IN1}
		${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT1}
		${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT2}
		${ARGV7})
	endforeach()
	
	foreach(FILE ${RESOURCE_FILES})
		# Generate again when we are building, need to clear the files first still, otherwise there may be errors
		# Adding BYPRODUCTS seems to break finding the generated files
		add_custom_command(OUTPUT 
				${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT1}
				${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT2}
			DEPENDS ${TARGET_DEPENDENCIES}
			COMMAND ${CMAKE_BINARY_DIR}/tools/tools ${TOOL_NAME}
				${CMAKE_SOURCE_DIR}/data/${FOLDER_NAME}${FILE}${IN1}
				${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT1}
				${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT2}
				${ARGV7}
			BYPRODUCTS ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT1}
				${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT2}
			COMMENT "Exporting ${RESOURCE_NAME} ${FILE}")
		set_property(SOURCE ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT1} PROPERTY GENERATED 1)
		set_property(SOURCE ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT2} PROPERTY GENERATED 1)
		add_custom_target(${FILE}-${RESOURCE_NAME} ALL
			DEPENDS tools ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT1}
				${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT2}
			SOURCES ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT1}
				${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT2}
			COMMENT "Building ${FILE}")
		list(APPEND ${OBJECT_LIST_NAME} ${FILE}-${RESOURCE_NAME})
		list(APPEND ${OUT1_LIST_NAME} ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT1})
		list(APPEND ${OUT2_LIST_NAME} ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${FILE}${OUT2})
	endforeach()

	add_custom_target(${PLURAL} ALL
		DEPENDS ${${OBJECT_LIST_NAME}} ${${OUT1_LIST_NAME}} ${${OUT2_LIST_NAME}}
		SOURCES ${${OUT1_LIST_NAME}} ${${OUT2_LIST_NAME}}
		COMMENT "Building ${PLURAL}")
endmacro()

macro(build_resources)
	set(RESOURCE_FILES ${ARGV0})
	set(RESOURCE_FILES ${ARGV1})
	foreach (FILE ${RESOURCE_FILES})
		get_filename_component(file_dir "${FILE}" PATH)
		
		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)

		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)
		list(GET file_new_split 1 file_type)
		file(MAKE_DIRECTORY ${file_dir}/${file_name})
		set_property(SOURCE ${file_dir}/${file_name} PROPERTY GENERATED 1)
		add_custom_command(OUTPUT ${file_dir}/${file_name}/${file_name}.o ${file_dir}/${file_name}/${file_name}.d
			#-MP means we construct a phony target for each dependency other than the main file, causing it to depend on nothing
			#-MMD and -MF file, specifies a file to write the dependencies to, it overrides the default dependecy output thanks to -MMD
			#-x assembler-with-cpp indicates that the assembly code contains C directives and armclang must run the C preprocessor
			#-c output_file writes all comments passed through to the output file
			#-o output file
			COMMAND ${ARMCC} -MMD -MP -MF ${file_dir}/${file_name}/${file_name}.d -x assembler-with-cpp ${ASFLAGS} -c ${file_dir}/${file_name}${file_type} -o ${file_dir}/${file_name}/${file_name}.o
			DEPENDS RESOURCES_ASSEMBLY RESOURCES_HPP ${file_dir}/${file_name}${file_type}
			BYPRODUCTS ${file_dir}/${file_name}/${file_name}.o ${file_dir}/${file_name}/${file_name}.d
			COMMENT "Building Assembly File Object ${file_name}.o and Dependency ${file_name}.d")
		set_property(SOURCE ${file_dir}/${file_name}/${file_name}.o PROPERTY GENERATED 1)
		set_property(SOURCE ${file_dir}/${file_name}/${file_name}.d PROPERTY GENERATED 1)
		add_custom_target(${file_name}-object ALL
			DEPENDS ${file_dir}/${file_name}${file_type} ${file_dir}/${file_name}/${file_name}.o ${file_dir}/${file_name}/${file_name}.d
			SOURCES ${file_dir}/${file_name}/${file_name}.o ${file_dir}/${file_name}/${file_name}.d
			COMMENT "Built Assembly File Object ${file_name}.o and Dependency ${file_name}.d")
		list(APPEND RESOURCES_OBJECTS_DEPENDS ${file_name}-object)
		list(APPEND RESOURCES_OBJECTS_SOURCES ${file_dir}/${file_name}/${file_name}.o ${file_dir}/${file_name}/${file_name}.d)
	endforeach()
endmacro()

export_resources("SPRITE" "tools" "sprite-export" "sprites/" ".png" ".s" ".hpp" "" "${SPR_FILES}")
export_resources("BACKGROUND" "SPRITES" "background-export" "backgrounds/" ".png" ".s" ".hpp" "" "${BKG_FILES}")
export_resources("FONT" "BACKGROUNDS" "font-export" "fonts/" ".ttf" ".s" ".hpp" "" "${FNT_FILES}")
export_resources("SETTING" "FONTS" "audio-export-settings" "" ".json" ".hpp" "-settings.json" "" "${AUS_FILES}")
export_resources("SOUND" "SETTINGS" "sound-export" "sounds/" ".wav" ".s" ".hpp" "${CMAKE_BINARY_DIR}/data/audio-settings.json" "${SND_FILES}")

add_custom_target(RESOURCES_ASSEMBLY ALL
	DEPENDS SPRITES
		BACKGROUNDS
		FONTS
		SETTINGS
		SOUNDS
	SOURCES ${SPRITES_OUT1_LIST}
		${BACKGROUNDS_OUT1_LIST}
		${FONTS_OUT1_LIST}
		${SOUNDS_OUT1_LIST})

add_custom_target(RESOURCES_HPP ALL
	DEPENDS SPRITES
		BACKGROUNDS
		FONTS
		SETTINGS
		SOUNDS
	SOURCES ${SPRITES_OUT2_LIST}
		${BACKGROUNDS_OUT2_LIST}
		${FONTS_OUT2_LIST}
		${AUS_OUT2_LIST}
		${SOUNDS_OUT2_LIST})

build_resources("SPRITES" "${SPRITES_OUT1_LIST}")
build_resources("BACKGROUNDS" "${BACKGROUNDS_OUT1_LIST}")
build_resources("FONTS" "${FONTS_OUT1_LIST}")
build_resources("SOUNDS" "${SOUNDS_OUT1_LIST}")

add_custom_target(RESOURCES ALL
	DEPENDS ${RESOURCES_OBJECTS_DEPENDS}
	SOURCES ${RESOURCES_OBJECTS_SOURCES}
	COMMENT "RESOURCES Built!")
	
	
#add_library(GAME_OBJECTS OBJECT
#	${GAME_C_FILES}
#	${GAME_CPP_FILES}
#	${GAME_S_FILES}
#	${GAME_HPP_FILES})

#target_include_directories(GAME_OBJECTS PRIVATE
#	${CMAKE_SOURCE_DIR}/external/tonc/tonclib/include)
	
# Source files
#build/%.o: %.c
#	${CMAKE_C_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -c $< -o $@
#foreach (FILE ${GAME_C_FILES})
#	string(REPLACE "/" "-" FILE_COM ${FILE})
#	message(STATUS ${FILE_COM})
#	set(COMMAND "${ARMCC} -MMD -MP -MF ${CMAKE_BINARY_DIR}/source${FILE}.d ${CFLAGS} ${EXTERNAL_INCLUDES} -c ${CMAKE_SOURCE_DIR}/source${FILE}.c -o ${CMAKE_BINARY_DIR}/source${FILE}.o")
#	message(STATUS ${COMMAND})
#	add_custom_command(OUTPUT c-${FILE_COM}-object-command
#		COMMAND ${COMMAND}
#		DEPENDS ASSEMBLY_RESOURCE_OBJECTS
#		COMMENT "${COMMAND}")
#	add_custom_target(c-${FILE_COM}-object ALL
#		DEPENDS c-${FILE_COM}-object-command 
#		#SOURCES ${CMAKE_BINARY_DIR}/source${FILE}.o ${CMAKE_BINARY_DIR}/source${FILE}.d
#		COMMENT "Built ${FILE} Object")
#	list(APPEND C_OBJECTS_DEPENDS c-${FILE_COM}-object)
#	list(APPEND C_OBJECTS_SOURCES ${CMAKE_BINARY_DIR}/source${FILE}.o ${CMAKE_BINARY_DIR}/source${FILE}.d)
#endforeach()
	
#add_custom_target(C_OBJECT_FILES ALL
#	DEPENDS C_OBJECTS_DEPENDS)

#build/%.iwram.o: %.iwram.c
#	${CMAKE_C_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@
#foreach (FILE ${IWRAM_C_FILES})
#	string(REPLACE "/" "-" FILE_COM ${FILE})
#	message(STATUS ${FILE_COM})
#	add_custom_command(OUTPUT iwram-${FILE_COM}-object-command
#		COMMAND ${ARMCC} -MMD -MP -MF ${CMAKE_BINARY_DIR}/source${FILE}.d ${CFLAGS} ${INCDIR} -fno-lto -marm -mlong-calls -c ${CMAKE_SOURCE_DIR}/source${FILE}.c -o ${CMAKE_BINARY_DIR}/source${FILE}.o
#		DEPENDS C_OBJECT_FILES
#		COMMENT "Building IWRAM C File Object ${FILE}")
#	add_custom_target(iwram-${FILE_COM}-object ALL
#		DEPENDS iwram-${FILE_COM}-object-command 
#		#SOURCES ${CMAKE_BINARY_DIR}/source${FILE}.o ${CMAKE_BINARY_DIR}/source${FILE}.d
#		COMMENT "Built ${FILE} Object")
#	list(APPEND IWRAM_C_OBJECTS_DEPENDS iwram-${FILE_COM}-object)
#	list(APPEND IWRAM_C_OBJECTS_SOURCES ${CMAKE_BINARY_DIR}/source${FILE}.o ${CMAKE_BINARY_DIR}/source${FILE}.d)
#endforeach()
#	
#add_custom_target(IWRAM_C_OBJECT_FILES ALL
#	DEPENDS IWRAM_C_OBJECTS_DEPENDS)
	
#build/%.o: %.cpp
#add_custom_target(CPP_OBJECTS ALL
#	DEPENDS IWRAM_C_OBJECT_FILES)
#	${CMAKE_CXX_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -c $< -o $@
	
#build/%.iwram.o: %.iwram.cpp
#add_custom_target(IWRAM_CPP_OBJECT_FILES ALL
#	DEPENDS CPP_OBJECTS)
#	${CMAKE_CXX_COMPILER} -MMD -MP -MF $(@:.o=.d) $(CPPFLAGS) $(INCLUDE) -fno-lto -marm -mlong-calls -c $< -o $@
		
# Build Source Assembly File Objects

#bin/game.elf: build/data/audio-settings.hpp $(OFILES)
# Get the right linker
#ifeq ($(strip $(CPPFILES)),)
#	ARMLD := $(ARMCC)
#else
#	ARMLD := $(ARMCPP)
#endif
#	@$(ARMLD) $(LDFLAGS) -specs=gba.specs $(filter-out %crt0.o, $(OFILES)) $(LIBPATHS) $(LIBRARIES) -o $@

#add_custom_target(OBJECT_DUMP ALL
#	DEPENDS IWRAM_CPP_OBJECT_FILES IWRAM_C_OBJECT_FILES CPP_OBJECTS C_OBJECT_FILES ASSEMBLY_OBJECTS
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
#	)

#bin/game.gba:
#add_custom_target(ROM_BUILD ALL
#	DEPENDS OBJECT_DUMP
#	COMMENT "Building ROM"
#	-O binary Write the output file using binary.
#	INFILE .elf
#	OUTFILE .gba
#	@${CMAKE_OBJCOPY} -O binary ${CMAKE_BINARY_DIR}/gba-modern.elf ${CMAKE_BINARY_DIR}/gba-modern.gba
#	)

#add_custom_target(ROM ALL
#	DEPENDS ROM_BUILD
#	COMMENT "Sanitising final file"
#	#COMMAND ${CMAKE_BINARY_DIR}/tools/tools rom-sanitize ${CMAKE_BINARY_DIR}/gba-modern.gba gba.json ${CMAKE_BINARY_DIR}/gba-modern.gba
#	)