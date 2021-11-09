cmake_minimum_required(VERSION 3.0.0)
project(draw LANGUAGES C VERSION 0.1.0)

include(CTest)
enable_testing()
set(CUSTOM_C_FLAGS "-pedantic -ansi")
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${CUSTOM_C_FLAGS}")
file(GLOB_RECURSE source "${PROJECT_SOURCE_DIR}/*.c" "${PROJECT_SOURCE_DIR}/*.h")
add_executable(draw ${source})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
