# Projects Settings
cmake_minimum_required (VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gba-modern-tools)

find_package(Freetype REQUIRED)

get_filename_component(PARENT_DIRECTORY ../ ABSOLUTE)
set(LIBSAMPLERATE_INCLUDE_DIR ${PARENT_DIRECTORY}/external/libsamplerate-install)
set(LIBSAMPLERATE_LIBRARY ${PARENT_DIRECTORY}/external/libsamplerate-install/lib/libsamplerate.so)

# CPP_FILES
set(TOOLS_CPP_FILES
	audio-export-settings.cpp
	background-convert.cpp
	background-export.cpp
	font-export.cpp
	graphics.cpp
	lodepng.cpp
	main.cpp
	rom-sanitize.cpp
	sound-data.cpp
	sound-export.cpp
	sprite-export.cpp
	UnionFind.cpp
	util.cpp
	main.cpp)

set(TOOLS_PCH_HEADERS
	background-export-state.hpp
	grid.hpp
	lodepng.h
	nlohmann/json.hpp
	graphics.hpp
	UnionFind.hpp
	sound-data.hpp
	split.hpp
	util.hpp)

add_executable(tools ${TOOLS_CPP_FILES})
target_link_libraries(tools ${FREETYPE_LIBRARIES})
target_link_libraries(tools ${LIBSAMPLERATE_LIBRARY})
target_include_directories(tools PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_precompile_headers(tools PUBLIC pch.hpp PRIVATE ${TOOLS_PCH_HEADERS})