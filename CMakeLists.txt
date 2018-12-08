# CMake build script for Ingescape

#TODO Add NSIS management (CPACK)
#TODO Read version from admin.c file
#TODO Fix install in ingescape subfolder
#TODO Add yajl dependency
#TODO Add clean target (see czmq)

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.0)
project(Ingescape)

enable_language(C)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# Select flags
SET(CMAKE_C_FLAGS_RELEASE "-O3")
# Will be used to add flags to pkg-config useful when apps want to statically link
set(pkg_config_libs_private "")

########################################################################
# options
########################################################################
# If build in git directory, debug build will be launch
if (NOT CMAKE_BUILD_TYPE)
    if (EXISTS "${SOURCE_DIR}/.git")
        set (CMAKE_BUILD_TYPE Debug)
    else ()
# http://xit0.org/2013/04/cmake-use-git-branch-and-commit-details-in-project/
# http://stackoverflow.com/questions/6797395/cmake-execute-process-always-fails-with-no-such-file-or-directory-when-i-cal
        execute_process(
            COMMAND git rev-parse --show-toplevel
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            RESULT_VARIABLE git_result
            OUTPUT_VARIABLE git_root
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        message(STATUS "git workspace root [${git_result}]: ${git_root}")
        if ( "${git_result}" STREQUAL "0" )
            set (CMAKE_BUILD_TYPE Debug)
        else ()
            set (CMAKE_BUILD_TYPE Release)
        endif ()
    endif ()
endif ()

########################################################################
# plateform specific
########################################################################
#The MSVC C compiler is too out of date,
#so the sources have to be compiled as c++
if (MSVC)
    enable_language(CXX)
    file(GLOB sources "${SOURCE_DIR}/src/*.c")
    set_source_files_properties(
        ${sources}
        PROPERTIES LANGUAGE CXX
    )
    set(MORE_LIBRARIES ws2_32)
endif()
# required libraries for mingw
if (MINGW)
    set(MORE_LIBRARIES -lws2_32)
endif()

# required libraries for cygwin
if (CYGWIN)
    set(MORE_LIBRARIES)
endif()

list(APPEND CMAKE_MODULE_PATH "${SOURCE_DIR}/builds/cmake/dependencies")
set(OPTIONAL_LIBRARIES)

include(CheckCCompilerFlag)
foreach(_CCOPT -pedantic -Wall -Wextra -W -Wpointer-arith -Wwrite-strings -Wunused -Wshadow -Winline -Wnested-externs -Wno-long-long -Wfloat-equal -Wno-multichar -Wsign-compare -Wundef -Wno-format-nonliteral -Wendif-labels -Wstrict-prototypes -Wstrict-aliasing=3 -Wcast-align -Wtype-limits -Wold-style-declaration -Wmissing-parameter-type -Wempty-body -Wclobbered -Wignored-qualifiers -Wconversion -Wno-sign-conversion -Wvla -Wdouble-promotion -Wno-system-headers -Wno-pedantic-ms-format)
    # surprisingly, CHECK_C_COMPILER_FLAG needs a new variable to store each new
    # test result in.
    check_c_compiler_flag(${_CCOPT} OPT${_CCOPT})
    if(OPT${_CCOPT})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_CCOPT}")
    endif()
endforeach()

########################################################################
# LIBSODIUM dependency
########################################################################
find_package(libsodium REQUIRED)
IF (LIBSODIUM_FOUND)
    include_directories(${LIBSODIUM_INCLUDE_DIRS})
    list(APPEND MORE_LIBRARIES ${LIBSODIUM_LIBRARIES})
    set(pkg_config_libs_private "${pkg_config_libs_private} -lsodium")
ELSE (LIBSODIUM_FOUND)
    message( FATAL_ERROR "libsodium not found." )
ENDIF (LIBSODIUM_FOUND)

########################################################################
# LIBZMQ dependency
########################################################################
find_package(libzmq REQUIRED)
IF (LIBZMQ_FOUND)
    include_directories(${LIBZMQ_INCLUDE_DIRS})
    list(APPEND MORE_LIBRARIES ${LIBZMQ_LIBRARIES})
    set(pkg_config_libs_private "${pkg_config_libs_private} -lzmq")
ELSE (LIBZMQ_FOUND)
    message( FATAL_ERROR "libzmq not found." )
ENDIF (LIBZMQ_FOUND)

########################################################################
# CZMQ dependency
########################################################################
find_package(czmq REQUIRED)
IF (CZMQ_FOUND)
    include_directories(${CZMQ_INCLUDE_DIRS})
    list(APPEND MORE_LIBRARIES ${CZMQ_LIBRARIES})
    set(pkg_config_libs_private "${pkg_config_libs_private} -lczmq")
ELSE (CZMQ_FOUND)
    message( FATAL_ERROR "czmq not found." )
ENDIF (CZMQ_FOUND)

########################################################################
# ZYRE dependency
########################################################################
find_package(zyre REQUIRED)
IF (ZYRE_FOUND)
    include_directories(${ZYRE_INCLUDE_DIRS})
    list(APPEND MORE_LIBRARIES ${ZYRE_LIBRARIES})
    set(pkg_config_libs_private "${pkg_config_libs_private} -lzyre")
ELSE (ZYRE_FOUND)
    message( FATAL_ERROR "zyre not found." )
ENDIF (ZYRE_FOUND)

########################################################################
# YAJL dependency
########################################################################
find_package(yajl REQUIRED)
IF (YAJL_FOUND)
    include_directories(${YAJL_INCLUDE_DIRS})
    list(APPEND MORE_LIBRARIES ${YAJL_LIBRARIES})
    set(pkg_config_libs_private "${pkg_config_libs_private} -lyajl")
ELSE (YAJL_FOUND)
    message( FATAL_ERROR "yajl not found." )
ENDIF (YAJL_FOUND)

########################################################################
# version
########################################################################
file(READ ${SOURCE_DIR}/src/admin.c ADMIN_C_CONTENT)
string(REGEX MATCH "#define INGESCAPE_MAJOR ([0-9]*)" _ ${ADMIN_C_CONTENT})
set(INGESCAPE_VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "#define INGESCAPE_MINOR ([0-9]*)" _ ${ADMIN_C_CONTENT})
set(INGESCAPE_VERSION_MINOR ${CMAKE_MATCH_1})
string(REGEX MATCH "#define INGESCAPE_MICRO ([0-9]*)" _ ${ADMIN_C_CONTENT})
set(INGESCAPE_VERSION_PATCH ${CMAKE_MATCH_1})

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
add_definitions(-DINGESCAPE=1)
include_directories("${SOURCE_DIR}/src/include")
set (ingescape_sources
    src/admin.c
    src/bus.c
    src/definition.c
    src/mapping.c
    src/model.c
    src/network.c
    src/parser.c
    src/token.c
)

IF (WIN32)
    list(APPEND ingescape_sources dependencies/windows/unix/unixfunctions.c)
ENDIF (WIN32)

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
    add_library(ingescape SHARED ${ingescape_sources})
  ELSE (MSVC)
    add_library(ingescape SHARED $<TARGET_OBJECTS:ingescape_objects>)
  ENDIF (MSVC)

  set_target_properties (ingescape PROPERTIES
    PUBLIC_HEADER "${public_headers}"
    DEFINE_SYMBOL "INGESCAPE_EXPORTS"
    SOVERSION "0"
    VERSION "${INGESCAPE_VERSION}"
    COMPILE_DEFINITIONS "DLL_EXPORT"
    OUTPUT_NAME "ingescape"
    PREFIX "lib"
  )

  target_link_libraries(ingescape
    PRIVATE ${MORE_LIBRARIES}
  )

  install(TARGETS ingescape
    EXPORT ingescape-targets
    LIBRARY DESTINATION "lib${LIB_SUFFIX}" # .so file
    ARCHIVE DESTINATION "lib${LIB_SUFFIX}" # .lib file
    RUNTIME DESTINATION bin                # .dll file
  )

  target_include_directories(ingescape
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
  )
endif()

# static
if (INGESCAPE_BUILD_STATIC)
  IF (MSVC)
    add_library(ingescape-static STATIC ${ingescape_sources})
  ELSE (MSVC)
    add_library(ingescape-static STATIC $<TARGET_OBJECTS:ingescape_objects>)
  ENDIF (MSVC)

  set_target_properties(ingescape-static PROPERTIES
    PUBLIC_HEADER "${public_headers}"
    COMPILE_DEFINITIONS "INGESCAPE_STATIC"
    OUTPUT_NAME "ingescape"
    PREFIX "lib"
  )

  target_link_libraries(ingescape-static
    PRIVATE ${MORE_LIBRARIES}
  )

  install(TARGETS ingescape-static
    EXPORT ingescape-targets
    LIBRARY DESTINATION "lib${LIB_SUFFIX}" # .so file
    ARCHIVE DESTINATION "lib${LIB_SUFFIX}" # .lib file
    RUNTIME DESTINATION bin                # .dll file
  )

  target_include_directories(ingescape-static
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
  )
  target_compile_definitions(ingescape-static
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
  export(EXPORT ingescape-targets
         FILE "${CMAKE_CURRENT_BINARY_DIR}/ingescapeTargets.cmake")
endif()
configure_package_config_file(builds/cmake/Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfig.cmake"
                              INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR})
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfigVersion.cmake
                                 VERSION ${INGESCAPE_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(EXPORT ingescape-targets
        FILE ingescapeTargets.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfigVersion.cmake
              DESTINATION ${CMAKECONFIG_INSTALL_DIR})

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
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
message (STATUS "  Install prefix    :" "${CMAKE_INSTALL_PREFIX}")
message (STATUS "")
message (STATUS "*************************************************************")
message (STATUS "Configuration complete! Now procced with:")
message (STATUS "  'make'                compile the project")
message (STATUS "  'make install'        install the project to ${CMAKE_INSTALL_PREFIX}")
message (STATUS "  'make uninstall'      uninstall the project from ${CMAKE_INSTALL_PREFIX}")
message (STATUS "")
