cmake_minimum_required(VERSION 2.8)
project(speech_interaction_base)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
include_directories(
    ${PROJECT_SOURCE_DIR}/src/ifly_tuling/
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/ifly_lib/include
    )
if( CMAKE_SIZEOF_VOID_P EQUAL "8" )
    link_directories(${PROJECT_SOURCE_DIR}/ifly_lib/libs/x64)
else( CMAKE_SIZEOF_VOID_P EQUAL "8" )
    link_directories(${PROJECT_SOURCE_DIR}/ifly_lib/libs/x86)
endif( CMAKE_SIZEOF_VOID_P EQUAL "8" )
aux_source_directory(${PROJECT_SOURCE_DIR}/src/ifly_tuling/ifly_interface iflyC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/ifly_tuling/tuling_interface tulingC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/ifly_tuling/broadcast_interface broadcastC)
add_library(SpeechInteractionBase SHARED 
    src/speech_interaction_base.cpp 
    src/ifly_tuling/ifly_tuling.cpp 
    ${iflyC}
    ${tulingC}
    ${broadcastC}
    )
target_link_libraries(SpeechInteractionBase msc asound curl)
