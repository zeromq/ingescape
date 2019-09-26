cmake_minimum_required(VERSION 2.8.9)
project(ingeprobe)

set(CMAKE_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULES_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src) # for local headers

if(WIN32)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/windows)
  # for getopt.h
endif(WIN32)

if(UNIX AND NOT APPLE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
endif()

# set(SOURCES main.c)
file(GLOB SOURCES "src/*.c")

add_executable(ingeprobe ${SOURCES})

find_package(sodium)
if(SODIUM_FOUND)
  include_directories(${SODIUM_INCLUDE_DIRS})
  target_link_libraries(ingeprobe ${SODIUM_LIBRARIES})
else(SODIUM_FOUND)
  message(FATAL_ERROR "Dependency not found")
endif(SODIUM_FOUND)

find_package(czmq)
if(CZMQ_FOUND)
  include_directories(${CZMQ_INCLUDE_DIRS})
  target_link_libraries(ingeprobe ${CZMQ_LIBRARIES})
else(CZMQ_FOUND)
  message(FATAL_ERROR "Dependency not found")
endif(CZMQ_FOUND)

find_package(zyre)
if(ZYRE_FOUND)
  include_directories(${ZYRE_INCLUDE_DIRS})
  target_link_libraries(ingeprobe ${ZYRE_LIBRARIES})
else(ZYRE_FOUND)
  message(FATAL_ERROR "Dependency not found")
endif(ZYRE_FOUND)

target_link_libraries(ingeprobe PRIVATE ${MORE_LIBRARIES})

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "ingeprobe")

install(TARGETS ingeprobe RUNTIME DESTINATION bin)

# ##############################################################################
# installer
# ##############################################################################
# Package installer for release build only
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CPACK_MONOLITHIC_INSTALL ON)
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
      "License generator for Ingescape platforms")
  set(CPACK_PACKAGE_VENDOR "Ingescape")
  set(CPACK_PACKAGE_CONTACT "contact@ingescape.com")

  # Installers for 32- vs. 64-bit CMake: - Root install directory (displayed to
  # end user at installer-run time) - "NSIS package/display name" (text used in
  # the installer GUI) - Registry key used to store info about the installation
  if(CMAKE_CL_64)
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_PACKAGE_NAME
        "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION} (Win64)")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} (Win64)")
  else()
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    set(CPACK_NSIS_PACKAGE_NAME
        "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} (Win32)")
  endif()

  # NSIS configuration for Windows see https://cmake.org/cmake/help/v3.14/cpack_
  # gen/nsis.html#cpack_gen:CPack%20NSIS%20Generator
  set(CPACK_NSIS_DISPLAY_NAME "First full agent")
  set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})
  set(CPACK_NSIS_HELP_LINK "https://ingescape.com")
  set(CPACK_NSIS_CONTACT "contact@ingescape.com")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
  set(CPACK_NSIS_MODIFY_PATH ON)
  set(CPACK_NSIS_URL_INFO_ABOUT ${CMAKE_PROJECT_HOMEPAGE_URL})

  if(WIN32)
    # On windows put all agents in ingescape directory
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "ingescape")
  endif(WIN32)

  set(CPACK_SOURCE_IGNORE_FILES
      "${CMAKE_CURRENT_SOURCE_DIR}/.git/;${CMAKE_CURRENT_SOURCE_DIR}/build/")

  if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")

  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CPACK_GENERATOR "STGZ;ZIP")

  else()
    set(CPACK_GENERATOR "DEB;RPM;ZIP")

    # Debian 10 (buster/testing) TODO Debian dependencies need to be different
    # between stretch and buster set(CPACK_DEBIAN_PACKAGE_DEPENDS "libczmq4,
    # libingescape")

    # Debian 9 (stretch/stable)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libczmq4, libingescape")
    set(CPACK_RPM_PACKAGE_REQUIRES "libczmq4, libingescape")

  endif()

  include(CPack)
endif()
