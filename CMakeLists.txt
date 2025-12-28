# Set cmake version
cmake_minimum_required(VERSION 3.22.1)

# Set project name, version, description
set(CMAKE_CXX_STANDARD 20) 
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


# Compile options
if(CMAKE_BUILD_TYPE STREQUAL "debug")
    target_link_libraries(D_Builder PRIVATE -fsanitize=address)
    add_compile_options(-g -Wall -Wextra -Wshadow -Wunused -Wconversion -pedantic  -fdiagnostics-color=always -fsanitize=address)
elseif(CMAKE_BUILD_TYPE STREQUAL "release")
    add_compile_options(-O2 -fPIC -D_FORTIFY_SOURCE=2 -Wl,-z,relro -Wl,-z,now 
    -Wall -Wextra -Wshadow -Wunused -Wconversion -pedantic -fdiagnostics-color=always)
else()
    message(FATAL_ERROR "Invalid build type, use -DCMAKE_BUILD_TYPE and set 'release' or 'debug'")
endif()


# Get them tests running
include(CTest)


#########################################################################
#                           Installation Rules                          #
#########################################################################

# Get GNU install variables
include(GNUInstallDirs)
