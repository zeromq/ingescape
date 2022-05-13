# CMake build script for Ingescape C# binding

########################################################################
# Project setup
########################################################################

cmake_minimum_required(VERSION 3.13.1)
project(IngescapeCSharp HOMEPAGE_URL https://ingescape.com LANGUAGES CSharp)

include(CSharpUtilities)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CSharp_FLAGS "/langversion:6")

if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Debug Release)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
    "Reset the configurations to what we need"
    FORCE)
endif()

# Default build mode is debug
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif ()

########################################################################
# version
########################################################################
# Set version to current ingescape version
set(MAJOR 3)
set(MINOR 5)
set(BUILD 49)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/AssemblyInfo.cs.in
  ${CMAKE_CURRENT_SOURCE_DIR}/AssemblyInfo.cs
  @ONLY)

########################################################################
# sources files
########################################################################

set (ingescape_csharp_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Ingescape.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/ServiceArgument.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/Helpers.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/IgsAgent.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/Enums.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/AssemblyInfo.cs
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
