cmake_minimum_required(VERSION "3.2")

project(aviapplication)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE on)
# set(CMAKE_CXX_COMPILER "g++-8")
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(SDL2 REQUIRED)

file(GLOB_RECURSE SOURCES
  "dependencies/imgui/lib/*.cpp"
  "dependencies/imgui/include/*.h"
  "source/*.cpp"
  "source/*.hpp"
  "dependencies/editor/*.cpp" "dependencies/editor/*.h")

include_directories("${CMAKE_SOURCE_DIR}/dependencies/imgui/include")
include_directories("dependencies/editor/")
include_directories("${OpenCV_INCLUDE_DIRS}")
include_directories("${SDL2_INCLUDE_DIRS}")
include_directories("dependencies/lua/")

link_libraries("SDL2" "GLEW" "GL")
link_libraries("${OpenCV_LIBS}")
link_libraries("${SDL2_LIBS}")

add_library(lua
  "dependencies/lua/lapi.c"
  "dependencies/lua/lauxlib.c"
  "dependencies/lua/lbaselib.c"
  "dependencies/lua/lcode.c"
  "dependencies/lua/ldblib.c"
  "dependencies/lua/ldebug.c"
  "dependencies/lua/ldo.c"
  "dependencies/lua/ldump.c"
  "dependencies/lua/lfunc.c"
  "dependencies/lua/lgc.c"
  "dependencies/lua/linit.c"
  "dependencies/lua/liolib.c"
  "dependencies/lua/llex.c"
  "dependencies/lua/lmathlib.c"
  "dependencies/lua/lmem.c"
  "dependencies/lua/loadlib.c"
  "dependencies/lua/lobject.c"
  "dependencies/lua/lopcodes.c"
  "dependencies/lua/loslib.c"
  "dependencies/lua/lparser.c"
  "dependencies/lua/lstate.c"
  "dependencies/lua/lstring.c"
  "dependencies/lua/lstrlib.c"
  "dependencies/lua/ltable.c"
  "dependencies/lua/ltablib.c"
  "dependencies/lua/ltm.c"
  "dependencies/lua/lua.c"
  "dependencies/lua/luac.c"
  "dependencies/lua/lundump.c"
  "dependencies/lua/lvm.c"
  "dependencies/lua/lzio.c"
  "dependencies/lua/print.c" )

add_executable(avitool "${SOURCES}")
target_link_libraries(avitool lua)
target_link_libraries(avitool "stdc++fs")
