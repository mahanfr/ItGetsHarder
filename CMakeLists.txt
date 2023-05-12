cmake_minimum_required(VERSION 3.7)
project(it_gets_harder)

set(CMAKE_CXX_STANDARD 17)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_image_INCLUDE_DIRS})

add_executable(it_gets_harder main.cpp ./src/utils.cpp ./src/platformer.cpp)
target_link_libraries(it_gets_harder ${SDL2_LIBRARIES} SDL2_image::SDL2_image)
