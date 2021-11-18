cmake_minimum_required(VERSION 3.16)

project(LexAnalyser)

set(SOURCES
    src/HashMap.c
    src/LexAnalyser.c
    src/List.c
)

add_executable(LexAnalyser ${SOURCES})

target_include_directories(LexAnalyser
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)