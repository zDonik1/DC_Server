cmake_minimum_required(VERSION 3.8)
project(DC_Server)

file (GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file (GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SFML_DIR "C:/work/SFML-2.5.1/lib/cmake/SFML")
#set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-network)
