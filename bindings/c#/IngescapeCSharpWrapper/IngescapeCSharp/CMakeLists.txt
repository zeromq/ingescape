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

# Add path to custom macro
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(INGESCAPE_CMAKE_MODULES_DIR "${SOURCE_DIR}/builds/cmake")
list(APPEND CMAKE_MODULE_PATH ${INGESCAPE_CMAKE_MODULES_DIR})

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
  ${SOURCE_DIR}/Ingescape.cs
  ${SOURCE_DIR}/CallArgument.cs
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

########################################################################
# Installer
########################################################################
# Package installer for release build only
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  # Do not work because vs add netstandard2.0 sub folder
  #install (TARGETS ${PROJECT_NAME}
  #  LIBRARY DESTINATION "lib${LIB_SUFFIX}" # .so file
  #  ARCHIVE DESTINATION "lib${LIB_SUFFIX}" # .lib file
  #  RUNTIME DESTINATION "lib${LIB_SUFFIX}" # .dll file
  #)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Release/netstandard2.0/${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
    DESTINATION lib${LIB_SUFFIX}
  )

  set(CPACK_MONOLITHIC_INSTALL ON)
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} library")
  set(CPACK_PACKAGE_VENDOR "Ingescape")
  set(CPACK_PACKAGE_CONTACT "contact@ingescape.com")
  set(CPACK_PACKAGE_VERSION ${INGESCAPE_VERSION})
  set(CPACK_PACKAGE_VERSION_MAJOR "${INGESCAPE_VERSION_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${INGESCAPE_VERSION_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${INGESCAPE_VERSION_PATCH}")

  if (WIN32)
    # On windows put all agents in ingescape directory
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "ingescape")
  endif (WIN32)

  # set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

  # Installers for 32- vs. 64-bit CMake:
  #  - Root install directory (displayed to end user at installer-run time)
  #  - "NSIS package/display name" (text used in the installer GUI)
  #  - Registry key used to store info about the installation
  if ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION} (Win64)")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} (Win64)")
  else()
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} (Win32)")
  endif()

  # NSIS configuration for Windows
  set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}")
  set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})
  set(CPACK_NSIS_HELP_LINK "https://ingescape.com")
  set(CPACK_NSIS_CONTACT "contact@ingescape.com")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
  set(CPACK_NSIS_MODIFY_PATH OFF)
  set(CPACK_NSIS_URL_INFO_ABOUT ${CMAKE_PROJECT_HOMEPAGE_URL})

  set(CPACK_SOURCE_IGNORE_FILES "${SOURCE_DIR}/cmake-vs/")

  if (WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
  elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    set(CPACK_GENERATOR "STGZ")
  else ()
    set(CPACK_GENERATOR "ZIP")
  endif ()

  include (CPack)
endif()
