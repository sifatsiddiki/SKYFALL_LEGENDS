cmake_minimum_required(VERSION 3.16)
project(SkyfallLegends_CPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This project is written against SFML 2.x API
find_package(SFML 2 REQUIRED COMPONENTS system window graphics audio)

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

add_executable(SkyfallLegends ${SOURCES})

target_include_directories(SkyfallLegends PRIVATE src)

target_link_libraries(SkyfallLegends PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)
