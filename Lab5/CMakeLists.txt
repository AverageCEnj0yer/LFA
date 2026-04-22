cmake_minimum_required(VERSION 3.10)
project(Lab5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test 
    main.cpp 
    src/grammar.cpp
)

target_include_directories(test PRIVATE include)
