cmake_minimum_required(VERSION 2.8.9)
project(ingescapeTesting)
set (VERSION "1.0.0")
set (VERSION_MAJOR "1")
set (VERSION_MINOR "0")
set (VERSION_PATCH "0")

set(CMAKE_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULES_DIR})
include(IdentifyOS)

# ##############################################################################
# agent and external dependencies (if any)
# ##############################################################################
add_executable(tester
    src/tester.c
    src/common.c)

add_executable(partner
    src/partner.c
    src/common.c)

if(UNIX AND NOT APPLE)
    #FIXME Is the condition necessary even when using C_STANDARD property?
    set_target_properties(${PROJECT_NAME}
      PROPERTIES
        C_STANDARD 99
    )
endif()

target_include_directories(tester PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src) # local headers
target_include_directories(partner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src) # local headers


if (WIN32)
	target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/windows)
endif (WIN32)

find_package(sodium)
if(SODIUM_FOUND)
  include_directories(${SODIUM_INCLUDE_DIRS})
  target_link_libraries(tester ${SODIUM_LIBRARIES})
  target_link_libraries(partner ${SODIUM_LIBRARIES})
else(SODIUM_FOUND)
  message(FATAL_ERROR "Dependency not found")
endif(SODIUM_FOUND)

find_package(czmq)
if(CZMQ_FOUND)
  include_directories(${CZMQ_INCLUDE_DIRS})
  target_link_libraries(tester ${CZMQ_LIBRARIES})
  target_link_libraries(partner ${CZMQ_LIBRARIES})
else(CZMQ_FOUND)
  message(FATAL_ERROR "Dependency not found")
endif(CZMQ_FOUND)

find_package(zyre)
if(ZYRE_FOUND)
  include_directories(${ZYRE_INCLUDE_DIRS})
  target_link_libraries(tester ${ZYRE_LIBRARIES})
  target_link_libraries(partner ${ZYRE_LIBRARIES})
else(ZYRE_FOUND)
  message(FATAL_ERROR "Dependency not found")
endif(ZYRE_FOUND)

find_package (ingescape 2.0 REQUIRED)
if (ingescape_FOUND)
    target_link_libraries (tester PRIVATE ingescape)
    target_link_libraries (partner PRIVATE ingescape)
else (ingescape_FOUND)
	message(FATAL_ERROR "ingescape - Dependency not found")
endif (ingescape_FOUND)

set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}" )

install (TARGETS tester RUNTIME DESTINATION bin COMPONENT agent)
install (TARGETS partner RUNTIME DESTINATION bin COMPONENT agent)
install(FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/builds/xcode/resources/ingescapetester.cert
	${CMAKE_CURRENT_SOURCE_DIR}/builds/xcode/resources/ingescapetester.cert_secret
	${CMAKE_CURRENT_SOURCE_DIR}/builds/xcode/resources/partner.cert
	${CMAKE_CURRENT_SOURCE_DIR}/builds/xcode/resources/partner.cert_secret
        DESTINATION "bin/certificates"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        COMPONENT agent)

########################################################################
# installer
########################################################################
# Package installer for release build only
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Ingescape agents for testing")
  set(CPACK_PACKAGE_DESCRIPTION "Ingescape agents for testing")
  set(CPACK_PACKAGE_HOMEPAGE_URL "https://ingescape.com")
  set(CPACK_PACKAGE_VENDOR "ingenuity.io")
  set(CPACK_PACKAGE_CONTACT "contact@ingescape.com")
  set(CPACK_PACKAGE_VERSION ${VERSION})
  set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
  set(CPACK_STRIP_FILES TRUE)

  set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/.git/;${CMAKE_CURRENT_SOURCE_DIR}/build/")

  if (WIN32)
    # Installers for 32- vs. 64-bit CMake:
    #  - Root install directory (displayed to end user at installer-run time)
    #  - "NSIS package/display name" (text used in the installer GUI)
    #  - Registry key used to store info about the installation
    if(CMAKE_CL_64)
      set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
      set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION} (Win64)")
      set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} (Win64)")
    else()
      set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
      set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
      set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} (Win32)")
    endif()

    # On windows put all agents in ingescape directory
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "ingescape")

    # NSIS configuration for Windows
    set(CPACK_NSIS_DISPLAY_NAME "Ingescape agents for testing")
    set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_NSIS_HELP_LINK "https://ingescape.com")
    set(CPACK_NSIS_CONTACT "contact@ingescape.com")

    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_URL_INFO_ABOUT ${CMAKE_PROJECT_HOMEPAGE_URL})
    set(CPACK_MONOLITHIC_INSTALL ON)
    set(CPACK_GENERATOR "NSIS;ZIP")

  elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    # Without component productbuild will not package any files
    set (CPACK_COMPONENTS_ALL agent)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
    set(CPACK_SYSTEM_NAME "macos")
    set(CPACK_GENERATOR "productbuild;ZIP")

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${CMAKE_BINARY_DIR}/LICENSE.txt" COPYONLY)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/LICENSE.txt")

  else ()
    set(CPACK_MONOLITHIC_INSTALL ON)
    set(CPACK_GENERATOR "TGZ")
  endif ()

  include (CPack)
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
  endif()
endif()
message(STATUS "")
