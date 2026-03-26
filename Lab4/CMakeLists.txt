cmake_minimum_required(VERSION 3.10)
project(Lab4)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(SOURCES
    main.cpp
    src/lexer.cpp
    src/nodes.cpp
    src/parser.cpp
)

add_executable(test ${SOURCES})

add_test(NAME RunTest COMMAND hello)
