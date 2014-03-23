cmake_minimum_required (VERSION 2.6)
project (ircbot)

if(DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the build type.")
else()
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type.")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Werror -fno-exceptions -fno-rtti --std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -fno-strict-aliasing")

add_definitions(-DOS_LINUX)

# Set CPU
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
	add_definitions(-DARCH_X86)
endif()

include_directories(${PROJECT_ROOT_DIR})
add_executable(ircbot main.cc bot.cc strings.cc)
