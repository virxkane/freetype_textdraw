project(freetype-textdraw)

cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Find the Qt5 libraries
find_package(Qt5Core 5.14 REQUIRED)
find_package(Qt5Gui 5.14 REQUIRED)
find_package(Qt5Widgets 5.14 REQUIRED)

option(USE_HARFBUZZ "Use HarfBuzz for text shaping" ON)
option(BUILD_TESTS "Build test programs" OFF)

find_package(Freetype REQUIRED)
if (USE_HARFBUZZ)
	find_package(HarfBuzz REQUIRED)
	add_definitions(-DUSE_HARFBUZZ=1)
endif(USE_HARFBUZZ)

include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${HARFBUZZ_INCLUDE_DIR})

include_directories(${CMAKE_BINARY_DIR})

# add defines for debug build type
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

if (WIN32)
	set(CMAKE_RC_COMPILER windres)
	# set rc syntax
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -O coff -o <OBJECT> -i <SOURCE>")
	set(CMAKE_RC_SOURCE_FILE_EXTENSIONS rc)
	set(CMAKE_RC_FLAGS "-I${CMAKE_BINARY_DIR}")
	# enable resource language for this project
	enable_language(RC)
endif(WIN32)

add_subdirectory(src)
add_subdirectory(fc-lang_conv)
add_subdirectory(locale_data)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)
