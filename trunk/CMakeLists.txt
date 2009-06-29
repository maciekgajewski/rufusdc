project(RUFUSDC)
cmake_minimum_required(VERSION 2.6)

set( CMAKE_BUILD_TYPE Debug )

set( EXECUTABLE_OUTPUT_PATH ${RUFUSDC_BINARY_DIR}/bin )
set( LIBRARY_OUTPUT_PATH ${RUFUSDC_BINARY_DIR}/lib )

add_subdirectory( dcpp )
add_subdirectory( kgui )

