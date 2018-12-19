# CMake build script for Ingescape

#TODO Add NSIS management (CPACK)
#TODO Add clean target (see czmq)

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.13.1)
project(Ingescape HOMEPAGE_URL https://ingescape.com)

enable_language(C)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# Select flags
SET(CMAKE_C_FLAGS_RELEASE "-O2")
# Will be used to add flags to pkg-config useful when apps want to statically link
set(pkg_config_libs_private "")
# Add path to custom macro
set(INGESCAPE_CMAKE_MODULES_DIR "${SOURCE_DIR}/builds/cmake/modules")
list(APPEND CMAKE_MODULE_PATH ${INGESCAPE_CMAKE_MODULES_DIR})
include(IngescapeHelper)

########################################################################
# options
########################################################################
# Default build mode is debug
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif ()

add_definitions(-DINGESCAPE=1)

########################################################################
# plateform specific
########################################################################
if (NOT MSVC)
  include(CheckCCompilerFlag)
  foreach(_CCOPT -pedantic -Wall -Wextra -W -Wpointer-arith -Wwrite-strings -Wunused -Wshadow -Winline -Wnested-externs -Wno-long-long -Wfloat-equal -Wno-multichar -Wsign-compare -Wundef -Wno-format-nonliteral -Wendif-labels -Wstrict-prototypes -Wstrict-aliasing=3 -Wcast-align -Wtype-limits -Wold-style-declaration -Wmissing-parameter-type -Wempty-body -Wclobbered -Wignored-qualifiers -Wconversion -Wno-sign-conversion -Wvla -Wdouble-promotion -Wno-system-headers -Wno-pedantic-ms-format)
    # surprisingly, CHECK_C_COMPILER_FLAG needs a new variable to store each new
    # test result in.
    check_c_compiler_flag(${_CCOPT} OPT${_CCOPT})
    if(OPT${_CCOPT})
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_CCOPT}")
    endif()
  endforeach()
endif()

########################################################################
# dependency
########################################################################
add_ingescape_libraries_dependencies(MORE_LIBRARIES pkg_config_libs_private)

########################################################################
# version
########################################################################
get_ingescape_version(INGESCAPE_VERSION_MAJOR INGESCAPE_VERSION_MINOR INGESCAPE_VERSION_PATCH)

set(INGESCAPE_VERSION "${INGESCAPE_VERSION_MAJOR}.${INGESCAPE_VERSION_MINOR}.${INGESCAPE_VERSION_PATCH}")
message(STATUS "Detected INGESCAPE Version - ${INGESCAPE_VERSION}")

########################################################################
# includes
########################################################################
set (ingescape_headers_to_install
    src/include/ingescape.h
    src/include/ingescape_advanced.h
)

set (ingescape_headers
    ${ingescape_headers_to_install}
    src/include/ingescape_advanced.h
    src/include/ingescape_private.h
    src/include/uthash/libut.h
    src/include/uthash/ringbuf.h
    src/include/uthash/utarray.h
    src/include/uthash/uthash.h
    src/include/uthash/utlist.h
    src/include/uthash/utmm.h
    src/include/uthash/utringbuffer.h
    src/include/uthash/utstring.h
    src/include/uthash/utvector.h
)

IF (WIN32)
    list(APPEND ingescape_headers dependencies/windows/unix/)
ENDIF (WIN32)

source_group ("Header Files" FILES ${ingescape_headers})
install(FILES ${ingescape_headers_to_install} DESTINATION include/ingescape)

########################################################################
# library
########################################################################

add_ingescape_include_directory(_INCLUDES_DIRECTORY)
include_directories(${_INCLUDES_DIRECTORY})

add_ingescape_sources(ingescape_sources)

source_group("Source Files" FILES ${ingescape_sources})

option(INGESCAPE_BUILD_SHARED "Whether or not to build the shared object" ON)
option(INGESCAPE_BUILD_STATIC "Whether or not to build the static archive" ON)

if (NOT INGESCAPE_BUILD_SHARED AND NOT INGESCAPE_BUILD_STATIC)
  message(FATAL_ERROR "Neither static nor shared library build enabled")
endif()

IF (NOT MSVC)
  # avoid building everything twice for shared + static
  # only on *nix, as Windows needs different preprocessor defines in static builds
  add_library (ingescape_objects OBJECT ${ingescape_sources})
  set_property(TARGET ingescape_objects PROPERTY POSITION_INDEPENDENT_CODE ON)
ENDIF (NOT MSVC)

# shared
if (INGESCAPE_BUILD_SHARED)
  IF (MSVC)
    add_library(${PROJECT_NAME} SHARED ${ingescape_sources})
  ELSE (MSVC)
    add_library(${PROJECT_NAME} SHARED $<TARGET_OBJECTS:ingescape_objects>)
  ENDIF (MSVC)

  set_target_properties (${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${public_headers}"
    DEFINE_SYMBOL "INGESCAPE_EXPORTS"
    SOVERSION "0"
    VERSION "${INGESCAPE_VERSION}"
    OUTPUT_NAME "ingescape"
    PREFIX "lib"
  )

  target_link_libraries(${PROJECT_NAME} PRIVATE ${MORE_LIBRARIES})

  install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION "lib${LIB_SUFFIX}" # .so file
    ARCHIVE DESTINATION "lib${LIB_SUFFIX}" # .lib file
    RUNTIME DESTINATION bin                # .dll file
  )

  target_include_directories(${PROJECT_NAME}
    PUBLIC
      $<BUILD_INTERFACE:${SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
  )
endif()

# static
if (INGESCAPE_BUILD_STATIC)
  IF (MSVC)
    add_library(${PROJECT_NAME}-static STATIC ${ingescape_sources})
  ELSE (MSVC)
    add_library(${PROJECT_NAME}-static STATIC $<TARGET_OBJECTS:ingescape_objects>)
  ENDIF (MSVC)

  set_target_properties(${PROJECT_NAME}-static PROPERTIES
    PUBLIC_HEADER "${public_headers}"
    COMPILE_DEFINITIONS "INGESCAPE_STATIC"
    OUTPUT_NAME "ingescape"
    PREFIX "lib"
  )

  target_link_libraries(${PROJECT_NAME}-static PRIVATE ${MORE_LIBRARIES})

  install(TARGETS ${PROJECT_NAME}-static
    EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION "lib${LIB_SUFFIX}" # .so file
    ARCHIVE DESTINATION "lib${LIB_SUFFIX}" # .lib file
    RUNTIME DESTINATION bin                # .dll file
  )

  target_include_directories(${PROJECT_NAME}-static
    PUBLIC
      $<BUILD_INTERFACE:${SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
  )
  target_compile_definitions(${PROJECT_NAME}-static
    PUBLIC INGESCAPE__STATIC
  )
endif()

########################################################################
# pkgconfig
########################################################################
set (VERSION "0.8.1")
set (prefix "${CMAKE_INSTALL_PREFIX}")
set (exec_prefix "\${prefix}")
set (libdir "\${prefix}/lib${LIB_SUFFIX}")
set (includedir "\${prefix}/include")
set (pkg_config_defines "")
configure_file(
    "${SOURCE_DIR}/src/libingescape.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libingescape.pc"
@ONLY)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/libingescape.pc"
    DESTINATION "lib${LIB_SUFFIX}/pkgconfig"
)

########################################################################
# installer
########################################################################
include(CMakePackageConfigHelpers)
if (WIN32)
  set(CMAKECONFIG_INSTALL_DIR "CMake" CACHE STRING "install path for ingescapeConfig.cmake")
else()
  # GNUInstallDirs "DATADIR" wrong here; CMake search path wants "share".
  set(CMAKECONFIG_INSTALL_DIR "share/cmake/ingescape" CACHE STRING "install path for ingescapeConfig.cmake")
endif()

if (NOT CMAKE_VERSION VERSION_LESS 3.0)
  export(EXPORT ${PROJECT_NAME}-targets
         FILE "${CMAKE_CURRENT_BINARY_DIR}/ingescapeTargets.cmake")
endif()
configure_package_config_file(builds/cmake/Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfig.cmake"
                              INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR})
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfigVersion.cmake
                                 VERSION ${INGESCAPE_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(EXPORT ${PROJECT_NAME}-targets
        FILE ingescapeTargets.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfigVersion.cmake
              DESTINATION ${CMAKECONFIG_INSTALL_DIR})

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${SOURCE_DIR}/builds/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

########################################################################
# installer
########################################################################
# Package installer just for release build
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CPACK_MONOLITHIC_INSTALL ON)
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} library")
  set(CPACK_PACKAGE_VENDOR "Ingenuity IO")
  set(CPACK_PACKAGE_CONTACT "contact@ingenuity.io")
  set(CPACK_PACKAGE_VERSION ${INGESCAPE_VERSION})
  set(CPACK_PACKAGE_VERSION_MAJOR "${INGESCAPE_VERSION_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${INGESCAPE_VERSION_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${INGESCAPE_VERSION_PATCH}")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

  # Installers for 32- vs. 64-bit CMake:
  #  - Root install directory (displayed to end user at installer-run time)
  #  - "NSIS package/display name" (text used in the installer GUI)
  #  - Registry key used to store info about the installation
  if(CMAKE_CL_64)
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION} (Win64)")
  else()
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
  endif()
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_NSIS_PACKAGE_NAME}")

  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
  set(CPACK_NSIS_MODIFY_PATH ON)
  set(CPACK_NSIS_URL_INFO_ABOUT ${CMAKE_PROJECT_HOMEPAGE_URL})

  set(CPACK_SOURCE_IGNORE_FILES "${SOURCE_DIR}/bindings/;${SOURCE_DIR}/build/;${SOURCE_DIR}/.git/;${SOURCE_DIR}/doc/;${SOURCE_DIR}/editor/;${SOURCE_DIR}/ingeprobe/;${SOURCE_DIR}/packaging/")
  
  if (WIN32)
      set(CPACK_GENERATOR "NSIS;ZIP")
      set(CPACK_SOURCE_GENERATOR "ZIP")
      # On windows we want to package dependencies with this library
      install_ingescape_dependencies(LIBSODIUM_LIBRARIES LIBSODIUM_INCLUDE_DIRS)
      install_ingescape_dependencies(LIBZMQ_LIBRARIES LIBZMQ_INCLUDE_DIRS)
      install_ingescape_dependencies(CZMQ_LIBRARIES CZMQ_INCLUDE_DIRS)
      install_ingescape_dependencies(ZYRE_LIBRARIES ZYRE_INCLUDE_DIRS)
      install_ingescape_dependencies(YAJL_LIBRARIES YAJL_INCLUDE_DIRS)

      list(APPEND CPACK_SOURCE_IGNORE_FILES ${SOURCE_DIR}/dependencies/android)
      list(APPEND CPACK_SOURCE_IGNORE_FILES ${SOURCE_DIR}/dependencies/ios)
      list(APPEND CPACK_SOURCE_IGNORE_FILES ${SOURCE_DIR}/dependencies/macos)
      list(APPEND CPACK_SOURCE_IGNORE_FILES ${SOURCE_DIR}/dependencies/raspberry)
      list(APPEND CPACK_SOURCE_IGNORE_FILES ${SOURCE_DIR}/dependencies/unix)
  elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
      set(CPACK_GENERATOR "STGZ")
      set(CPACK_SOURCE_GENERATOR "TGZ")
      list(APPEND CPACK_SOURCE_IGNORE_FILES ${SOURCE_DIR}/dependencies/)
  else ()
      set(CPACK_GENERATOR "DEB;RPM")
      set(CPACK_SOURCE_GENERATOR "TGZ")
      list(APPEND CPACK_SOURCE_IGNORE_FILES ${SOURCE_DIR}/dependencies/)
  endif ()

  include (CPack)
endif()

########################################################################
# summary
########################################################################
message ("")
message (STATUS "******************* Configuration Summary *******************")
message (STATUS "General:")
message (STATUS "  Version           :   ${VERSION}")
message (STATUS "  System            :   ${CMAKE_SYSTEM_NAME}")
message (STATUS "  C compiler        :   ${CMAKE_C_COMPILER}")
message (STATUS "  Debug C flags     :   ${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
message (STATUS "  Release C flags   :   ${CMAKE_C_FLAGS_RELEASE} ${CMAKE_C_FLAGS}")
message (STATUS "  Build type        :   ${CMAKE_BUILD_TYPE}")
message (STATUS "  Static build      :   ${INGESCAPE_BUILD_STATIC}")
message (STATUS "  Shared build      :   ${INGESCAPE_BUILD_SHARED}")
message (STATUS "")
message (STATUS "Dependencies:")
include(FeatureSummary)
feature_summary (WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
message (STATUS "")
message (STATUS "Install:")
message (STATUS "  Install prefix    :   ${CMAKE_INSTALL_PREFIX}")
message (STATUS "")
if (NOT MSVC)
message (STATUS "*************************************************************")
message (STATUS "Configuration complete! Now procced with:")
message (STATUS "  'make'                compile the project")
message (STATUS "  'make install'        install the project to ${CMAKE_INSTALL_PREFIX}")
message (STATUS "  'make uninstall'      uninstall the project from ${CMAKE_INSTALL_PREFIX}")
if (CMAKE_BUILD_TYPE STREQUAL "Release")
message (STATUS "  'make package'        Create library installer")
message (STATUS "  'make package_source' Create source installer")
endif ()
endif ()
message (STATUS "")
