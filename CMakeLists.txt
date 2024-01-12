# https://cmake.org/licensing/

cmake_minimum_required(VERSION 3.27)
project(fix_win_num_popup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB SOURCES "src/*.cpp")

add_executable(fix_win_num_popup ${SOURCES})
target_link_options(fix_win_num_popup PRIVATE -mwindows)
