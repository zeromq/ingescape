message(STATUS "Package configuration (RPM)")
set(BASE_PACKAGE_NAME ${PROJECT_NAME})
option(RPM_NO_STRIP "Do not strip binaries when generating the .rpm package" OFF)
# these are cache variables, so they could be overwritten with -D,
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "Debug build, binaries with debug symbols will be used for .rpm package")
    set(CPACK_PACKAGE_NAME ${BASE_PACKAGE_NAME}-debug-dev CACHE STRING "The resulting package name")
    set(CPACK_RPM_PACKAGE_CONFLICTS "${BASE_PACKAGE_NAME}-dev, ${BASE_PACKAGE_NAME}-no-strip-dev")
elseif (${RPM_NO_STRIP})
    message(STATUS "RPM_NO_STRIP activated, binaries won't be stripped of symbols when generating the .rpm package")
    set(CPACK_PACKAGE_NAME ${BASE_PACKAGE_NAME}-no-strip-dev CACHE STRING "The resulting package name")
    set(CPACK_RPM_PACKAGE_CONFLICTS "${BASE_PACKAGE_NAME}-dev, ${BASE_PACKAGE_NAME}-debug-dev")
else()
    message(STATUS "Binaries will be stripped of debug symbols.")
    message(STATUS "  Set -DCMAKE_BUILD_TYPE=Debug to create a package with debug symbols.")
    message(STATUS "  Set -DRPM_NO_STRIP=ON to have release binaries not stripped when generating the .rpm package.")
    set(CPACK_PACKAGE_NAME ${BASE_PACKAGE_NAME}-dev CACHE STRING "The resulting package name")
    set(CPACK_RPM_PACKAGE_CONFLICTS "${BASE_PACKAGE_NAME}-debug-dev, ${BASE_PACKAGE_NAME}-no-strip-dev")
endif()

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Model-based framework for broker-free distributed software environments - development package" CACHE STRING "Package description summary")
set(CPACK_PACKAGE_VENDOR "Ingescape")

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/_packages")

set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_CONTACT "github@ingescape.com")
set(CPACK_RPM_PACKAGE_LICENSE "MPL-2.0")
set(CPACK_RPM_PACKAGE_URL "https://ingescape.com")

set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
        DESTINATION share/doc/${CPACK_PACKAGE_NAME}
        RENAME copyright)

# Strip binaries of debugging names and symbols
if ((${CMAKE_BUILD_TYPE} STREQUAL "Debug") OR ${RPM_NO_STRIP})
    message(STATUS "Binaries will NOT be stripped when generating .rpm package")
    set(CPACK_STRIP_FILES NO)
else()
    message(STATUS "Binaries will be stripped when generating .rpm package")
    set(CPACK_STRIP_FILES YES)
endif()

# Automatic dependency detection (equivalent to CPACK_DEBIAN_PACKAGE_SHLIBDEPS)
set(CPACK_RPM_PACKAGE_AUTOREQPROV YES)

# package name for rpm — RPM-DEFAULT produces the standard
# <name>-<version>-<release>.<arch>.rpm naming
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_RPM_COMPONENT_INSTALL YES)

message(STATUS "Components to pack: ${CPACK_COMPONENTS_ALL}")

include(CPack)
