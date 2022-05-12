# CMake build script for Ingescape C# binding

########################################################################
# Project setup
########################################################################

cmake_minimum_required(VERSION 3.13.1)
project(IngescapeCSharp HOMEPAGE_URL https://ingescape.com LANGUAGES CSharp)

# Little hack because without this line, CMAKE_CXX_SIZEOF_VOID_P is not initialised
# And installer will alwayse end with win32, not win64
enable_language(CXX)

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
set(INGESCAPE_VERSION_MAJOR 0)
set(INGESCAPE_VERSION_MINOR 9)
set(INGESCAPE_VERSION_PATCH 0)
set(INGESCAPE_VERSION "${INGESCAPE_VERSION_MAJOR}.${INGESCAPE_VERSION_MINOR}.${INGESCAPE_VERSION_PATCH}")
message(STATUS "Detected INGESCAPE binding Version - ${INGESCAPE_VERSION}")

########################################################################
# sources files
########################################################################

set (ingescape_csharp_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Ingescape.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/ServiceArgument.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/Helpers.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/IgsAgent.cs
  ${CMAKE_CURRENT_SOURCE_DIR}/Enums.cs
)

source_group("Source Files" FILES ${ingescape_csharp_sources})

########################################################################
# Library
########################################################################

add_library(${PROJECT_NAME} SHARED ${ingescape_csharp_sources})

OPTION(UTF8_ENCODING "UTF8" OFF)
OPTION(UNITY "UNITY" OFF)
OPTION(IOS "IOS" OFF)

IF(UTF8_ENCODING)
    ADD_DEFINITIONS(-DUTF8_ENCODING)
ENDIF(UTF8_ENCODING)

IF(IOS)
    ADD_DEFINITIONS(-DIOS)
ENDIF(IOS)

IF(UNITY)
    ADD_DEFINITIONS(-DUTF8_ENCODING -DUNITY)
ENDIF(UNITY)

set_property(TARGET ${PROJECT_NAME} PROPERTY
  VS_GLOBAL_TargetFramework "netstandard2.0"
)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_PACKAGE_REFERENCES
   	"MSTest.TestFramework_1.3.2"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY
  VS_GLOBAL_EnableDefaultItems false
)

IF (WIN32)
  set_target_properties (${PROJECT_NAME} PROPERTIES
      SOVERSION "${INGESCAPE_VERSION_MAJOR}"
      VERSION "${INGESCAPE_VERSION}"
  )
ELSE (WIN32)
  set_target_properties (${PROJECT_NAME} PROPERTIES
    SOVERSION "${INGESCAPE_VERSION_MAJOR}"
    VERSION "${INGESCAPE_VERSION}"
    PREFIX "lib"
  )
ENDIF(WIN32)
