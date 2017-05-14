cmake_minimum_required(VERSION 3.6)
project(Go)

set(CMAKE_CXX_STANDARD 11)

add_library(Game Game.cpp Move.cpp Board.cpp Player.cpp CommandLinePlayer.cpp RandomPlayout.cpp RandomPlayoutPlayer.cpp)

add_executable(Go main.cpp)
target_link_libraries(Go Game)