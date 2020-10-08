# CMake build script for Ingescape

# ##############################################################################
# Project setup
# ##############################################################################
cmake_minimum_required(VERSION 3.13.1)
project(ingescape HOMEPAGE_URL https://ingescape.com LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
ADD_DEFINITIONS (-DZYRE_BUILD_DRAFT_API)
ADD_DEFINITIONS (-DCZMQ_BUILD_DRAFT_API)

# Add path to custom macro
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake/dependencies")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake/modules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake/modules/library")

# Include custom helpers
include(IngescapeHelper)
include(IdentifyOS)

# Default build mode is debug
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Check C compiler flags but not on MSVC
if(NOT MSVC)
  include(CheckCCompilerFlag)
  foreach(_CCOPT
          -pedantic
          -Wall
          -Wextra
          -W
          -Wpointer-arith
          -Wwrite-strings
          -Wunused
          -Wshadow
          -Winline
          -Wnested-externs
          -Wno-long-long
          -Wfloat-equal
          -Wno-multichar
          -Wsign-compare
          -Wundef
          -Wno-format-nonliteral
          -Wendif-labels
          -Wstrict-prototypes
          -Wstrict-aliasing=3
          -Wcast-align
          -Wtype-limits
          -Wold-style-declaration
          -Wmissing-parameter-type
          -Wempty-body
          -Wclobbered
          -Wignored-qualifiers
          -Wconversion
          -Wno-sign-conversion
          -Wvla
          -Wdouble-promotion
          -Wno-system-headers
          -Wno-pedantic-ms-format)
    # surprisingly, CHECK_C_COMPILER_FLAG needs a new variable to store each new
    # test result in.
    check_c_compiler_flag(${_CCOPT} OPT${_CCOPT})
    if(OPT${_CCOPT})
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_CCOPT}")
    endif()
  endforeach()
endif()

# gets version from admin.c file
get_ingescape_version(INGESCAPE_VERSION_MAJOR INGESCAPE_VERSION_MINOR INGESCAPE_VERSION_PATCH)
set(INGESCAPE_VERSION "${INGESCAPE_VERSION_MAJOR}.${INGESCAPE_VERSION_MINOR}.${INGESCAPE_VERSION_PATCH}")
message(STATUS "Detected INGESCAPE Version - ${INGESCAPE_VERSION}")

# ##############################################################################
# Source files
# ##############################################################################
list(APPEND ingescape_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_definition.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_mapping.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_model.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_network.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_admin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_bus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_call.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_license.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_monitor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_json.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_performance.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_agent.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/igs_replay.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/yajl_alloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/yajl_buf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/yajl_encode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/yajl_gen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/yajl_lex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/yajl_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/yajl_tree.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/yajl_version.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/yajl.c
    $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/windows/unix/unixfunctions.c>
    $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_BINARY_DIR}/version.rc>
)

source_group("Source Files" FILES ${ingescape_sources})

# ##############################################################################
# Header files
# ##############################################################################
set(ingescape_headers_to_install
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ingescape.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ingescape_agent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ingescape_advanced.h
)

set(ingescape_private_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ingescape_private.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/uthash/libut.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/uthash/ringbuf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/uthash/utarray.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/uthash/uthash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/uthash/utlist.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/uthash/utmm.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/uthash/utringbuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/uthash/utstring.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/uthash/utvector.h
)

source_group("Header Files" FILES ${ingescape_private_headers} ${ingescape_headers_to_install})


########################################################################
# Dependencies
########################################################################
find_package(libsodium REQUIRED)
find_package(ZeroMQ 4.3.4 REQUIRED)
message(STATUS "Found ZeroMQ version: ${ZeroMQ_VERSION}")
find_package(czmq 4.2.1 REQUIRED)
message(STATUS "Found czmq version: ${czmq_VERSION}")
find_package(zyre 2.0.1 REQUIRED)
message(STATUS "Found zyre version: ${zyre_VERSION}")

if (APPLE)
  find_library(CORE_FRAMEWORK CoreFoundation)
endif (APPLE)

# ##############################################################################
# library
# ##############################################################################
option(INGESCAPE_BUILD_SHARED "Whether or not to build the shared object" ON)
option(INGESCAPE_BUILD_STATIC "Whether or not to build the static archive" ON)
if(NOT INGESCAPE_BUILD_SHARED AND NOT INGESCAPE_BUILD_STATIC)
  message(FATAL_ERROR "Neither static nor shared library build enabled")
endif()


if(NOT MSVC)
  # avoid building everything twice for shared + static only on *nix, as Windows
  # needs different preprocessor defines in static builds
  #FIXME This is a good idea but it's not used anymore. Let's fix that!
  #add_library(ingescape_objects OBJECT ${ingescape_sources})
  #set_property(TARGET ingescape_objects PROPERTY POSITION_INDEPENDENT_CODE ON)
else()
  # WIN32_LIB_FILEFLAGS is used in the RC file that will register the DLL version
  if (${CMAKE_BUILD_TYPE} MATCHES "Release")
    set(WIN32_LIB_FILEFLAGS "0")
  else ()
      set(WIN32_LIB_FILEFLAGS "VS_FF_DEBUG")
  endif()
endif()


# shared
if(INGESCAPE_BUILD_SHARED)
  add_library(${PROJECT_NAME} SHARED ${ingescape_sources})

  # OS specifig target properties
  if(MSVC)
    # WIN32_LIB_FILETYPE is used in the RC file that will register the DLL version
    set(WIN32_LIB_FILETYPE "VFT_DLL")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake/modules/library/library_version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)

    set_target_properties(${PROJECT_NAME}
                            PROPERTIES
                              OUTPUT_NAME_RELEASE "ingescape"
                              OUTPUT_NAME_DEBUG "ingescaped"
    )

    math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")
    if (${BITS} EQUAL 64)
        install(DIRECTORY dependencies/windows/vcredist/ DESTINATION ressources FILES_MATCHING PATTERN "*64.exe")
    else()
      install(DIRECTORY dependencies/windows/vcredist/ DESTINATION ressources FILES_MATCHING PATTERN "*86.exe")
    endif()

  else(MSVC)
    set_target_properties(${PROJECT_NAME}
                            PROPERTIES
                              #PUBLIC_HEADER "${ingescape_headers_to_install}"
                              #TODO We do not generate a Framework yet.
                              #FRAMEWORK TRUE
                              #FRAMEWORK_VERSION C
                              #MACOSX_FRAMEWORK_IDENTIFIER com.ingescape
                              #XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "MyAccount Developer"
                              VERSION "${INGESCAPE_VERSION}" # Current version
                              SOVERSION "${INGESCAPE_VERSION_MAJOR}.0.0" # Compatibility version
                              OUTPUT_NAME "ingescape"
                              PREFIX "lib"
    )
  endif(MSVC)

  # Add compilation #defines to the target
  target_compile_definitions(${PROJECT_NAME} PRIVATE INGESCAPE=1;TARGET_OS_IOS=0)

  # Add linked dependencies
  #FIXME All private here ?
  target_link_libraries(${PROJECT_NAME}
    PUBLIC
      czmq
      libzmq
    PRIVATE
      ${LIBSODIUM_LIBRARIES}
      zyre
      $<$<BOOL:${APPLE}>:${CORE_FRAMEWORK}>
      $<$<BOOL:${WIN32}>:ws2_32>
  )

  # Header files
  target_include_directories(${PROJECT_NAME}
    PUBLIC
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
    PRIVATE
      ${LIBSODIUM_INCLUDE_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/api
      $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/windows/unix>
  )

  install(TARGETS ${PROJECT_NAME}
          EXPORT ${PROJECT_NAME}-targets
          FRAMEWORK DESTINATION "Frameworks" # .framework file
                  COMPONENT library
          PUBLIC_HEADER DESTINATION include
                  COMPONENT library
          LIBRARY DESTINATION "lib${LIB_SUFFIX}" # .so file
                  COMPONENT library
          ARCHIVE DESTINATION "lib${LIB_SUFFIX}" # .lib file
                  COMPONENT library
          RUNTIME DESTINATION "lib${LIB_SUFFIX}" # .dll file
                  COMPONENT library)
endif()

# static
if(INGESCAPE_BUILD_STATIC)
  add_library(${PROJECT_NAME}-static STATIC ${ingescape_sources})

  # OS specifig target properties
  if(MSVC)
    # WIN32_LIB_FILETYPE is used in the RC file that will register the DLL version
    set(WIN32_LIB_FILETYPE "VFT_STATIC_LIB")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake/modules/library/library_version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
    target_sources(${PROJECT_NAME}-static PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)

    set_target_properties(${PROJECT_NAME}-static
                            PROPERTIES
                              OUTPUT_NAME_RELEASE "ingescape_static"
                              OUTPUT_NAME_DEBUG "ingescaped_static")

  else(MSVC)
    set_target_properties(${PROJECT_NAME}-static
                          PROPERTIES
                            OUTPUT_NAME "ingescape"
                            PREFIX "lib")
  endif(MSVC)

  # Add compilation #defines to the target
  target_compile_definitions(${PROJECT_NAME}-static PRIVATE INGESCAPE=1;TARGET_OS_IOS=0)

  # Add linked dependencies
  #FIXME All private here ?
  target_link_libraries(${PROJECT_NAME}-static
    PRIVATE
      czmq
      ${LIBSODIUM_LIBRARIES}
      libzmq
      zyre
      $<$<BOOL:${APPLE}>:${CORE_FRAMEWORK}>
      $<$<BOOL:${WIN32}>:ws2_32>
  )

  # Header files
  target_include_directories(${PROJECT_NAME}-static
    PUBLIC
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
    PRIVATE
      ${LIBSODIUM_INCLUDE_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/yajl/src/api
      $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/windows/unix>
  )

  # Release build compile with -O2 flag
  if (${CMAKE_BUILD_TYPE} MATCHES "Release")
      target_compile_options(${PROJECT_NAME}-static PRIVATE "-O2")
  endif()

  install(TARGETS ${PROJECT_NAME}-static
          EXPORT ${PROJECT_NAME}-targets
          LIBRARY DESTINATION "lib${LIB_SUFFIX}" # .so file
                  COMPONENT library
          ARCHIVE DESTINATION "lib${LIB_SUFFIX}" # .lib file
                  COMPONENT library
          RUNTIME DESTINATION "lib${LIB_SUFFIX}" # .dll file
                  COMPONENT library)
endif()

# Install public include files
install(FILES ${ingescape_headers_to_install}
        DESTINATION include/ingescape
        COMPONENT library)

# Install headers from libzmq and czmq
#FIXME Exclude from x-compil builds for now due to pipeline errors with CPack
if(NOT CMAKE_TOOLCHAIN_FILE)
  # Variable holding the ZMQ public headers to install with ingescape
  #NB: This variable is manually set from the CMakeLists.txt from ZeroMQ
  get_target_property(ZMQ_PUBLIC_HEADERS_DIR libzmq INTERFACE_INCLUDE_DIRECTORIES)
  set(ZMQ_PUBLIC_HEADERS
    ${ZMQ_PUBLIC_HEADERS_DIR}/zmq.h
    ${ZMQ_PUBLIC_HEADERS_DIR}/zmq_utils.h
  )

  # Variable holding the czmq public headers to install with ingescape
  #NB: This variable is manually set from the CMakeLists.txt from czmq
  get_target_property(CZMQ_PUBLIC_HEADERS_DIR czmq INTERFACE_INCLUDE_DIRECTORIES)
  set (CZMQ_PUBLIC_HEADERS
    ${CZMQ_PUBLIC_HEADERS_DIR}/czmq_library.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/czmq_prelude.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/czmq.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zactor.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zarmour.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zcert.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zcertstore.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zchunk.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zclock.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zconfig.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zdigest.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zdir.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zdir_patch.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zfile.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zframe.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zhash.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zhashx.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/ziflist.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zlist.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zlistx.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zloop.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zmsg.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zpoller.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zsock.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zstr.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zsys.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zuuid.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zauth.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zbeacon.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zgossip.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zmonitor.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zproxy.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zrex.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zgossip_msg.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zsock_option.inc
    ${CZMQ_PUBLIC_HEADERS_DIR}/zgossip_engine.inc
    ${CZMQ_PUBLIC_HEADERS_DIR}/zhash_primes.inc
    ${CZMQ_PUBLIC_HEADERS_DIR}/sha1.inc_c
    ${CZMQ_PUBLIC_HEADERS_DIR}/sha1.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/slre.inc_c
    ${CZMQ_PUBLIC_HEADERS_DIR}/slre.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/readme.txt
    ${CZMQ_PUBLIC_HEADERS_DIR}/zargs.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zproc.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/ztimerset.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/ztrie.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zhttp_client.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zhttp_server.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zhttp_server_options.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zhttp_request.h
    ${CZMQ_PUBLIC_HEADERS_DIR}/zhttp_response.h
  )

  # Variable holding the sodium public headers to install with ingescape
  #NB: This variable is manually set from the sodium installation
  set(SODIUM_PUBLIC_HEADERS
    ${LIBSODIUM_INCLUDE_DIRS}/sodium.h
  )
  set(SODIUM_PUBLIC_HEADERS_SUBDIR
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/core.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_aead_aes256gcm.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_aead_chacha20poly1305.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_aead_xchacha20poly1305.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_auth.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_auth_hmacsha256.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_auth_hmacsha512.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_auth_hmacsha512256.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_box.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_box_curve25519xchacha20poly1305.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_box_curve25519xsalsa20poly1305.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_core_ed25519.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_core_ristretto255.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_core_hchacha20.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_core_hsalsa20.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_core_salsa20.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_core_salsa2012.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_core_salsa208.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_generichash.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_generichash_blake2b.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_hash.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_hash_sha256.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_hash_sha512.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_kdf.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_kdf_blake2b.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_kx.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_onetimeauth.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_onetimeauth_poly1305.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_pwhash.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_pwhash_argon2i.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_pwhash_argon2id.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_pwhash_scryptsalsa208sha256.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_scalarmult.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_scalarmult_curve25519.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_scalarmult_ed25519.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_scalarmult_ristretto255.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_secretbox.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_secretbox_xchacha20poly1305.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_secretbox_xsalsa20poly1305.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_secretstream_xchacha20poly1305.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_shorthash.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_shorthash_siphash24.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_sign.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_sign_ed25519.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_sign_edwards25519sha512batch.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_stream.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_stream_chacha20.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_stream_salsa20.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_stream_salsa2012.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_stream_salsa208.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_stream_xchacha20.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_stream_xsalsa20.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_verify_16.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_verify_32.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/crypto_verify_64.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/export.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/randombytes.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/randombytes_internal_random.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/randombytes_sysrandom.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/runtime.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/utils.h
    ${LIBSODIUM_INCLUDE_DIRS}/sodium/version.h
  )

  # Variable holding the zyre public headers to install with ingescape
  #NB: This variable is manually set from the zyre CMakeLists.txt file
  get_target_property(ZYRE_PUBLIC_HEADERS_DIR zyre INTERFACE_INCLUDE_DIRECTORIES)
  set(ZYRE_PUBLIC_HEADERS
    ${ZYRE_PUBLIC_HEADERS_DIR}/zyre_library.h
    ${ZYRE_PUBLIC_HEADERS_DIR}/zyre.h
    ${ZYRE_PUBLIC_HEADERS_DIR}/zyre_event.h
    ${ZYRE_PUBLIC_HEADERS_DIR}/zre_msg.h
    ${ZYRE_PUBLIC_HEADERS_DIR}/zyre_peer.h
    ${ZYRE_PUBLIC_HEADERS_DIR}/zyre_group.h
    ${ZYRE_PUBLIC_HEADERS_DIR}/zyre_election.h
    ${ZYRE_PUBLIC_HEADERS_DIR}/zyre_node.h
  )

  # Install czmq & ZeroMQ headers since they are in our INTERFACE
  install(
    FILES
      ${SODIUM_PUBLIC_HEADERS}
      ${ZMQ_PUBLIC_HEADERS}
      ${CZMQ_PUBLIC_HEADERS}
      ${ZYRE_PUBLIC_HEADERS}
    DESTINATION
      include
    COMPONENT library
  )
  install(
    FILES
      ${SODIUM_PUBLIC_HEADERS_SUBDIR}
    DESTINATION
      include/sodium
    COMPONENT library
  )

  message(STATUS "ZMQ and CZMQ files installed")
endif(NOT CMAKE_TOOLCHAIN_FILE)

# ##############################################################################
# pkgconfig
# ##############################################################################
set(VERSION "${INGESCAPE_VERSION}")
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
set(libdir "\${prefix}/lib${LIB_SUFFIX}")
set(includedir "\${prefix}/include")
set(pkg_config_defines "")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/libingescape.pc.in"
               "${CMAKE_CURRENT_BINARY_DIR}/libingescape.pc" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libingescape.pc"
        DESTINATION "lib${LIB_SUFFIX}/pkgconfig"
        COMPONENT library)

# ##############################################################################
# installation
# ##############################################################################
include(CMakePackageConfigHelpers)
if (WIN32)
  set(CMAKECONFIG_INSTALL_DIR "CMake" CACHE STRING "install path for ingescapeConfig.cmake")
else()
  set(CMAKECONFIG_INSTALL_DIR "share/cmake/ingescape" CACHE STRING "install path for ingescapeConfig.cmake")
endif(WIN32)

# Export ingescapeTargets.cmake
export(EXPORT ${PROJECT_NAME}-targets FILE "${CMAKE_CURRENT_BINARY_DIR}/ingescapeTargets.cmake")

# Generate ingescapeConfig.cmake
configure_package_config_file(
  builds/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

# Generate ingescapeConfigVersion.cmake
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfigVersion.cmake
  VERSION ${INGESCAPE_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# Install CMake config and target files
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/ingescapeConfigVersion.cmake
  DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(EXPORT ${PROJECT_NAME}-targets
  FILE ingescapeTargets.cmake
  DESTINATION ${CMAKECONFIG_INSTALL_DIR})

# uninstall target
if(NOT TARGET uninstall)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake/cmake_uninstall.cmake.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
                 IMMEDIATE
                 @ONLY)

  add_custom_target(uninstall
                    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

# ##############################################################################
# installer
# ##############################################################################
# Package installer for release build only
if((WIN32) OR (CMAKE_BUILD_TYPE STREQUAL "Release"))
  #https://cmake.org/cmake/help/v3.16/module/CPack.html#
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The ${PROJECT_NAME} core library")
  set(CPACK_PACKAGE_DESCRIPTION "The ${PROJECT_NAME} core library")
  set(CPACK_PACKAGE_HOMEPAGE_URL "https://ingescape.com")
  set(CPACK_PACKAGE_VENDOR "ingenuity.io")
  set(CPACK_PACKAGE_CONTACT "contact@ingescape.com")
  set(CPACK_PACKAGE_VERSION ${INGESCAPE_VERSION})
  set(CPACK_PACKAGE_VERSION_MAJOR "${INGESCAPE_VERSION_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${INGESCAPE_VERSION_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${INGESCAPE_VERSION_PATCH}")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  set(CPACK_STRIP_FILES TRUE)
 
  set(
    CPACK_SOURCE_IGNORE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/bindings/;${CMAKE_CURRENT_SOURCE_DIR}/build/;${CMAKE_CURRENT_SOURCE_DIR}/.git/;${CMAKE_CURRENT_SOURCE_DIR}/doc/;${CMAKE_CURRENT_SOURCE_DIR}/applications/;${CMAKE_CURRENT_SOURCE_DIR}/ingeprobe/;${CMAKE_CURRENT_SOURCE_DIR}/packaging/"
    )

  if(WIN32)
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
    set(CPACK_NSIS_DISPLAY_NAME "The ${PROJECT_NAME} core library")
    set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_NSIS_HELP_LINK "https://ingescape.com")
    set(CPACK_NSIS_CONTACT "contact@ingescape.com")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_URL_INFO_ABOUT ${CMAKE_PROJECT_HOMEPAGE_URL})
    set(CPACK_NSIS_MENU_LINKS ${CMAKE_PROJECT_HOMEPAGE_URL} "IngeScape Web Site")

    set(CPACK_MONOLITHIC_INSTALL ON)

    set(CPACK_GENERATOR "NSIS;ZIP")
    # On windows we want to package these libraries
    install_igs_lib_dependencies()

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/README.txt
         "The bin directory is empty after installation on purpose.\r\n"
         "It will contain agents' executables once installed.")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/README.txt
            DESTINATION bin
            COMPONENT library)

  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # https://cmake.org/cmake/help/git-stage/cpack_gen/productbuild.html
    # Without component productbuild will not package any files
    set(CPACK_COMPONENTS_ALL library)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
    set(CPACK_SYSTEM_NAME "macos")
   
    #set(CPACK_PRODUCTBUILD_IDENTITY_NAME "Developer ID Installer: Ingenuity i/o (56DRT9MG9S)")
    #set(CPACK_PRODUCTBUILD_KEYCHAIN_PATH "$ENV{HOME}/Library/Keychains/login.keychain")
    #set(CPACK_PKGBUILD_IDENTITY_NAME "Developer ID Installer: Ingenuity i/o (56DRT9MG9S)")
    #set(CPACK_PKGBUILD_KEYCHAIN_PATH "$ENV{HOME}/Library/Keychains/login.keychain")

    set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/builds/xcode/WELCOME.txt")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/builds/xcode/README.txt")

    # Without extention productbuild will crash So we just copy License file in
    # build folder with an extension
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
                   "${CMAKE_BINARY_DIR}/LICENSE.txt" COPYONLY)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/LICENSE.txt")
   
    set(CPACK_PREFLIGHT_LIBRARY_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/builds/xcode/preflight.sh")
    set(CPACK_POSTFLIGHT_LIBRARY_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/builds/xcode/postflight.sh")

    set(CPACK_GENERATOR "productbuild")
    #FIXME TODO We will package dependencies within the ingescape.framework. Will be needed when ingescape.framework will be a thing.
    install_igs_lib_dependencies()

  else()
    set(CPACK_MONOLITHIC_INSTALL ON)
    #FIXME TODO Define a strategy for dependencies for linux
    if(ANDROID)
      set(CPACK_GENERATOR "ZIP")
    else()
      install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/unix/install_script.sh"
              DESTINATION "."
              PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
              COMPONENT library)
      set(CPACK_GENERATOR "TGZ")
    endif()
  endif()

  if(MSVC)
    if( ${BITS} EQUAL 64)
      set(CPACK_NSIS_WINDOWS_ARCH "x64")
    else()
      set(CPACK_NSIS_WINDOWS_ARCH "x32")
    endif(${BITS} EQUAL 64)
  endif(MSVC)

  include(CPack)
endif()

# ##############################################################################
# summary
# ##############################################################################
message("")
message(STATUS "******************* Configuration Summary *******************")
message(STATUS "General:")
message(STATUS "  Version           :   ${VERSION}")
message(STATUS "  System            :   ${CMAKE_SYSTEM_NAME}")
message(STATUS "  C compiler        :   ${CMAKE_C_COMPILER}")
message(
  STATUS "  Debug C flags     :   ${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
message(
  STATUS "  Release C flags   :   ${CMAKE_C_FLAGS_RELEASE} ${CMAKE_C_FLAGS}")
message(STATUS "  Build type        :   ${CMAKE_BUILD_TYPE}")
message(STATUS "  Static build      :   ${INGESCAPE_BUILD_STATIC}")
message(STATUS "  Shared build      :   ${INGESCAPE_BUILD_SHARED}")
message(STATUS "")
message(STATUS "Dependencies:")
include(FeatureSummary)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
message(STATUS "")
message(STATUS "Install:")
message(STATUS "  Install prefix    :   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
if(NOT MSVC)
  message(
    STATUS "*************************************************************")
  message(STATUS "Configuration complete! Now procced with:")
  message(STATUS "  'make'                compile the project")
  message(
    STATUS
      "  'make install'        install the project to ${CMAKE_INSTALL_PREFIX}")
  message(
    STATUS
      "  'make uninstall'      uninstall the project from ${CMAKE_INSTALL_PREFIX}"
    )
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "  'make package'        Create library installer")
    message(STATUS "  'make package_source' Create source installer")
  endif()
endif()
message(STATUS "")
