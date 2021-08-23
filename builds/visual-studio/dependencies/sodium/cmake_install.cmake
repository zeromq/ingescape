# Install script for directory: C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/ingescape")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/sodium" TYPE FILE FILES
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/core.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_aead_aes256gcm.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_aead_chacha20poly1305.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_aead_xchacha20poly1305.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_auth.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_auth_hmacsha256.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_auth_hmacsha512.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_auth_hmacsha512256.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_box.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_box_curve25519xchacha20poly1305.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_box_curve25519xsalsa20poly1305.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_core_ed25519.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_core_hchacha20.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_core_hsalsa20.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_core_ristretto255.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_core_salsa20.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_core_salsa2012.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_core_salsa208.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_generichash.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_generichash_blake2b.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_hash.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_hash_sha256.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_hash_sha512.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_kdf.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_kdf_blake2b.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_kx.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_onetimeauth.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_onetimeauth_poly1305.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_pwhash.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_pwhash_argon2i.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_pwhash_argon2id.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_pwhash_scryptsalsa208sha256.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_scalarmult.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_scalarmult_curve25519.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_scalarmult_ed25519.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_scalarmult_ristretto255.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_secretbox.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_secretbox_xchacha20poly1305.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_secretbox_xsalsa20poly1305.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_secretstream_xchacha20poly1305.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_shorthash.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_shorthash_siphash24.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_sign.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_sign_ed25519.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_sign_edwards25519sha512batch.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_stream.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_stream_chacha20.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_stream_salsa20.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_stream_salsa2012.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_stream_salsa208.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_stream_xchacha20.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_stream_xsalsa20.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_verify_16.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_verify_32.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/crypto_verify_64.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/export.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/private/chacha20_ietf_ext.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/private/common.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/private/ed25519_ref10.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/private/ed25519_ref10_fe_25_5.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/private/ed25519_ref10_fe_51.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/private/implementations.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/private/mutex.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/private/sse2_64_32.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/randombytes.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/randombytes_internal_random.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/randombytes_sysrandom.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/runtime.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/utils.h"
    "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium/version.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "C:/Users/Admin/Documents/DEV/work/ingescape/dependencies/sodium/src/libsodium/include/sodium.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/Debug/sodium.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/Release/sodium.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/MinSizeRel/sodium.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/RelWithDebInfo/sodium.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/Debug/libsodium.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/Release/libsodium.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/MinSizeRel/libsodium.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/RelWithDebInfo/libsodium.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/Debug/libsodium.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/Release/libsodium.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/MinSizeRel/libsodium.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/RelWithDebInfo/libsodium.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/CMake/sodiumTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/CMake/sodiumTargets.cmake"
         "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/CMakeFiles/Export/CMake/sodiumTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/CMake/sodiumTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/CMake/sodiumTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CMake" TYPE FILE FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/CMakeFiles/Export/CMake/sodiumTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CMake" TYPE FILE FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/CMakeFiles/Export/CMake/sodiumTargets-debug.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CMake" TYPE FILE FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/CMakeFiles/Export/CMake/sodiumTargets-minsizerel.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CMake" TYPE FILE FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/CMakeFiles/Export/CMake/sodiumTargets-relwithdebinfo.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CMake" TYPE FILE FILES "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/CMakeFiles/Export/CMake/sodiumTargets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/CMake" TYPE FILE FILES
    "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/sodiumConfig.cmake"
    "C:/Users/Admin/Documents/DEV/work/ingescape/builds/visual-studio/dependencies/x64/dependencies/sodium/sodiumConfigVersion.cmake"
    )
endif()

