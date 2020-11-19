# Projects Settings
cmake_minimum_required (VERSION 3.16)
add_definitions("-fpermissive")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(gba-modern-tools)
find_package(Freetype REQUIRED)
find_package(LibSampleRate REQUIRED)

# CPP_FILES
set(TOOLS_CPP_FILES
	audio-export-settings.cpp
	background-convert.cpp
	background-export.cpp
	font-export.cpp
	graphics.cpp
	lodepng.cpp
	main.cpp
	resample.cpp
	rom-sanitize.cpp
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
	resample.hpp
	${LIBSAMPLERATE_INCLUDE_DIRS}
	UnionFind.hpp
	split.hpp
	util.hpp)

add_executable(tools ${TOOLS_CPP_FILES})
target_link_libraries(tools ${FREETYPE_LIBRARIES})
target_link_libraries(tools ${LIBSAMPLERATE_LIBRARY})
target_include_directories(tools PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_include_directories(tools PRIVATE ${LIBSAMPLERATE_INCLUDE_DIRS})
target_precompile_headers(tools PUBLIC pch.hpp PRIVATE ${TOOLS_PCH_HEADERS})