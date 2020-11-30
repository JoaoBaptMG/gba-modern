# TODO: - Cache Cmake variables for configuration
#		- Comment for clarity
#		- Use variables where needed
#		- Optimise build functions
#		- Skip unpacking externals if they are already unpacked

#	This is kind of messy, and just uses custom commands and targets to control the build order.

# Projects Settings
# We require at least version 3.16 if cmake because of external projects
cmake_minimum_required (VERSION 3.16)

include(ExternalProject)

project (gba-modern)

set(ARCH -mthumb -mthumb-interwork CACHE)
set(CFLAGS -g -Wall -O3 -ffunction-sections -fdata-sections -mcpu=arm7tdmi -mtune=arm7tdmi -flto ${ARCH} CACHE)
set(CPPFLAGS ${CFLAGS} -std=c++17 -fno-rtti -fno-exceptions CACHE)
set(ASFLAGS -g ${ARCH} CACHE)
set(LDFLAGS	${ARCH} -Wl,--gc-sections -Wl,-Map,gba.map CACHE)

# Dependencies
set(GCC_PLATFORM "x86_64-linux" CACHE)
set(GCC_PREFIX "10-2020q2" CACHE)
set(GCC_NAME "gcc-arm-none-eabi-10-2020-q2-preview" CACHE)
set(GCC_URL "https://developer.arm.com/-/media/Files/downloads/gnu-rm/${GCC_PREFIX}/${GCC_NAME}-${GCC_PLATFORM}.tar.bz2")
set(GCC_MD5 "2d19775c5aa091b7f55a68727c5d7d76" CACHE)
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

set(GBA_C_COMPILER ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-gcc CACHE)
set(GBA_CXX_COMPILER ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-g++ CACHE)
set(GBA_OBJCOPY ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-objcopy CACHE)
set(GBA_OBJDUMP ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-objdump CACHE)

set(TONC_URL "http://www.coranac.com/files/tonc-code.zip")
set(TONC_MD5 "3f9ebdbbe7989b33c2a788c5b5a1bfed" CACHE)
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

set(LSRC_VERSION "0.1.9" CACHE)
set(LSRC_URL "http://www.mega-nerd.com/SRC/libsamplerate-${LSRC_VERSION}.tar.gz")
set(LSRC_MD5 "2b78ae9fe63b36b9fbb6267fad93f259" CACHE)
set(LSRC_INSTALL_FOLDER ${CMAKE_SOURCE_DIR}/external/libsamplerate-install/ CACHE)
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

set(EXTERNAL_LIBS
	-L ${CMAKE_SOURCE_DIR}/external/tonc/code/tonclib/lib
	 -nodefaultlibs -ltonc)

set(EXTERNAL_INCLUDES
	-I${CMAKE_SOURCE_DIR}/external/tonc/code/tonclib/include -I${CMAKE_SOURCE_DIR}/external/gcem/include)

ExternalProject_Add(tools
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools
	BINARY_DIR ${CMAKE_BINARY_DIR}/tools
	BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/tools
	INSTALL_COMMAND cmake -E echo "Skipping install step."
	DEPENDS LibSampleRateSource)

# Resource File Definitions
file(GLOB_RECURSE SPR_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/data/sprites/ ${PROJECT_SOURCE_DIR}/data/sprites/*.png)
file(GLOB_RECURSE BKG_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/data/backgrounds/ ${PROJECT_SOURCE_DIR}/data/backgrounds/*.png)
file(GLOB_RECURSE FNT_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/data/fonts/ ${PROJECT_SOURCE_DIR}/data/fonts/*.ttf)
set(AUS_FILES
	audio.json) # Not using GLOB here since its just a single file, and for audio. There seems to be some other json files in the data directories too, but they are used for bitmask stuff?
file(GLOB_RECURSE SND_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/data/sounds/ ${PROJECT_SOURCE_DIR}/data/sounds/*.wav)

# Source files
file(GLOB_RECURSE IWRAM_C_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/source ${PROJECT_SOURCE_DIR}/source/*.iwram.c)
file(GLOB_RECURSE IWRAM_CPP_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/source ${PROJECT_SOURCE_DIR}/source/*.iwram.cpp)
file(GLOB_RECURSE GAME_C_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/source ${PROJECT_SOURCE_DIR}/source/*.c)
list(FILTER GAME_C_FILES EXCLUDE REGEX ".*iwram.c$")
file(GLOB_RECURSE GAME_CPP_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/source ${PROJECT_SOURCE_DIR}/source/*.cpp)
list(FILTER GAME_CPP_FILES EXCLUDE REGEX ".*iwram.cpp$")
file(GLOB_RECURSE GAME_ASM_FILES
	CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}/source ${PROJECT_SOURCE_DIR}/source/*.s)

# Build Functions
function(build_resource)
	# Set Variables to make this easier
	# i.e SPRITE
	set(RESOURCE_NAME ${ARGV0})
	# i.e SPRITES
	set(PLURAL ${RESOURCE_NAME}S)
	# passed in dependencies, tool or the previous object
	set(TARGET_DEPENDENCIES ${ARGV1})
	# Set helped variables based on what resource we are exporting and building
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
		string(REPLACE "." ";." file ${FILE})
		list(GET file 0 file_name)
		list(GET file 1 file_type)

		set(OUTPUT_FILE_1 ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${file_name}${OUT1})
		set(OUTPUT_FILE_2 ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${file_name}${OUT2})

		# This command generates the files from the source/data files
		# OUTPUT means the files are set as Generated in cmake,
		# so when they are used as a source, a dependency is set on their generation
		# We pass in the dependencies to DEPENDS, the first call of this function passes in the Tools project as a dependency,
		# so that all subsequent commands and targets are implicitely dependent on Tools having finished
		# WORKING_DIRECTORY is set to the project root (${CMAKE_SOURCE_DIR}) so a relative path to the file that is to be exported
		# is sent to the tool, for proper generation of namespaces
		add_custom_command(OUTPUT ${OUTPUT_FILE_1} ${OUTPUT_FILE_2}
			DEPENDS ${TARGET_DEPENDENCIES}
			COMMAND ${CMAKE_BINARY_DIR}/tools/tools ${TOOL_NAME}
				./data/${FOLDER_NAME}${file_name}${file_type}
				${OUTPUT_FILE_1} ${OUTPUT_FILE_2} ${IN2}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			COMMENT "Exporting ${RESOURCE_NAME} ${file_name}")

		# We don't want to generate and object and dependency? file for audio.json, since it only outputs a header and another json file
		if(NOT ${RESOURCE_NAME} MATCHES "SETTING")
			file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}/${file_name})

			set(OUTPUT_OBJECT ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${file_name}/${file_name}.o)
			set(OUTPUT_OBJECT_D ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${file_name}/${file_name}.d)

			# This command generates the object file and dependency? of the exported ASM and header
			# OUTPUT again means these files are set as GENERATED
			# MAIN_DEPENDENCY sets the input ASM file as the main dependency
			# DEPENDS sets the header file as a dependency
			# We need too set dependencies to the input files used in the command, as it is not automatically done even though they are source files.
			# This ensures this command is ran after generating the ASM and header files
			add_custom_command(OUTPUT ${OUTPUT_OBJECT} ${OUTPUT_OBJECT_D}
				MAIN_DEPENDENCY ${OUTPUT_FILE_1}
				DEPENDS ${OUTPUT_FILE_2}
				COMMAND ${GBA_C_COMPILER} -MMD -MP -MF ${OUTPUT_OBJECT_D} -x assembler-with-cpp ${ASFLAGS} -c ${OUTPUT_FILE_1} -o ${OUTPUT_OBJECT}
				COMMENT "BUILDING ${OUTPUT_OBJECT} AND ${OUTPUT_OBJECT_D}")

			# Since we pass in dependencies, we can handle all resource objects as a group
			# We append to a lists that we later set to the parent scope to use when building
			list(APPEND RESOURCE_OBJECT_FILES ${OUTPUT_OBJECT})
			list(APPEND RESOURCE_OBJECT_D_FILES ${OUTPUT_OBJECT_D})
		endif()

		# The second output file is usually the header file,
		# unless the settings are being made, then it is the first.
		if(NOT ${RESOURCE_NAME} MATCHES "SETTING")
			list(APPEND RESOURCES_HPP ${OUTPUT_FILE_2})
		else()
			list(APPEND RESOURCES_HPP ${OUTPUT_FILE_1})
		endif()
	endforeach()

	# Set our object, dependency? and header lists to the parent scope
	set(RESOURCE_OBJECT_FILES ${RESOURCE_OBJECT_FILES} PARENT_SCOPE)
	set(RESOURCE_OBJECT_D_FILES ${RESOURCE_OBJECT_D_FILES} PARENT_SCOPE)
	set(RESOURCES_HPP ${RESOURCES_HPP} PARENT_SCOPE)
endfunction()

function(build_iwram_c)
	foreach (FILE ${IWRAM_C_FILES})
		get_filename_component(file_dir "${CMAKE_BINARY_DIR}/source/${FILE}" PATH)

		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)

		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)

		file(MAKE_DIRECTORY ${file_dir}/${file_name})

		set(OUTPUT_OBJECT ${file_dir}/${file_name}/${file_name}.iwram.o)
		set(OUTPUT_OBJECT_D ${file_dir}/${file_name}/${file_name}.iwram.d)

		add_custom_command(OUTPUT ${OUTPUT_OBJECT} ${OUTPUT_OBJECT_D}
			DEPENDS RESOURCE_OBJECTS_BUILT
			COMMAND ${GBA_C_COMPILER} -MMD -MP -MF ${OUTPUT_OBJECT_D} ${CFLAGS} ${EXTERNAL_INCLUDES} -I${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}/source -fno-lto -marm -mlong-calls -c ${PROJECT_SOURCE_DIR}/source/${FILE} -o ${OUTPUT_OBJECT}
			COMMENT "BUILDING ${OUTPUT_OBJECT} AND ${OUTPUT_OBJECT_D}")

		list(APPEND IWRAM_C_OBJECT_FILES ${OUTPUT_OBJECT})
		list(APPEND IWRAM_C_OBJECT_D_FILES ${OUTPUT_OBJECT_D})
	endforeach()
	set(IWRAM_C_OBJECT_FILES ${IWRAM_C_OBJECT_FILES} PARENT_SCOPE)
	set(IWRAM_C_OBJECT_D_FILES ${IWRAM_C_OBJECT_D_FILES} PARENT_SCOPE)
endfunction()

function(build_iwram_cpp)
	foreach (FILE ${IWRAM_CPP_FILES})
		get_filename_component(file_dir "${CMAKE_BINARY_DIR}/source/${FILE}" PATH)

		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)

		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)

		file(MAKE_DIRECTORY ${file_dir}/${file_name})

		set(OUTPUT_OBJECT ${file_dir}/${file_name}/${file_name}.iwram.o)
		set(OUTPUT_OBJECT_D ${file_dir}/${file_name}/${file_name}.iwram.d)

		add_custom_command(OUTPUT ${OUTPUT_OBJECT} ${OUTPUT_OBJECT_D}
			DEPENDS RESOURCE_OBJECTS_BUILT
			COMMAND ${GBA_CXX_COMPILER} -MMD -MP -MF ${OUTPUT_OBJECT_D} ${CPPFLAGS} ${EXTERNAL_INCLUDES} -I${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}/source -fno-lto -marm -mlong-calls -c ${PROJECT_SOURCE_DIR}/source/${FILE} -o ${OUTPUT_OBJECT}
			COMMENT "BUILDING ${OUTPUT_OBJECT} AND ${OUTPUT_OBJECT_D}")

		list(APPEND IWRAM_CPP_OBJECT_FILES ${OUTPUT_OBJECT})
		list(APPEND IWRAM_CPP_OBJECT_D_FILES ${OUTPUT_OBJECT_D})
	endforeach()
	set(IWRAM_CPP_OBJECT_FILES ${IWRAM_CPP_OBJECT_FILES} PARENT_SCOPE)
	set(IWRAM_CPP_OBJECT_D_FILES ${IWRAM_CPP_OBJECT_D_FILES} PARENT_SCOPE)
endfunction()

function(build_game_c)
	foreach (FILE ${GAME_C_FILES})
		get_filename_component(file_dir "${CMAKE_BINARY_DIR}/source/${FILE}" PATH)

		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)

		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)

		file(MAKE_DIRECTORY ${file_dir}/${file_name})

		set(OUTPUT_OBJECT ${file_dir}/${file_name}/${file_name}.o)
		set(OUTPUT_OBJECT_D ${file_dir}/${file_name}/${file_name}.d)

		add_custom_command(OUTPUT ${OUTPUT_OBJECT} ${OUTPUT_OBJECT_D}
			DEPENDS RESOURCE_OBJECTS_BUILT
			COMMAND ${GBA_C_COMPILER} -MMD -MP -MF ${OUTPUT_OBJECT_D} ${CFLAGS} ${EXTERNAL_INCLUDES} -I${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}/source -c ${PROJECT_SOURCE_DIR}/source/${FILE} -o ${OUTPUT_OBJECT}
			COMMENT "BUILDING ${OUTPUT_OBJECT} AND ${OUTPUT_OBJECT_D}")

		list(APPEND GAME_C_OBJECT_FILES ${OUTPUT_OBJECT})
		list(APPEND GAME_C_OBJECT_D_FILES ${OUTPUT_OBJECT_D})
	endforeach()
	set(GAME_C_OBJECT_FILES ${GAME_C_OBJECT_FILES} PARENT_SCOPE)
	set(GAME_C_OBJECT_D_FILES ${GAME_C_OBJECT_D_FILES} PARENT_SCOPE)
endfunction()

function(build_game_cpp)
	foreach (FILE ${GAME_CPP_FILES})
		get_filename_component(file_dir "${CMAKE_BINARY_DIR}/source/${FILE}" PATH)

		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)

		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)

		file(MAKE_DIRECTORY ${file_dir}/${file_name})

		set(OUTPUT_OBJECT ${file_dir}/${file_name}/${file_name}.o)
		set(OUTPUT_OBJECT_D ${file_dir}/${file_name}/${file_name}.d)

		add_custom_command(OUTPUT ${OUTPUT_OBJECT} ${OUTPUT_OBJECT_D}
			DEPENDS RESOURCE_OBJECTS_BUILT
			COMMAND ${GBA_CXX_COMPILER} -MMD -MP -MF ${OUTPUT_OBJECT_D} ${CPPFLAGS} ${EXTERNAL_INCLUDES} -I${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}/source -c ${PROJECT_SOURCE_DIR}/source/${FILE} -o ${OUTPUT_OBJECT}
			COMMENT "BUILDING ${OUTPUT_OBJECT} AND ${OUTPUT_OBJECT_D}")

		list(APPEND GAME_CPP_OBJECT_FILES ${OUTPUT_OBJECT})
		list(APPEND GAME_CPP_OBJECT_D_FILES ${OUTPUT_OBJECT_D})
	endforeach()
	set(GAME_CPP_OBJECT_FILES ${GAME_CPP_OBJECT_FILES} PARENT_SCOPE)
	set(GAME_CPP_OBJECT_D_FILES ${GAME_CPP_OBJECT_D_FILES} PARENT_SCOPE)
endfunction()

function(build_game_asm)
	foreach (FILE ${GAME_ASM_FILES})
		get_filename_component(file_dir "${CMAKE_BINARY_DIR}/source/${FILE}" PATH)

		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)

		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)

		file(MAKE_DIRECTORY ${file_dir}/${file_name})

		set(OUTPUT_OBJECT ${file_dir}/${file_name}/${file_name}.o)
		set(OUTPUT_OBJECT_D ${file_dir}/${file_name}/${file_name}.d)

		add_custom_command(OUTPUT ${OUTPUT_OBJECT} ${OUTPUT_OBJECT_D}
			DEPENDS RESOURCE_OBJECTS_BUILT
			COMMAND ${GBA_C_COMPILER} -MMD -MP -MF ${OUTPUT_OBJECT_D} -x assembler-with-cpp ${ASFLAGS} -I${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}/source -c ${PROJECT_SOURCE_DIR}/source/${FILE} -o ${OUTPUT_OBJECT}
			COMMENT "BUILDING ${OUTPUT_OBJECT} AND ${OUTPUT_OBJECT_D}")

		list(APPEND GAME_ASM_OBJECT_FILES ${OUTPUT_OBJECT})
		list(APPEND GAME_ASM_OBJECT_D_FILES ${OUTPUT_OBJECT_D})
	endforeach()
	set(GAME_ASM_OBJECT_FILES ${GAME_ASM_OBJECT_FILES} PARENT_SCOPE)
	set(GAME_ASM_OBJECT_D_FILES ${GAME_ASM_OBJECT_D_FILES} PARENT_SCOPE)
endfunction()

build_resource("SPRITE" "tools" "${SPR_FILES}")
build_resource("BACKGROUND" "${RESOURCES_HPP}" "${BKG_FILES}")
build_resource("FONT" "${RESOURCES_HPP}" "${FNT_FILES}")
build_resource("SETTING" "${RESOURCES_HPP}" "${AUS_FILES}")
build_resource("SOUND" "${RESOURCES_HPP}" "${SND_FILES}" "${CMAKE_BINARY_DIR}/data/audio-settings.json")

add_custom_target(RESOURCE_OBJECTS_BUILT ALL
	DEPENDS ${RESOURCE_OBJECT_FILES}
	COMMENT "${RESOURCE_OBJECT_FILES}")

build_iwram_c()
build_iwram_cpp()
build_game_c()
build_game_cpp()
build_game_asm()

add_custom_target(GAME_OBJECTS_BUILT ALL
	DEPENDS ${IWRAM_C_OBJECT_FILES} ${IWRAM_CPP_OBJECT_FILES} ${GAME_C_OBJECT_FILES} ${GAME_CPP_OBJECT_FILES} ${GAME_ASM_OBJECT_FILES})

set(BUILD_GAME_FILES ${RESOURCE_OBJECT_FILES} ${IWRAM_C_OBJECT_FILES} ${IWRAM_CPP_OBJECT_FILES} ${GAME_C_OBJECT_FILES} ${GAME_CPP_OBJECT_FILES} ${GAME_ASM_OBJECT_FILES})
list(FILTER BUILD_GAME_FILES EXCLUDE REGEX "crt0")

add_custom_target(elf ALL
	DEPENDS GAME_OBJECTS_BUILT
	COMMAND ${GBA_CXX_COMPILER} ${LDFLAGS} -specs=${CMAKE_SOURCE_DIR}/gba.specs ${BUILD_GAME_FILES} ${EXTERNAL_LIBS} -o ${CMAKE_BINARY_DIR}/gba-modern.elf
	BYPRODUCTS ${CMAKE_BINARY_DIR}/gba-modern.elf ${CMAKE_SOURCE_DIR}/gba.map
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(elf_dump ALL
	DEPENDS elf
	COMMAND ${GBA_OBJDUMP} -dCS ${CMAKE_BINARY_DIR}/gba-modern.elf > ${CMAKE_BINARY_DIR}/gba-modern.elf.dump
	BYPRODUCTS ${CMAKE_BINARY_DIR}/gba-modern.elf.dump)

add_custom_target(create_rom ALL
	DEPENDS elf_dump
	COMMAND ${GBA_OBJCOPY} -O binary ${CMAKE_BINARY_DIR}/gba-modern.elf ${CMAKE_BINARY_DIR}/gba-modern.gba)

add_custom_target(sanitize_rom ALL
	DEPENDS create_rom
	COMMAND ${CMAKE_BINARY_DIR}/tools/tools rom-sanitize ${CMAKE_BINARY_DIR}/gba-modern.gba ${CMAKE_SOURCE_DIR}/gba.json ${CMAKE_BINARY_DIR}/gba-modern.gba
	BYPRODUCTS ${CMAKE_BINARY_DIR}/gba-modern.gba)
