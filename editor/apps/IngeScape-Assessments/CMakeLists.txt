# CMake build script for IngeScape-Assessments

# Macro for windows precompiled header
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.13.1)
project(IngeScape-Assessments HOMEPAGE_URL https://ingescape.com)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# Select flags
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Instruct CMake to run rcc automatically when needed
set(CMAKE_AUTORCC ON)
# Add path to custom macro
set(INGESCAPE_CMAKE_MODULES_DIR "${SOURCE_DIR}/../../../builds/cmake/modules")
list(APPEND CMAKE_MODULE_PATH ${INGESCAPE_CMAKE_MODULES_DIR})
list(APPEND CMAKE_MODULE_PATH "${SOURCE_DIR}/../../../builds/cmake/modules/apps") # NSIS templates
include(IngescapeHelper)
include(IngescapeEditorHelper)

########################################################################
# options
########################################################################
# Default build will be debug
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif ()

add_definitions(-DQT_DEPRECATED_WARNINGS=1)
add_definitions(-DINGESCAPE_FROM_PRI=1)
add_definitions(-DWIN32_LEAN_AND_MEAN) # To avoid include of winsock.h by windows.h

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10")

########################################################################
# plateform specific
########################################################################
if (NOT MSVC)
	include(CheckCXXCompilerFlag)
    include(CheckCCompilerFlag)
	foreach(_OPT -fPIC -pipe -pedantic -Wall -Wextra -W -Wpointer-arith -Wwrite-strings -Wunused -Wshadow -Winline -Wnested-externs -Wno-long-long -Wfloat-equal -Wno-multichar -Wsign-compare -Wundef -Wno-format-nonliteral -Wendif-labels -Wstrict-prototypes -Wstrict-aliasing=3 -Wcast-align -Wtype-limits -Wold-style-declaration -Wmissing-parameter-type -Wempty-body -Wclobbered -Wignored-qualifiers -Wconversion -Wno-sign-conversion -Wvla -Wdouble-promotion -Wno-system-headers -Wno-pedantic-ms-format)
		# surprisingly, CHECK_C_COMPILER_FLAG needs a new variable to store each new
		# test result in.
		check_cxx_compiler_flag(${_OPT} OPT${_OPT}_CXX)
		if(OPT${_OPT}_CXX)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_OPT}")
		endif()
        check_c_compiler_flag(${_OPT} OPT${_OPT}_C)
        if(OPT${_OPT}_C)
          set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_OPT}")
        endif()
	endforeach()
endif()

########################################################################
# version
########################################################################
get_ingescape_assessments_version(INGESCAPE_ASSESSMENTS_VERSION_MAJOR INGESCAPE_ASSESSMENTS_VERSION_MINOR INGESCAPE_ASSESSMENTS_VERSION_PATCH INGESCAPE_ASSESSMENTS_VERSION_BUILD)

set(INGESCAPE_ASSESSMENTS_VERSION "${INGESCAPE_ASSESSMENTS_VERSION_MAJOR}.${INGESCAPE_ASSESSMENTS_VERSION_MINOR}.${INGESCAPE_ASSESSMENTS_VERSION_PATCH}")
if (WIN32)
    add_definitions(-DVERSION="${INGESCAPE_ASSESSMENTS_VERSION}.${INGESCAPE_ASSESSMENTS_VERSION_BUILD}")
else (WIN32)
    add_definitions(-DVERSION="${INGESCAPE_ASSESSMENTS_VERSION}")
endif (WIN32)
message(STATUS "Detected INGESCAPE Assessments Version - ${INGESCAPE_ASSESSMENTS_VERSION}")

########################################################################
# includes
########################################################################
file(GLOB_RECURSE ingescape_assessments_headers "${SOURCE_DIR}/*.h")
add_ingescape_common_headers(ingescape_assessments_headers)

source_group ("Header Files" FILES ${ingescape_assessments_headers})

add_ingescape_include_directory(_INCLUDES_DIRECTORY)
add_ingescape_common_include_directory(_INCLUDES_DIRECTORY)

list(APPEND _INCLUDES_DIRECTORY
    "${SOURCE_DIR}"
    "${SOURCE_DIR}/../../frameworks/I2Quick/include"
    "${SOURCE_DIR}/../../dependencies/cassandra-cpp-driver/include"
    "${SOURCE_DIR}/../../dependencies/cqlexporter"
)

include_directories(${_INCLUDES_DIRECTORY})

########################################################################
# Sources files
########################################################################

# One glob by folder because cmake add cpp file in build folder to check compiler flags
# And linker is not happy because he find two main
file(GLOB_RECURSE ingescape_assessments_sources_controller "${SOURCE_DIR}/controller/*.cpp")
list(APPEND ingescape_assessments_sources ${ingescape_assessments_sources_controller})

file(GLOB_RECURSE ingescape_assessments_sources_model "${SOURCE_DIR}/model/*.cpp")
list(APPEND ingescape_assessments_sources ${ingescape_assessments_sources_model})

file(GLOB_RECURSE ingescape_assessments_sources_view_model "${SOURCE_DIR}/viewModel/*.cpp")
list(APPEND ingescape_assessments_sources ${ingescape_assessments_sources_view_model})

list(APPEND ingescape_assessments_sources "${SOURCE_DIR}/main.cpp")

add_ingescape_common_sources(ingescape_assessments_sources)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/stable.cpp "#include <stable.h>")
ADD_MSVC_PRECOMPILED_HEADER("stable.h" ${CMAKE_CURRENT_BINARY_DIR}/stable.cpp ingescape_assessments_sources)

add_ingescape_sources(ingescape_assessments_sources)

# CQL exporter is written in C and is separated from the rest of the sources to avoid issues with precompiled headers
file(GLOB_RECURSE ingescape_assessments_sources_view_model "${SOURCE_DIR}/../../dependencies/cqlexporter/*.c")
list(APPEND ingescape_assessments_cqlexporter_sources ${ingescape_assessments_sources_view_model})

source_group("Source Files" FILES ${ingescape_assessments_sources} ${ingescape_assessments_cqlexporter_sources})

set (ingescape_assessments_ressources "${SOURCE_DIR}/qml.qrc")
add_ingescape_common_ressources(ingescape_assessments_ressources)

########################################################################
# dependencies
########################################################################
# Define Qt cmake modules path if not already done
if (DEFINED ENV{Qt5_DIR} AND NOT DEFINED Qt5_DIR)
    STRING(REPLACE "\"" "" Qt5_DIR $ENV{Qt5_DIR})
endif ()

# Find QT dependencies
find_package(Qt5 NO_MODULE COMPONENTS
    Qml
    Quick
    Svg
    Xml
    Concurrent
    Sql
    Core
    Gui
    REQUIRED
)


# Find ingescape dependencies
set(pkg_config_libs_private "")
add_ingescape_libraries_dependencies(_INGESCAPE_LIBS pkg_config_libs_private)


# Find I2 Quick dependencies
add_library(I2Quick SHARED IMPORTED)
if (APPLE)
    set(I2QUICK_LIB "${SOURCE_DIR}/../../frameworks/I2Quick/Mac/libI2Quick.dylib")
    set_source_files_properties("${I2QUICK_LIB}" PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)
    set_target_properties(I2Quick PROPERTIES IMPORTED_LOCATION "${I2QUICK_LIB}")
elseif (WIN32)
    # Check 32 or 64 bit
    # CMAKE_SIZEOF_VOID_P EQUAL 8 and CMAKE_CL_64 check current platform and no target
    if ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
        # target is 64 bit
        set(_MY_LIB_FOLDER "Win64")
    else ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
        # target is 32 bit
        set(_MY_LIB_FOLDER "Win32")
    endif ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    set(I2QUICK_LIB "${SOURCE_DIR}/../../frameworks/I2Quick/${_MY_LIB_FOLDER}/I2Quick.lib")
    set_target_properties(I2Quick PROPERTIES IMPORTED_IMPLIB "${I2QUICK_LIB}")
    set_target_properties(I2Quick PROPERTIES IMPORTED_LOCATION "${SOURCE_DIR}/../../frameworks/I2Quick/${_MY_LIB_FOLDER}/I2Quick.dll")
    install(FILES "${SOURCE_DIR}/../../frameworks/I2Quick/${_MY_LIB_FOLDER}/I2Quick.dll" DESTINATION .)
else ()
    #TODO Add so for unix
endif ()


########################################################################
# executable
########################################################################
set(INGESCAPE_ASSESSMENTS_ICON)
if (APPLE)
    set(INGESCAPE_ASSESSMENTS_ICON "${SOURCE_DIR}/icon_IGS.icns")
    set_source_files_properties("${INGESCAPE_ASSESSMENTS_ICON}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
elseif (WIN32)
    set(INGESCAPE_ASSESSMENTS_ICON "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc")
    file(WRITE ${INGESCAPE_ASSESSMENTS_ICON} "IDI_ICON1               ICON    DISCARDABLE     \"icon_IGS.ico\"")
endif ()

# Create target
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
    ${ingescape_assessments_sources}
    ${ingescape_assessments_cqlexporter_sources}
    ${ingescape_assessments_ressources}
    ${INGESCAPE_ASSESSMENTS_ICON}
    ${I2QUICK_LIB}
)

target_link_libraries(
    ${PROJECT_NAME}
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickPrivate
    Qt5::Svg
    Qt5::Xml
    Qt5::Concurrent
    Qt5::Sql
    Qt5::Core
    Qt5::Gui
    ${_INGESCAPE_LIBS}
    I2Quick
)


if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework Foundation -framework AppKit")
endif()

if (WIN32)
    target_link_libraries(${PROJECT_NAME} Wtsapi32)
endif()


# Find Cassandra dependencies
if(UNIX)
  include(ExternalProject)
  set(CASS_SOURCE_DIR ${SOURCE_DIR}/../../dependencies/cassandra-cpp-driver)
  ExternalProject_Add(cassandra_external
    SOURCE_DIR ${CASS_SOURCE_DIR}
    CMAKE_ARGS
        ${GLOBAL_DEFAULT_ARGS}
        ${GLOBAL_THIRDPARTY_LIB_ARGS}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/build/
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    )

    add_dependencies(${PROJECT_NAME} cassandra_external)

    set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")
    set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/include")

    if(APPLE)
        set(CASSANDRA_LIBRARIES "${LIB_DIR}/libcassandra.dylib")
        install(DIRECTORY "${LIB_DIR}" DESTINATION /usr/local PATTERN "*.dylib" )
    else(APPLE)
        set(CASSANDRA_LIBRARIES "${LIB_DIR}/libcassandra.so")
        install(DIRECTORY "${LIB_DIR}" DESTINATION /usr/local PATTERN "*.so" )
    endif(APPLE)
    target_link_libraries(${PROJECT_NAME} ${CASSANDRA_LIBRARIES})
elseif (WIN32)
    add_library(cassandra SHARED IMPORTED)
    if (MSVC)
        math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")
        set_target_properties(cassandra PROPERTIES IMPORTED_IMPLIB "${SOURCE_DIR}/../../dependencies/windows/lib${BITS}/cassandra.lib")
        set_target_properties(cassandra PROPERTIES IMPORTED_LOCATION "${SOURCE_DIR}/../../dependencies/windows/lib${BITS}/cassandra.dll")
        install(DIRECTORY ${SOURCE_DIR}/../../dependencies/windows/lib${BITS}/ DESTINATION lib FILES_MATCHING PATTERN "*.dll")
    endif()
    target_link_libraries(${PROJECT_NAME} cassandra)
else ()
    message(FATAL_ERROR "UNIX or WIN32 not defined, can not create make file")
endif(UNIX)

# Retrieve the absolute path to qmake and then use that path to find
# the binaries
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE YES)

    # Mac OS X bundle specific settings
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${INGESCAPE_ASSESSMENTS_VERSION}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${INGESCAPE_ASSESSMENTS_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${INGESCAPE_ASSESSMENTS_VERSION_MAJOR}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${INGESCAPE_ASSESSMENTS_VERSION}")
    set(MACOSX_BUNDLE_ICON_FILE "icon_IGS")

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change libI2Quick.dylib "@executable_path/../Frameworks/libI2Quick.dylib" $<TARGET_FILE:${PROJECT_NAME}>
    )

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")

        # NB: macdeployqt only runs qmlimportscanner correctly when run from Qt bin directory
        add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND cd ${_qt_bin_dir} && "${MACDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../.." -qmldir="${SOURCE_DIR}" -no-strip
            COMMENT "Deploying Qt..."
        )

        set(CMAKE_SKIP_INSTALL_RPATH TRUE)
    endif ()
elseif (WIN32)
    if(MSVC) # Check if we are using the Visual Studio compiler
        # This line avoid console windows that appears along with the GUI program
        set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE YES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif(MSVC)

    # Copy I2Quick dependency
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SOURCE_DIR}/../../frameworks/I2Quick/${_MY_LIB_FOLDER}/I2Quick.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        include(Windeploy)
        windeployqt(${PROJECT_NAME} ${SOURCE_DIR})
    endif ()
else ()
    #TODO Add unix target
endif ()

########################################################################
# installer
########################################################################
# Package installer just for release build
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    install(TARGETS ${PROJECT_NAME} DESTINATION .)

    set(CPACK_MONOLITHIC_INSTALL ON)
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VENDOR "Ingenuity IO")
    set(CPACK_PACKAGE_CONTACT "contact@ingenuity.io")
    set(CPACK_PACKAGE_VERSION ${INGESCAPE_ASSESSMENTS_VERSION})
    set(CPACK_PACKAGE_VERSION_MAJOR "${INGESCAPE_ASSESSMENTS_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${INGESCAPE_ASSESSMENTS_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${INGESCAPE_ASSESSMENTS_VERSION_PATCH}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

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

    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_URL_INFO_ABOUT ${CMAKE_PROJECT_HOMEPAGE_URL})
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY .)
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN ${PROJECT_NAME}.exe)

    set(CPACK_SOURCE_IGNORE_FILES "${SOURCE_DIR}/build/;${SOURCE_DIR}/IngeScape-Assessments.pro.user")

    if (WIN32)
        set(CPACK_GENERATOR "NSIS;ZIP")
        set(CPACK_SOURCE_GENERATOR "ZIP")
        # On windows we want to package dependencies with this library
        set(IS_ASSESSMENTS TRUE)
        install_ingescape_dependencies(LIBSODIUM_LIBRARIES LIBSODIUM_INCLUDE_DIRS IS_ASSESSMENTS)
        install_ingescape_dependencies(LIBZMQ_LIBRARIES LIBZMQ_INCLUDE_DIRS IS_ASSESSMENTS)
        install_ingescape_dependencies(CZMQ_LIBRARIES CZMQ_INCLUDE_DIRS IS_ASSESSMENTS)
        install_ingescape_dependencies(ZYRE_LIBRARIES ZYRE_INCLUDE_DIRS IS_ASSESSMENTS)
    elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
        set(CPACK_GENERATOR "DragNDrop")
        set(CPACK_SOURCE_GENERATOR "TGZ")
    else ()
        #TODO Add unix target
    endif ()

	set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PROJECT_NAME}")
	set(CPACK_CREATE_DESKTOP_LINKS "${PROJECT_NAME}" "${PROJECT_NAME}")

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
message (STATUS "  CXX compiler      :   ${CMAKE_CXX_COMPILER}")
message (STATUS "  Debug CXX flags   :   ${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS}")
message (STATUS "  Release CXX flags :   ${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS}")
message (STATUS "  Build type        :   ${CMAKE_BUILD_TYPE}")
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
if (CMAKE_BUILD_TYPE STREQUAL "Release")
message (STATUS "  'make package'        Create library installer")
message (STATUS "  'make package_source' Create source installer")
endif ()
message (STATUS "")
endif ()
