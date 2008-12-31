project(RUFUSDC)
cmake_minimum_required(VERSION 2.6)

set( CMAKE_BUILD_TYPE Debug )

add_subdirectory( rufusdc )
add_subdirectory( kgui )
add_subdirectory( testapp )

