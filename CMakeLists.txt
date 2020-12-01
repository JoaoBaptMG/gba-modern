# TODO: - Optimise build functions
#		- Skip unpacking externals if they are already unpacked

#	This is kind of messy, and just uses custom commands and targets to control the build order.
#	I think this could be better implemented through Object Libraries, and using the toolchain functionality of cmake
#	and building an executable (as an elf) then using a custom target and POST BUILD command to do the final object dump and rom sanitize.

# Projects Settings
# We require at least version 3.16 if cmake because we are using ExternalProject_Add
cmake_minimum_required (VERSION 3.16)

include(ExternalProject)

set(ROM_NAME "gba-modern" CACHE STRING "Project/ROM Name")

project (${ROM_NAME})

set(ARCH "-mthumb;-mthumb-interwork" CACHE STRING "ARCH Flags")
set(CFLAGS "-g;-Wall;-O3;-ffunction-sections;-fdata-sections;-mcpu=arm7tdmi;-mtune=arm7tdmi;-flto;${ARCH}" CACHE STRING "C Flags")
set(CPPFLAGS "${CFLAGS};-std=c++17;-fno-rtti;-fno-exceptions" CACHE STRING "CPP Flags")
set(ASFLAGS "-g;${ARCH}" CACHE STRING "Assembly Flags")
set(LDFLAGS	"${ARCH};-Wl,--gc-sections;-Wl,-Map,gba.map" CACHE STRING "Linker Flags")

set(GBA_C_COMPILER ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-gcc CACHE PATH "GCC C Compiler Path")
set(GBA_CXX_COMPILER ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-g++ CACHE PATH "GCC CPP Compiler Path")
set(GBA_OBJCOPY ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-objcopy CACHE PATH "GCC Object Copy Path")
set(GBA_OBJDUMP ${CMAKE_SOURCE_DIR}/external/gcc/bin/arm-none-eabi-objdump CACHE PATH "GCC Object Dump Path")

# Dependencies
set(GCC_PLATFORM "x86_64-linux" CACHE STRING "GCC Platform Name")
set(GCC_PREFIX "10-2020q2" CACHE STRING "GCC Prefix")
set(GCC_NAME "gcc-arm-none-eabi-10-2020-q2-preview" CACHE STRING "GCC Name")
set(GCC_URL "https://developer.arm.com/-/media/Files/downloads/gnu-rm/${GCC_PREFIX}/${GCC_NAME}-${GCC_PLATFORM}.tar.bz2")
set(GCC_MD5 "2d19775c5aa091b7f55a68727c5d7d76" CACHE STRING "GCC MD5 Hash")
set(GCC_FOLDER ${CMAKE_SOURCE_DIR}/external/gcc CACHE PATH "GCC Path")
file(MAKE_DIRECTORY ${GCC_FOLDER})

set(TONC_URL "http://www.coranac.com/files/tonc-code.zip")
set(TONC_MD5 "3f9ebdbbe7989b33c2a788c5b5a1bfed" CACHE STRING "TONC MD5 Hash")
set(TONC_FOLDER ${CMAKE_SOURCE_DIR}/external/tonc CACHE PATH "TONC Path")
file(MAKE_DIRECTORY ${TONC_FOLDER})

set(LSRC_VERSION "0.1.9" CACHE STRING "LibSampleRate Version")
set(LSRC_URL "http://www.mega-nerd.com/SRC/libsamplerate-${LSRC_VERSION}.tar.gz")
set(LSRC_MD5 "2b78ae9fe63b36b9fbb6267fad93f259" CACHE STRING "LibSampleRate MD5 Hash")
set(LSRC_INSTALL_FOLDER ${CMAKE_SOURCE_DIR}/external/libsamplerate-install CACHE PATH "LibSampleRate Install Path")
set(LSRC_SOURCE_FOLDER ${CMAKE_SOURCE_DIR}/external/libsamplerate CACHE PATH "LibSampleRate Source Path")
file(MAKE_DIRECTORY ${LSRC_SOURCE_FOLDER})

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
function(build_resource RESOURCE_NAME TARGET_DEPENDENCIES RESOURCE_FILES)
	# Set helper variables based on what resource we are exporting and building
	if(${RESOURCE_NAME} MATCHES "SPRITE")
		set(TOOL_NAME sprite-export)
		set(FOLDER_NAME sprites/)
		set(OUT .s .hpp)
	elseif(${RESOURCE_NAME} MATCHES "BACKGROUND")
		set(TOOL_NAME background-export)
		set(FOLDER_NAME backgrounds/)
		set(OUT .s .hpp)
	elseif(${RESOURCE_NAME} MATCHES "FONT")
		set(TOOL_NAME font-export)
		set(FOLDER_NAME fonts/)
		set(OUT .s .hpp)
	elseif(${RESOURCE_NAME} MATCHES "SETTING")
		set(TOOL_NAME audio-export-settings)
		set(FOLDER_NAME "")
		set(OUT -settings.hpp -settings.json)
	elseif(${RESOURCE_NAME} MATCHES "SOUND")
		set(TOOL_NAME sound-export)
		set(FOLDER_NAME sounds/)
		set(OUT .s .hpp)
	endif()

	#make a directory in build folder for the output files
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME})	

	foreach(FILE ${RESOURCE_FILES})
		get_filename_component(file_dir "${FILE}" PATH)
		string(REPLACE "." ";." file ${FILE})
		list(GET file 0 file_name)
		list(GET file 1 file_type)
		set(OUTPUT_FILES "")
		foreach(OUTPUT ${OUT})
			if(${OUTPUT} MATCHES ".hpp")
				list(APPEND RESOURCES_HPP ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${file_name}${OUTPUT})
			elseif(${OUTPUT} MATCHES ".s")
				set(RESOURCE_INPUT_FILE ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${file_name}${OUTPUT})
			endif()
			list(APPEND OUTPUT_FILES ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${file_name}${OUTPUT})
		endforeach()

		# This command generates the files from the source/data files
		# OUTPUT means the files are set as Generated in cmake,
		# so when they are used as a source, a dependency is set on their generation
		# We pass in the dependencies to DEPENDS, the first call of this function passes in the Tools project as a dependency,
		# so that all subsequent commands and targets are implicitely dependent on Tools having finished
		# WORKING_DIRECTORY is set to the project root (${CMAKE_SOURCE_DIR}) so a relative path to the file that is to be exported
		# is sent to the tool, for proper generation of namespaces
		add_custom_command(OUTPUT ${OUTPUT_FILES}
			DEPENDS ${TARGET_DEPENDENCIES}
			COMMAND ${CMAKE_BINARY_DIR}/tools/tools ${TOOL_NAME}
				./data/${FOLDER_NAME}${file_name}${file_type}
				${OUTPUT_FILES} ${ARGN}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			COMMENT "Exporting ${RESOURCE_NAME} ${file_name}")

		# We don't want to generate and object file for audio.json, since it only outputs a header and another json file
		if(NOT ${RESOURCE_NAME} MATCHES "SETTING")
			file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}/${file_name})

			set(OUTPUT_OBJECT ${CMAKE_BINARY_DIR}/data/${FOLDER_NAME}${file_name}/${file_name}.o)

			# This command generates the object file of the exported ASM and header
			# OUTPUT again means these files are set as GENERATED
			# MAIN_DEPENDENCY sets the input ASM file as the main dependency
			# DEPENDS sets the header file as a dependency
			# We need too set dependencies to the input files used in the command, as it is not automatically done even though they are source files.
			# This ensures this command is ran after generating the ASM and header files
			add_custom_command(OUTPUT ${OUTPUT_OBJECT}
				DEPENDS ${OUTPUT_FILES}
				COMMAND ${GBA_C_COMPILER} -x assembler-with-cpp ${ASFLAGS} -c ${RESOURCE_INPUT_FILE} -o ${OUTPUT_OBJECT}
				COMMENT "BUILDING ${OUTPUT_OBJECT}")

			# Since we pass in dependencies, we can handle all resource objects as a group
			# We append to a lists that we later set to the parent scope to use when building
			list(APPEND RESOURCE_OBJECT_FILES ${OUTPUT_OBJECT})
		endif()

		# The second output file is usually the header file,
		# unless the settings are being made, then it is the first.
		if(NOT ${RESOURCE_NAME} MATCHES "SETTING")
			list(APPEND RESOURCES_HPP ${OUTPUT_FILE_2})
		else()
			list(APPEND RESOURCES_HPP ${OUTPUT_FILE_1})
		endif()
	endforeach()

	# Set our object and header lists to the parent scope
	set(RESOURCE_OBJECT_FILES ${RESOURCE_OBJECT_FILES} PARENT_SCOPE)
	set(RESOURCES_HPP ${RESOURCES_HPP} PARENT_SCOPE)
endfunction()

function(build_game_objects GAME_SOURCE_FILES SOURCE_TYPE)
	foreach (FILE ${GAME_SOURCE_FILES})
		get_filename_component(file_dir "${CMAKE_BINARY_DIR}/source/${FILE}" PATH)
		string(REPLACE "/" ";" splits ${FILE})
		list(GET splits -1 file_new)
		string(REPLACE "." ";." file_new_split ${file_new})
		list(GET file_new_split 0 file_name)
		list(GET file_new_split 1 file_type)
		file(MAKE_DIRECTORY ${file_dir}/${file_name})
		if(file_type MATCHES ".iwram")
			list(GET file_new_split 2 file_type2)
			set(OUTPUT_OBJECT ${file_dir}/${file_name}/${file_name}.iwram.o)
		else()
			set(OUTPUT_OBJECT ${file_dir}/${file_name}/${file_name}.o)
		endif()
		
		# Can't have the Flags be cached and used in  the command,  or even setting the command beforehand, so we have to do seperate targets instead of just swapping out the command. 
		if(file_type MATCHES ".s")
			add_custom_command(OUTPUT ${OUTPUT_OBJECT}
				DEPENDS RESOURCE_OBJECTS_BUILT
				COMMAND ${GBA_C_COMPILER} -x assembler-with-cpp ${ASFLAGS} -I${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}/source -c ${PROJECT_SOURCE_DIR}/source/${FILE} -o ${OUTPUT_OBJECT}
				COMMENT "BUILDING ${OUTPUT_OBJECT}")
		elseif(file_type MATCHES ".cpp")
			add_custom_command(OUTPUT ${OUTPUT_OBJECT}
				DEPENDS RESOURCE_OBJECTS_BUILT
				COMMAND ${GBA_CXX_COMPILER} ${CPPFLAGS} ${EXTERNAL_INCLUDES} -I${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}/source -c ${PROJECT_SOURCE_DIR}/source/${FILE} -o ${OUTPUT_OBJECT}
				COMMENT "BUILDING ${OUTPUT_OBJECT}")
		elseif(file_type MATCHES ".c")
			add_custom_command(OUTPUT ${OUTPUT_OBJECT}
				DEPENDS RESOURCE_OBJECTS_BUILT
				COMMAND ${GBA_C_COMPILER} ${CFLAGS} ${EXTERNAL_INCLUDES} -I${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}/source -c ${PROJECT_SOURCE_DIR}/source/${FILE} -o ${OUTPUT_OBJECT}
				COMMENT "BUILDING ${OUTPUT_OBJECT}")
		elseif(file_type MATCHES ".iwram")
			if(file_type2 MATCHES ".cpp")
				add_custom_command(OUTPUT ${OUTPUT_OBJECT}
					DEPENDS RESOURCE_OBJECTS_BUILT
					COMMAND ${GBA_CXX_COMPILER} ${CPPFLAGS} ${EXTERNAL_INCLUDES} -I${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}/source -fno-lto -marm -mlong-calls -c ${PROJECT_SOURCE_DIR}/source/${FILE} -o ${OUTPUT_OBJECT}
					COMMENT "BUILDING ${OUTPUT_OBJECT}")
			elseif(file_type2 MATCHES ".c")
				add_custom_command(OUTPUT ${OUTPUT_OBJECT}
					DEPENDS RESOURCE_OBJECTS_BUILT
					COMMAND ${GBA_C_COMPILER} ${CFLAGS} ${EXTERNAL_INCLUDES} -I${CMAKE_BINARY_DIR} -I${CMAKE_SOURCE_DIR}/source -fno-lto -marm -mlong-calls -c ${PROJECT_SOURCE_DIR}/source/${FILE} -o ${OUTPUT_OBJECT}
					COMMENT "BUILDING ${OUTPUT_OBJECT}")
			endif()
		endif()
		# Add the output files to their own list
		list(APPEND GAME_${SOURCE_TYPE}_OBJECT_FILES ${OUTPUT_OBJECT})
	endforeach()
	# Set the lists to the parent scope
	set(GAME_${SOURCE_TYPE}_OBJECT_FILES ${GAME_${SOURCE_TYPE}_OBJECT_FILES} PARENT_SCOPE)
endfunction()

# Targets
ExternalProject_Add(gbaGCC
	URL ${GCC_URL}
	URL_HASH MD5=${GCC_MD5}
	DOWNLOAD_NO_EXTRACT TRUE
	DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/external
	UPDATE_COMMAND tar xjf ${CMAKE_SOURCE_DIR}/external/${GCC_NAME}-${GCC_PLATFORM}.tar.bz2 -C ${GCC_FOLDER} --strip-components 1
	CONFIGURE_COMMAND cmake -E echo "Skipping configure step."
	BUILD_COMMAND cmake -E echo "Skipping BUILD step."
	INSTALL_COMMAND cmake -E echo "Skipping install step.")

ExternalProject_Add(Tonc
	URL ${TONC_URL}
	URL_HASH MD5=${TONC_MD5}
	DOWNLOAD_NO_EXTRACT TRUE
	DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/external
	UPDATE_COMMAND unzip -qq -n ${TONC_FOLDER}-code.zip -d ${TONC_FOLDER}
	CONFIGURE_COMMAND cmake -E echo "Skipping configure step."
	BUILD_COMMAND cmake -E echo "Skipping BUILD step."
	INSTALL_COMMAND cmake -E echo "Skipping install step."
	DEPENDS gbaGCC)


ExternalProject_Add(LibSampleRateSource
	URL ${LSRC_URL}
	URL_HASH MD5=${LSRC_MD5}
	DOWNLOAD_NO_EXTRACT TRUE
	DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/external
	UPDATE_COMMAND tar xzf ${LSRC_SOURCE_FOLDER}-${LSRC_VERSION}.tar.gz -C ${LSRC_SOURCE_FOLDER} --strip-components 1
	CONFIGURE_COMMAND ${LSRC_SOURCE_FOLDER}/configure --prefix ${LSRC_INSTALL_FOLDER}
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

# Export and build the resources
build_resource("SPRITE" "tools" "${SPR_FILES}")
build_resource("BACKGROUND" "${RESOURCES_HPP}" "${BKG_FILES}")
build_resource("FONT" "${RESOURCES_HPP}" "${FNT_FILES}")
build_resource("SETTING" "${RESOURCES_HPP}" "${AUS_FILES}")
build_resource("SOUND" "${RESOURCES_HPP}" "${SND_FILES}" "${CMAKE_BINARY_DIR}/data/audio-settings.json")

# Dependency target to ensure resources are exported and built
add_custom_target(RESOURCE_OBJECTS_BUILT ALL
	DEPENDS ${RESOURCE_OBJECT_FILES}
	COMMENT "${RESOURCE_OBJECT_FILES}")
	
# Build the game files
build_game_objects("${IWRAM_C_FILES}" "IWRAM_C")
build_game_objects("${IWRAM_CPP_FILES}" "IWRAM_CPP")
build_game_objects("${GAME_CPP_FILES}" "CPP")
build_game_objects("${GAME_C_FILES}" "C")
build_game_objects("${GAME_ASM_FILES}" "ASM")

# Dependency target to ensure all the game objects are built, it implicitly depends on the resourccces being exported and built first
add_custom_target(GAME_OBJECTS_BUILT ALL
	DEPENDS ${GAME_IWRAM_C_OBJECT_FILES} ${GAME_IWRAM_CPP_OBJECT_FILES} ${GAME_C_OBJECT_FILES} ${GAME_CPP_OBJECT_FILES} ${GAME_ASM_OBJECT_FILES})

# We need to build crt0, but not use it when building the elf, so we filter it out of all the files
set(BUILD_GAME_FILES ${RESOURCE_OBJECT_FILES} ${GAME_IWRAM_C_OBJECT_FILES} ${GAME_IWRAM_CPP_OBJECT_FILES} ${GAME_C_OBJECT_FILES} ${GAME_CPP_OBJECT_FILES} ${GAME_ASM_OBJECT_FILES})
list(FILTER BUILD_GAME_FILES EXCLUDE REGEX "crt0")

# Build the elf
add_custom_target(elf ALL
	DEPENDS GAME_OBJECTS_BUILT
	COMMAND ${GBA_CXX_COMPILER} ${LDFLAGS} -specs=${CMAKE_SOURCE_DIR}/gba.specs ${BUILD_GAME_FILES} ${EXTERNAL_LIBS} -o ${CMAKE_BINARY_DIR}/${ROM_NAME}.elf
	BYPRODUCTS ${CMAKE_BINARY_DIR}/${ROM_NAME}.elf ${CMAKE_SOURCE_DIR}/gba.map
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# Dump the elf
add_custom_target(elf_dump ALL
	DEPENDS elf
	COMMAND ${GBA_OBJDUMP} -dCS ${CMAKE_BINARY_DIR}/${ROM_NAME}.elf > ${CMAKE_BINARY_DIR}/${ROM_NAME}.elf.dump
	BYPRODUCTS ${CMAKE_BINARY_DIR}/${ROM_NAME}.elf.dump)

# Create the ROM file
add_custom_target(create_rom ALL
	DEPENDS elf_dump
	COMMAND ${GBA_OBJCOPY} -O binary ${CMAKE_BINARY_DIR}/${ROM_NAME}.elf ${CMAKE_BINARY_DIR}/${ROM_NAME}.gba)

# Sanitize the ROM file
add_custom_target(sanitize_rom ALL
	DEPENDS create_rom
	COMMAND ${CMAKE_BINARY_DIR}/tools/tools rom-sanitize ${CMAKE_BINARY_DIR}/${ROM_NAME}.gba ${CMAKE_SOURCE_DIR}/gba.json ${CMAKE_BINARY_DIR}/${ROM_NAME}.gba
	BYPRODUCTS ${CMAKE_BINARY_DIR}/${ROM_NAME}.gba)
