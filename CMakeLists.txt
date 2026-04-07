cmake_minimum_required(VERSION 3.20)
project(eliott-evolution VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML CONFIG REQUIRED Graphics Window System)

add_subdirectory(extern/eliott-ecs)

add_executable(eliott-evolution
    src/main.cpp
)

target_include_directories(eliott-evolution PUBLIC
    include/
)

target_link_libraries(eliott-evolution PUBLIC
    SFML::Graphics
    eliott-ecs
)