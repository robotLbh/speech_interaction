cmake_minimum_required(VERSION 2.8)
project(test)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../src)
if( CMAKE_SIZEOF_VOID_P EQUAL "8" )
link_directories(
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../ifly_lib/libs/x64)
else( CMAKE_SIZEOF_VOID_P EQUAL "8" )
link_directories(
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../ifly_lib/libs/x86)
endif( CMAKE_SIZEOF_VOID_P EQUAL "8" )
add_executable(test main.cpp glog_speech.cpp)
target_link_libraries(test SpeechInteractionBase glog)
