cmake_minimum_required(VERSION 2.8.9)
project(igsRecorder)

set(CMAKE_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../builds/cmake")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULES_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}) #for local headers

#set(SOURCES main.c)
file(GLOB SOURCES "*.c")

add_executable(firstFullAgent ${SOURCES})

find_package (czmq)
if (CZMQ_FOUND)
include_directories(${CZMQ_INCLUDE_DIRS})
target_link_libraries (firstFullAgent ${CZMQ_LIBRARIES})
endif (CZMQ_FOUND)

find_package (ingescape)
if (INGESCAPE_FOUND)
    include_directories(${INGESCAPE_INCLUDE_DIRS})
    target_link_libraries (firstFullAgent ${INGESCAPE_LIBRARIES})
endif (INGESCAPE_FOUND)

target_link_libraries(firstFullAgent PRIVATE ${MORE_LIBRARIES})

install (TARGETS firstFullAgent RUNTIME DESTINATION bin)