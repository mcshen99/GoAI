cmake_minimum_required(VERSION 3.6)
project(Go)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_library(Game Game.cpp Move.cpp Board.cpp Player.cpp CommandLinePlayer.cpp RandomPlayout.cpp RandomPlayoutPlayer.cpp Gtp.cpp MonteCarloNode.cpp MonteCarloPlayer.cpp MoveGenerator.cpp PatternGenerator.cpp CaptureGenerator.cpp)
add_executable(Go main.cpp)
target_link_libraries(Go Game)