cmake_minimum_required(VERSION 3.21.3)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

project(birb2d VERSION 0.1)

set(ENGINE_SRC
./Birb2D/src/utils.cpp
./Birb2D/src/renderwindow.cpp
./Birb2D/src/timer.cpp
./Birb2D/src/timestep.cpp
./Birb2D/src/values.cpp
./Birb2D/src/entity.cpp
./Birb2D/src/logger.cpp
./Birb2D/src/math.cpp
)

#find_package(SDL2 REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2GFX REQUIRED SDL2_gfx)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)
include_directories(${Boost_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS} ${SDL2GFX_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/Birb2D/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

# SeedSplit
add_executable(SeedSplit ${ENGINE_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/src/seedsplit.cpp)
target_link_libraries(SeedSplit ${Boost_LIBRARIES} ${Boost_filesystem_LIBRARY} ${SDL2_LIBRARIES} ${SDL2GFX_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2MIXER_LIBRARIES})
