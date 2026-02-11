cmake_minimum_required(VERSION 3.10)
project(lfa_project)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(test
    main.cpp
    include/finiteAutomaton.cpp
    include/grammar.cpp
)

# Custom target to run the program
add_custom_target(run
    COMMAND test
    DEPENDS test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running the program..."
)

# Message for out-of-source build
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Please use an out-of-source build: mkdir build && cd build && cmake .. && cmake --build .")
endif()
