# Projects Settings
cmake_minimum_required (VERSION 3.16)

include(ExternalProject)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$CMAKE_SOURCE_DIR}")
set(CMAKE_VERBOSE_MAKEFILE ON)
project (gba-modern)

message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}.") #this is needed to actually load the toolchain file from the config for some reason lol
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_USE_RELATIVE_PATHS TRUE)

set(ARCH "-mthumb -mthumb-interwork")
set(CFLAGS "-Wall -O3 -ffunction-sections -fdata-sections -mcpu=arm7tdmi -mtune=arm7tdmi -flto ${ARCH}")
set(CPPFLAGS "-std=c++17 -fno-rtti -fno-exceptions")
set(ASFLAGS "${ARCH}")
set(LDFLAGS	"${ARCH} -Wl,--gc-sections -Wl,-Map,gba.map")

# Dependencies
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


set(LIBSAMPLERATE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/libsamplerate-install)
set(LIBSAMPLERATE_LIBRARY ${CMAKE_SOURCE_DIR}/external/libsamplerate-install/lib/libsamplerate.so)

set(EXTERNAL_LIBS
	"${CMAKE_SOURCE_DIR}/external/tonc/code/tonclib/lib")

set(EXTERNAL_INCLUDES
	${CMAKE_SOURCE_DIR}/external/tonc/code/tonclib/include ${CMAKE_SOURCE_DIR}/external/gcem/include)


ExternalProject_Add(tools
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools
	BINARY_DIR ${CMAKE_BINARY_DIR}/tools
	BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/tools
	INSTALL_COMMAND cmake -E echo "Skipping install step."
	DEPENDS LibSampleRateSource)

set(ARMCC ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-gcc)
set(ARMCPP ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-g++)
set(ARMOC ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-objcopy)
set(ARMOD ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-objdump)

# Resource File Definitions
file(GLOB_RECURSE SPR_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/data/sprites/ ${PROJECT_SOURCE_DIR}/data/sprites/*.png)
file(GLOB_RECURSE BKG_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/data/backgrounds/ ${PROJECT_SOURCE_DIR}/data/backgrounds/*.png)
file(GLOB_RECURSE FNT_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/data/fonts/ ${PROJECT_SOURCE_DIR}/data/fonts/*.ttf)
set(AUS_FILES
	audio.json) # Not using GLOB here since its just a single file, and for audio. There seems to be some other json files in the data directories too.
file(GLOB_RECURSE SND_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/data/sounds/ ${PROJECT_SOURCE_DIR}/data/sounds/*.wav)

# Source files
file(GLOB_RECURSE IWRAM_C_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/source ${PROJECT_SOURCE_DIR}/source/*.iwram.c)
file(GLOB_RECURSE IWRAM_CPP_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/source ${PROJECT_SOURCE_DIR}/source/*.iwram.cpp)
file(GLOB_RECURSE GAME_C_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/source/*.c)
list(FILTER GAME_C_FILES EXCLUDE REGEX ".*iwram.c$")
file(GLOB_RECURSE GAME_CPP_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/source/*.cpp)
list(FILTER GAME_CPP_FILES EXCLUDE REGEX ".*iwram.cpp$")
file(GLOB_RECURSE GAME_ASM_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/source ${PROJECT_SOURCE_DIR}/source/*.s)
file(GLOB_RECURSE GAME_HPP_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/source ${PROJECT_SOURCE_DIR}/source/*.h*)	

# Build Functions
function(export_resources)
	# Set Variables to make this easier
	# i.e SPRITE
	set(RESOURCE_NAME ${ARGV0})
	# i.e SPRITES
	set(PLURAL ${RESOURCE_NAME}S)
	# passed in dependenccies, tool or the previous object
	set(TARGET_DEPENDENCIES ${ARGV1})
	if(${RESOURCE_NAME} MATCHES "SPRITE")
		set(TOOL_NAME sprite-export)
		set(FOLDER_NAME sprites/)
		set(OUT1 .s)
		set(OUT2 .hpp)
	elseif(${RESOURCE_NAME} MATCHES "BACKGROUND")
		set(TOOL_NAME background-export)
		set(FOLDER_NAME backgrounds/)
		set(OUT1 .s)
		set(OUT2 .hpp)
	elseif(${RESOURCE_NAME} MATCHES "FONT")
		set(TOOL_NAME font-export)
		set(FOLDER_NAME fonts/)
		set(OUT1 .s)
		set(OUT2 .hpp)
	elseif(${RESOURCE_NAME} MATCHES "SETTING")
		set(TOOL_NAME audio-export-settings)
		set(FOLDER_NAME "")
		set(OUT1 -settings.hpp)
		set(OUT2 -settings.json)
	elseif(${RESOURCE_NAME} MATCHES "SOUND")
		set(TOOL_NAME sound-export)
		set(FOLDER_NAME sounds/)
		set(OUT1 .s)
		set(OUT2 .hpp)
	endif()
	set(RESOURCE_FILES ${ARGV2})
	set(IN2 ${ARGV3})

	#make a directory in build folder for the output files
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME})
	
	set(OBJECT_LIST_NAME ${PLURAL}_OBJECT_LIST)
	set(OUT1_LIST_NAME ${PLURAL}_OUT1_LIST)
	set(OUT2_LIST_NAME ${PLURAL}_OUT2_LIST)

	
	foreach(FILE ${RESOURCE_FILES})
		get_filename_component(file_dir "${FILE}" PATH)
		string(REPLACE "." ";." file ${FILE})#
		list(GET file 0 file_name)
		list(GET file 1 file_type)
		
		set(OUTPUT_FILE_1 ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${file_name}${OUT1})
		set(OUTPUT_FILE_2 ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${file_name}${OUT2})
		
		execute_process(COMMAND ${CMAKE_BINARY_DIR}/tools/tools ${TOOL_NAME}
			./data/${FOLDER_NAME}${file_name}${file_type}
			${OUTPUT_FILE_1} ${OUTPUT_FILE_2} ${IN2}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

		add_custom_command(OUTPUT ${OUTPUT_FILE_1} ${OUTPUT_FILE_2}
			DEPENDS ${TARGET_DEPENDENCIES}
			COMMAND ${CMAKE_BINARY_DIR}/tools/tools ${TOOL_NAME}
				./data/${FOLDER_NAME}${file_name}${file_type}
				${OUTPUT_FILE_1} ${OUTPUT_FILE_2} ${IN2}
			#BYPRODUCTS ${OUTPUT_FILE_1} ${OUTPUT_FILE_2}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			COMMENT "Exporting ${RESOURCE_NAME} ${file_name}")
		
		add_library(${RESOURCE_NAME}_${file_name}_OBJECT OBJECT ${OUTPUT_FILE_1})
		target_include_directories(${RESOURCE_NAME}_${file_name}_OBJECT PRIVATE ${EXTERNAL_INCLUDES})
		#add_dependencies(${RESOURCE_NAME}_${file_name}_OBJECT ${OUTPUT_FILE_1})
		set_target_properties(${RESOURCE_NAME}_${file_name}_OBJECT PROPERTIES LINKER_LANGUAGE C)
		set_target_properties(${RESOURCE_NAME}_${file_name}_OBJECT PROPERTIES COMPILE_FLAGS "-MMD -MP -x assembler-with-cpp ${ASFLAGS}")
		if(${RESOURCE_NAME} MATCHES "SPRITE")
		list(APPEND RESOURCES_HPP ${OUTPUT_FILE_2})
	elseif(${RESOURCE_NAME} MATCHES "BACKGROUND")
		list(APPEND RESOURCES_HPP ${OUTPUT_FILE_2})
	elseif(${RESOURCE_NAME} MATCHES "FONT")
		list(APPEND RESOURCES_HPP ${OUTPUT_FILE_2})
	elseif(${RESOURCE_NAME} MATCHES "SETTING")
		list(APPEND RESOURCES_HPP ${OUTPUT_FILE_1})
	elseif(${RESOURCE_NAME} MATCHES "SOUND")
		list(APPEND RESOURCES_HPP ${OUTPUT_FILE_2})
	endif()
		list(APPEND ${OBJECT_LIST_NAME} ${RESOURCE_NAME}_${file_name}_OBJECT)
	endforeach()
	
	#i.e SPRITES_EXPORT_NAMES
	set(${PLURAL}_EXPORT_NAMES ${${OBJECT_LIST_NAME}} PARENT_SCOPE)
	set(RESOURCES_HPP ${RESOURCES_HPP} PARENT_SCOPE)
endfunction()

function(build_iwram_cpp)
	
endfunction()

#if (EXISTS ${CMAKE_BINARY_DIR}/tools/tools)
	export_resources("SPRITE" "tools" "${SPR_FILES}")
	export_resources("BACKGROUND" "${SPRITES_EXPORT_NAMES}" "${BKG_FILES}")
	export_resources("FONT" "${BACKGROUNDS_EXPORT_NAMES}" "${FNT_FILES}")
	export_resources("SETTING" "${FONTS_EXPORT_NAMES}" "${AUS_FILES}")
	export_resources("SOUND" "${SETTINGS_EXPORT_NAMES}" "${SND_FILES}" "${CMAKE_BINARY_DIR}/data/audio-settings.json")

	add_custom_target(RESOURCE_OBJECTS_BUILT ALL
		DEPENDS ${SPRITES_EXPORT_NAMES} ${BACKGROUNDS_EXPORT_NAMES} ${FONTS_EXPORT_NAMES} ${SOUNDS_EXPORT_NAMES})
	#	SOURCES ${SPRITE_OBJECTS} ${BACKGROUND_OBJECTS} ${FONT_OBJECTS} ${SOUND_OBJECTS})

	foreach (FILE ${IWRAM_C_FILES})
		get_filename_component(file_dir "${FILE}" PATH)
		
		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)

		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)

		add_library(IWRAM_${file_name} OBJECT ${CMAKE_SOURCE_DIR}/source/${file_dir}/${file_name}.iwram.c)
		target_include_directories(IWRAM_${file_name} PRIVATE ${EXTERNAL_INCLUDES})
		add_dependencies(IWRAM_${file_name} RESOURCE_OBJECTS_BUILT)
		set_target_properties(IWRAM_${file_name} PROPERTIES COMPILE_FLAGS "-MMD -MP ${CFLAGS} -fno-lto -marm -mlong-calls")
		list(APPEND IWRAM_C_OBJECTS_L IWRAM_${file_name})
	endforeach()
	set(IWRAM_C_OBJECTS ${IWRAM_C_OBJECTS} ${IWRAM_C_OBJECTS_L})
	foreach (FILE ${IWRAM_CPP_FILES})
		get_filename_component(file_dir "${FILE}" PATH)
		
		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)

		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)

		add_library(IWRAM_${file_name} OBJECT ${CMAKE_SOURCE_DIR}/source/${file_dir}/${file_name}.iwram.cpp)
		target_include_directories(IWRAM_${file_name} PRIVATE ${EXTERNAL_INCLUDES})
		add_dependencies(IWRAM_${file_name} RESOURCE_OBJECTS_BUILT)
		target_include_directories(IWRAM_${file_name} PRIVATE ${CMAKE_BINARY_DIR})
		target_include_directories(IWRAM_${file_name} PRIVATE ${CMAKE_SOURCE_DIR}/source)
		set_target_properties(IWRAM_${file_name} PROPERTIES COMPILE_FLAGS "-MMD -MP ${CFLAGS} ${CPPFLAGS} -fno-lto -marm -mlong-calls")
		list(APPEND IWRAM_CPP_OBJECTS_L IWRAM_${file_name})
	endforeach()
	set(IWRAM_CPP_OBJECTS ${IWRAM_CPP_OBJECTS} ${IWRAM_CPP_OBJECTS_L})
	foreach (FILE ${GAME_C_FILES})
		get_filename_component(file_dir "${FILE}" PATH)
		
		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)
		list(GET splits -2 file_dir)

		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)

		add_library(GAME_${file_dir}_${file_name}_OBJECT OBJECT ${FILE})
		target_include_directories(GAME_${file_dir}_${file_name}_OBJECT PRIVATE ${EXTERNAL_INCLUDES})
		add_dependencies(GAME_${file_dir}_${file_name}_OBJECT RESOURCE_OBJECTS_BUILT)
		set_target_properties(GAME_${file_dir}_${file_name}_OBJECT PROPERTIES COMPILE_FLAGS "-MMD -MP ${CFLAGS}")
		list(APPEND GAME_C_OBJECTS_L GAME_${file_dir}_${file_name}_OBJECT)
	endforeach()
	set(GAME_C_OBJECTS ${GAME_C_OBJECTS} ${GAME_C_OBJECTS_L})
	#foreach (FILE ${GAME_CPP_FILES})
		#get_filename_component(file_dir "${FILE}" PATH)
		#
		#string(REPLACE "/" ";" splits ${FILE})
		#list(GET splits -1 file_new)
		#list(GET splits -2 file_dir)
		#
		#string(REPLACE "." ";." file_new_split ${file_new})
		#list(GET file_new_split 0 file_name)
		#
	#	add_library(GAME_${file_dir}_${file_name}_OBJECT OBJECT ${FILE})
	#	target_include_directories(GAME_${file_dir}_${file_name}_OBJECT PRIVATE ${EXTERNAL_INCLUDES} ${CMAKE_BINARY_DIR}/data)
	#	target_include_directories(GAME_${file_dir}_${file_name}_OBJECT PUBLIC
	#	# The location of the headers before installation 
	#	$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
	#	# The location of the headers after installation
	#	$<INSTALL_INTERFACE:/data>)
	#	target_include_directories(GAME_${file_dir}_${file_name}_OBJECT PUBLIC
	#	# The location of the headers before installation 
	#	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/source>
	#	# The location of the headers after installation
	#	$<INSTALL_INTERFACE:/source>)
	#	add_dependencies(GAME_${file_dir}_${file_name}_OBJECT RESOURCE_OBJECTS_BUILT)
	#	set_target_properties(GAME_${file_dir}_${file_name}_OBJECT PROPERTIES COMPILE_FLAGS "-MMD -MP ${CFLAGS} ${CPPFLAGS}")
	#	list(APPEND GAME_CPP_OBJECTS_L GAME_${file_dir}_${file_name}_OBJECT)
	#endforeach()
	#set(GAME_CPP_OBJECTS ${GAME_CPP_OBJECTS} ${GAME_CPP_OBJECTS_L})
	foreach (FILE ${GAME_ASM_FILES})
		get_filename_component(file_dir "${FILE}" PATH)
		
		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)

		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)

		add_library(GAME_${file_name}_OBJECT OBJECT ${CMAKE_SOURCE_DIR}/source/${file_dir}/${file_name}.s)
		target_include_directories(GAME_${file_name}_OBJECT PRIVATE ${EXTERNAL_INCLUDES})
		add_dependencies(GAME_${file_name}_OBJECT RESOURCE_OBJECTS_BUILT)
		set_target_properties(GAME_${file_name}_OBJECT PROPERTIES COMPILE_FLAGS "-MMD -MP -x assembler-with-cpp ${ASFLAGS}")
		list(APPEND GAME_ASM_OBJECTS GAME_${file_name}_OBJECT)
	endforeach()
	set(GAME_ASM_OBJECTS ${GAME_ASM_OBJECTS} ${GAME_ASM_OBJECTS_L})
		
	foreach(sprite_object ${SPRITES_EXPORT_NAMES})
		list(APPEND OBJECT_FILES $<TARGET_OBJECTS:${sprite_object}>)
	endforeach()
	foreach(background_object ${BACKGROUNDS_EXPORT_NAMES})
		list(APPEND OBJECT_FILES $<TARGET_OBJECTS:${background_object}>)
	endforeach()
	foreach(font_object ${FONTS_EXPORT_NAMES})
		list(APPEND OBJECT_FILES $<TARGET_OBJECTS:${font_object}>)
	endforeach()
	foreach(sound_object ${SOUNDS_EXPORT_NAMES})
		list(APPEND OBJECT_FILES $<TARGET_OBJECTS:${sound_object}>)
	endforeach()
	foreach(iwram_c_object ${IWRAM_C_OBJECTS})
		list(APPEND OBJECT_FILES $<TARGET_OBJECTS:${iwram_c_object}>)
	endforeach()
	foreach(iwram_cpp_object ${IWRAM_CPP_OBJECTS})
		list(APPEND OBJECT_FILES $<TARGET_OBJECTS:${iwram_cpp_object}>)
	endforeach()
	foreach(game_c_object ${GAME_C_OBJECTS})
		list(APPEND OBJECT_FILES $<TARGET_OBJECTS:${game_c_object}>)
	endforeach()
	#foreach(game_cpp_object ${GAME_CPP_OBJECTS})
	#	list(APPEND OBJECT_FILES $<TARGET_OBJECTS:${game_cpp_object}>)
	#endforeach()
	foreach(game_asm_object ${GAME_ASM_OBJECTS})
		list(APPEND OBJECT_FILES $<TARGET_OBJECTS:${game_asm_object}>)
	endforeach()
	set_source_files_properties(OBJECT_FILES 
		PROPERTIES 
		EXTERNAL_OBJECT true
		GENERATED true)
	list(FILTER OBJECT_FILES EXCLUDE REGEX "GAME_crt0_OBJECT")
	message("${OBJECT_FILES}")

	add_executable(gba-modern ${GAME_CPP_FILES} ${OBJECT_FILES})

	#target_link_libraries(gba-modern ${OBJECT_FILES})
	set_target_properties(gba-modern PROPERTIES LINKER_LANGUAGE CXX)
	#set_target_properties(gba-modern PROPERTIES DEPENDS "${OBJECT_FILES} ${RESOURCES_HPP}")
	target_compile_options(gba-modern PRIVATE -mthumb -mthumb-interwork "-Wl,--gc-sections -Wl,-Map,gba.map" "-specs=${CMAKE_SOURCE_DIR}/gba.specs" "-L ${CMAKE_SOURCE_DIR}/external/tonc/code/tonclib/lib" -nodefaultlibs -ltonc )
	target_include_directories(gba-modern PRIVATE ${EXTERNAL_INCLUDES})
	target_include_directories(gba-modern PRIVATE ${CMAKE_BINARY_DIR})
	target_include_directories(gba-modern PRIVATE ${CMAKE_BINARY_DIR}/data)
	target_include_directories(gba-modern PRIVATE ${CMAKE_BINARY_DIR}/data/sprites)
	target_include_directories(gba-modern PRIVATE ${CMAKE_BINARY_DIR}/data/backgrounds)
	target_include_directories(gba-modern PRIVATE ${CMAKE_BINARY_DIR}/data/fonts)
	target_include_directories(gba-modern PRIVATE ${CMAKE_BINARY_DIR}/data/sounds)
	target_include_directories(gba-modern PRIVATE ${CMAKE_SOURCE_DIR}/source)
	target_include_directories(gba-modern PRIVATE ${CMAKE_SOURCE_DIR}/source/audio)
	target_include_directories(gba-modern PRIVATE ${CMAKE_SOURCE_DIR}/source/data)
	target_include_directories(gba-modern PRIVATE ${CMAKE_SOURCE_DIR}/source/gameplay)
	target_include_directories(gba-modern PRIVATE ${CMAKE_SOURCE_DIR}/source/graphics)
	target_include_directories(gba-modern PRIVATE ${CMAKE_SOURCE_DIR}/source/math)
	target_include_directories(gba-modern PRIVATE ${CMAKE_SOURCE_DIR}/source/memory)
	target_include_directories(gba-modern PRIVATE ${CMAKE_SOURCE_DIR}/source/scenes)
	target_include_directories(gba-modern PRIVATE ${CMAKE_SOURCE_DIR}/source/text)
	target_include_directories(gba-modern PRIVATE ${CMAKE_SOURCE_DIR}/source/util)
	target_link_directories(gba-modern PRIVATE ${EXTERNAL_LIBS})
#endif()

#bin/game.elf: build/data/audio-settings.hpp $(OFILES)
# Get the right linker
#ifeq ($(strip $(CPPFILES)),)
#	ARMLD := $(ARMCC)
#else
#	ARMLD := $(ARMCPP)
#endif
#	@$(ARMCPP) $(LDFLAGS) -specs=gba.specs $(filter-out %crt0.o, $(OFILES)) $(LIBPATHS) -nodefaultlibs -ltonc -o $@



#add_custom_target(gba-modern ALL
#	DEPENDS ${SPRITE_OBJECT_NAMES} ${BACKGROUND_OBJECT_NAMES} ${FONT_OBJECT_NAMES} ${SOUND_OBJECT_NAMES}
#	COMMAND ${ARMCPP} -mthumb -mthumb-interwork -Wl,--gc-sections -Wl,-Map,gba.map -g -specs=${CMAKE_SOURCE_DIR}/gba.specs ${SPRITE_OBJECTS} ${BACKGROUND_OBJECTS} ${FONT_OBJECTS} ${SOUND_OBJECTS} ${IWRAM_C_OBJECTS} ${IWRAM_CPP_OBJECTS} ${GAME_C_OBJECTS} ${GAME_CPP_OBJECTS} ${GAME_ASM_OBJECTS} -L ${CMAKE_SOURCE_DIR}/external/tonc/code/tonclib/lib -nodefaultlibs -ltonc -o ${CMAKE_BINARY_DIR}/gba-modern.elf)

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