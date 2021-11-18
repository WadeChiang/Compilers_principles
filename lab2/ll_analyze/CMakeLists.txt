cmake_minimum_required(VERSION 3.16.3)
project(LL1_Analyze)


set(CMAKE_CXX_STANDARD 11)

file(GLOB _SRC
    src/grammar.cc
    src/grammar.h
    src/parse.cc
)

add_executable(LL1 ${_SRC})