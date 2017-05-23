cmake_minimum_required(VERSION 3.6)
project(Go)

set(CMAKE_CXX_STANDARD 14)

add_library(Game Game.cpp Move.cpp Board.cpp Player.cpp CommandLinePlayer.cpp RandomPlayout.cpp RandomPlayoutPlayer.cpp Gtp.cpp MonteCarloNode.cpp MonteCarloPlayer.cpp)

add_executable(Go main.cpp)
target_link_libraries(Go Game)