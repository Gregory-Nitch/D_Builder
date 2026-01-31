# Set cmake version
cmake_minimum_required(VERSION 3.22.1)

# Set project name, version, description
set(CMAKE_CXX_STANDARD 23) 
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(D_Builder VERSION 1.0.0 DESCRIPTION "D_builder" LANGUAGES CXX)

# Files to use
add_executable(D_Builder src/d_builder.cpp)

target_sources(D_Builder 
              PRIVATE src/d_builder.cpp
              PRIVATE src/d_builder_common.cpp
              PRIVATE src/d_map.cpp
              PRIVATE src/d_tile.cpp
            )

target_include_directories(D_Builder PRIVATE inc/)

# Qt stuff
if(NOT CMAKE_PREFIX_PATH)
    message(STATUS "Using default Qt path...")
    set(CMAKE_PREFIX_PATH "~/repos/Qt/6.10.1/gcc_64/")
else()
    message(STATUS "Using passed Qt path...")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Gui REQUIRED)

target_link_libraries(D_Builder
    PRIVATE
    Qt6::Gui
)

# Compile options
if(CMAKE_BUILD_TYPE STREQUAL "debug")
    add_compile_options(-g -Wall -Wextra -Wshadow -Wunused -Wconversion -pedantic  -fdiagnostics-color=always)
elseif(CMAKE_BUILD_TYPE STREQUAL "release")
    add_compile_options(-O2 -fPIC -D_FORTIFY_SOURCE=2 -Wl,-z,relro -Wl,-z,now 
    -Wall -Wextra -Wshadow -Wunused -Wconversion -pedantic -fdiagnostics-color=always)
else()
    message(FATAL_ERROR "Invalid build type, use -DCMAKE_BUILD_TYPE and set 'release' or 'debug'")
endif()

# Test executable for generation tests
add_executable(D_Generation_Test src/d_generation_test.cpp)

target_sources(D_Generation_Test 
    PRIVATE src/d_generation_test.cpp
    PRIVATE src/d_builder_common.cpp
    PRIVATE src/d_map.cpp
    PRIVATE src/d_tile.cpp
)

target_include_directories(D_Generation_Test PRIVATE inc/)
target_link_libraries(D_Generation_Test PRIVATE Qt6::Gui)

# Get them tests running
include(CTest)

# Add test suite with valgrind -> # -s = show suppressed errors
add_test(NAME generation_tests
    COMMAND valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose -s
    $<TARGET_FILE:D_Generation_Test>
)
set_tests_properties(generation_tests
    PROPERTIES 
    TIMEOUT 0
    PASS_REGULAR_EXPRESSION
    ".*in use at exit: 0 bytes in 0 blocks.*0 errors from 0 contexts.*suppressed: 0 from 0.*"
)

#########################################################################
#                           Installation Rules                          #
#########################################################################

# Get GNU install variables
include(GNUInstallDirs)
