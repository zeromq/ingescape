# CMake build script for Ingescape C# binding

########################################################################
# Project setup
########################################################################

cmake_minimum_required(VERSION 3.13.1)
project(IngescapeCSharp HOMEPAGE_URL https://ingescape.com LANGUAGES CSharp)

set(WITH_CSHARP_TEST ON CACHE INTERNAL "")

if (WITH_CSHARP_TEST)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/test)
endif()

include(CSharpUtilities)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CSharp_FLAGS "/langversion:6")

if (CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Debug Release)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
    "Reset the configurations to what we need"
    FORCE)
endif()

# Default build mode is debug
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../builds/cmake/modules")

########################################################################
# version
########################################################################

if (NOT WITH_CSHARP_WRAPPER)
  include(IngescapeHelper)
  get_ingescape_version(INGESCAPE_VERSION_MAJOR INGESCAPE_VERSION_MINOR INGESCAPE_VERSION_PATCH)
endif ()

set(MAJOR ${INGESCAPE_VERSION_MAJOR})
set(MINOR ${INGESCAPE_VERSION_MINOR})
set(BUILD ${INGESCAPE_VERSION_PATCH})

message("Ingescape version : ${INGESCAPE_VERSION_MAJOR}.${INGESCAPE_VERSION_MINOR}.${INGESCAPE_VERSION_PATCH}")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake/AssemblyInfo.cs.in
  ${CMAKE_CURRENT_BINARY_DIR}/AssemblyInfo.cs
  @ONLY)

########################################################################
# sources files
########################################################################

set (ingescape_csharp_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Ingescape.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ServiceArgument.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Helpers.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/src/IgsAgent.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Enums.cs
  ${CMAKE_CURRENT_BINARY_DIR}/AssemblyInfo.cs
)
source_group("Source Files" FILES ${ingescape_csharp_sources})

########################################################################
# Library
########################################################################

add_library(${PROJECT_NAME} SHARED ${ingescape_csharp_sources})


set_property(TARGET ${PROJECT_NAME} PROPERTY
  VS_GLOBAL_TargetFramework "netstandard2.0"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY
  VS_GLOBAL_GenerateAssemblyInfo false
)

set_property(TARGET ${PROJECT_NAME} PROPERTY
  VS_GLOBAL_EnableDefaultItems false
)

if (WITH_CSHARP_TEST)
  target_link_libraries(igstester ${PROJECT_NAME})
endif ()