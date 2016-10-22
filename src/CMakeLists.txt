cmake_minimum_required(VERSION 3.1)

project(dynamic_dungeon)
set(DUNGEONS_SOURCE_FILES main.cpp dungeon_util.h dungeon_util.cpp)
add_executable(dungeons WIN32 ${DUNGEONS_SOURCE_FILES})
set_target_properties(dungeons PROPERTIES CXX_STANDARD 14)

# This part should ideally be replaced with find_package or PkgConfig
# to make the build more flexible, but this works well enough for now
set(SDL2_INCLUDE_DIRS "/usr/x86_64-w64-mingw32/include/SDL2")
set(SDL2_LIBRARIES
  "/usr/x86_64-w64-mingw32/lib/libmingw32.a"
  "/usr/x86_64-w64-mingw32/lib/libSDL2main.a"
  "/usr/x86_64-w64-mingw32/lib/libSDL2.dll.a")

set(SDL2IMAGE_INCLUDE_DIRS "/usr/x86_64-w64-mingw32/include/SDL2")
set(SDL2IMAGE_LIBRARIES
  "/usr/x86_64-w64-mingw32/lib/libSDL2_image.a")

target_include_directories(dungeons
  PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

target_link_libraries(dungeons ${SDL2IMAGE_LIBRARIES} ${SDL2_LIBRARIES})
