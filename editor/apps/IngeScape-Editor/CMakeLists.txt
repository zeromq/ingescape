# CMake build script for IngeScape-Editor

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
project(IngeScape-Editor HOMEPAGE_URL https://ingescape.com)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
include(IngescapeHelper)

########################################################################
# options
########################################################################
# Default build will be debug
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif ()

add_definitions(-DQT_DEPRECATED_WARNINGS=1)
add_definitions(-DINGESCAPE=1)
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
get_ingescape_version(INGESCAPE_EDITOR_VERSION_MAJOR INGESCAPE_EDITOR_VERSION_MINOR INGESCAPE_EDITOR_VERSION_PATCH)

set(INGESCAPE_EDITOR_VERSION "${INGESCAPE_EDITOR_VERSION_MAJOR}.${INGESCAPE_EDITOR_VERSION_MINOR}.${INGESCAPE_EDITOR_VERSION_PATCH}")
message(STATUS "Detected INGESCAPE Editor Version - ${INGESCAPE_EDITOR_VERSION}")

########################################################################
# includes
########################################################################
set (ingescape_editor_headers
    stable.h
    uthash/libut.h
    uthash/ringbuf.h
    uthash/utarray.h
    uthash/uthash.h
    uthash/utlist.h
    uthash/utmm.h
    uthash/utringbuffer.h
    uthash/utstring.h
    uthash/utvector.h
    controller/agentssupervisioncontroller.h
    controller/agentsmappingcontroller.h
    controller/ingescapeeditorcontroller.h
    controller/ingescapemodelmanager.h
    controller/networkcontroller.h
    controller/scenariocontroller.h
    controller/actioneditorcontroller.h
    controller/valueshistorycontroller.h
    controller/abstracttimeactionslinescenarioviewcontroller.h
    controller/ingescapelaunchermanager.h
    misc/ingescapeeditorsettings.h
    misc/ingescapeeditorutils.h
    misc/terminationsignalwatcher.h
    misc/collapsiblecolumn.h
    model/agentm.h
    model/iop/agentiopm.h
    model/iop/outputm.h
    model/jsonhelper.h
    model/definitionm.h
    model/mapping/agentmappingm.h
    model/mapping/elementmappingm.h
    model/scenario/condition/actionconditionm.h
    model/scenario/actionm.h
    model/scenario/scenariom.h
    model/scenario/scenariomarkerm.h
    model/scenario/condition/iopvalueconditionm.h
    model/enums.h
    model/publishedvaluem.h
    model/scenario/timetickm.h
    model/scenario/effect/actioneffectm.h
    model/scenario/effect/iopvalueeffectm.h
    model/scenario/effect/mappingeffectm.h
    model/scenario/effect/effectonagentm.h
    sortFilter/valueshistorysortfilter.h
    viewModel/agentvm.h
    viewModel/pointmapvm.h
    viewModel/agentinmappingvm.h
    viewModel/mapbetweeniopvm.h
    viewModel/scenario/actionvm.h
    viewModel/iop/inputvm.h
    viewModel/iop/outputvm.h
    viewModel/scenario/actionconditionvm.h
    viewModel/scenario/actioneffectvm.h
    viewModel/scenario/actioninpalettevm.h
    viewModel/scenario/actionexecutionvm.h
    viewModel/iop/parametervm.h
    model/hostm.h
    viewModel/iop/agentiopvm.h
    sortFilter/abstracttimerangefilter.h
    controller/hostssupervisioncontroller.h
    viewModel/hostvm.h
    model/recordm.h
    controller/recordssupervisioncontroller.h
    viewModel/recordvm.h
    controller/logstreamcontroller.h
    model/logm.h
    sortFilter/logssortfilter.h
    model/scenario/condition/conditiononagentm.h
    misc/qquickwindowblocktouches.h
    misc/textfielddoublevalidator.h
)

source_group ("Header Files" FILES ${ingescape_editor_headers})

add_ingescape_include_directory(_INCLUDES_DIRECTORY)

list(APPEND _INCLUDES_DIRECTORY 
    "${SOURCE_DIR}"
    "${SOURCE_DIR}/../../frameworks/I2Quick/include"
)

include_directories(${_INCLUDES_DIRECTORY})

########################################################################
# Sources files
########################################################################

set (ingescape_editor_sources
    main.cpp
    controller/agentssupervisioncontroller.cpp
    controller/agentsmappingcontroller.cpp
    controller/ingescapeeditorcontroller.cpp
    controller/ingescapemodelmanager.cpp
    controller/networkcontroller.cpp
    controller/scenariocontroller.cpp
    controller/actioneditorcontroller.cpp
    controller/valueshistorycontroller.cpp
    controller/abstracttimeactionslinescenarioviewcontroller.cpp
    controller/ingescapelaunchermanager.cpp
    misc/ingescapeeditorsettings.cpp
    misc/ingescapeeditorutils.cpp
    misc/terminationsignalwatcher.cpp
    misc/collapsiblecolumn.cpp
    model/agentm.cpp
    model/iop/agentiopm.cpp
    model/iop/outputm.cpp
    model/jsonhelper.cpp
    model/definitionm.cpp
    model/mapping/agentmappingm.cpp
    model/mapping/elementmappingm.cpp
    model/scenario/condition/actionconditionm.cpp
    model/scenario/actionm.cpp
    model/scenario/scenariom.cpp
    model/scenario/scenariomarkerm.cpp
    model/scenario/condition/iopvalueconditionm.cpp
    model/enums.cpp
    model/publishedvaluem.cpp
    model/scenario/timetickm.cpp
    model/scenario/effect/actioneffectm.cpp
    model/scenario/effect/iopvalueeffectm.cpp
    model/scenario/effect/mappingeffectm.cpp
    model/scenario/effect/effectonagentm.cpp
    sortFilter/valueshistorysortfilter.cpp
    viewModel/agentvm.cpp
    viewModel/pointmapvm.cpp
    viewModel/agentinmappingvm.cpp
    viewModel/mapbetweeniopvm.cpp
    viewModel/scenario/actionvm.cpp
    viewModel/iop/inputvm.cpp
    viewModel/iop/outputvm.cpp
    viewModel/scenario/actionconditionvm.cpp
    viewModel/scenario/actioneffectvm.cpp
    viewModel/scenario/actioninpalettevm.cpp
    viewModel/scenario/actionexecutionvm.cpp
    viewModel/iop/parametervm.cpp
    model/hostm.cpp
    viewModel/iop/agentiopvm.cpp
    sortFilter/abstracttimerangefilter.cpp
    controller/hostssupervisioncontroller.cpp
    viewModel/hostvm.cpp
    model/recordm.cpp
    controller/recordssupervisioncontroller.cpp
    viewModel/recordvm.cpp
    controller/logstreamcontroller.cpp
    model/logm.cpp
    sortFilter/logssortfilter.cpp
    model/scenario/condition/conditiononagentm.cpp
    misc/qquickwindowblocktouches.cpp
    misc/textfielddoublevalidator.cpp
)

if(MSVC)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/stable.cpp "#include <stable.h>")
    ADD_MSVC_PRECOMPILED_HEADER("stable.h" ${CMAKE_CURRENT_BINARY_DIR}/stable.cpp ingescape_editor_sources)
endif()

add_ingescape_sources(ingescape_editor_sources)

source_group("Source Files" FILES ${ingescape_editor_sources})

set (ingescape_editor_ressources qml.qrc)

########################################################################
# dependencies
########################################################################
# Define Qt cmake modules path if not already done
if (WIN32 AND DEFINED ENV{Qt5_DIR} AND NOT DEFINED Qt5_DIR)
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
add_ingescape_libraries_dependencies(_INDESCAPE_LIBS pkg_config_libs_private)

# Find I2 Quick dependencies
add_library(I2Quick SHARED IMPORTED)
if (APPLE)
    set(I2QUICK_LIB "${SOURCE_DIR}/../../frameworks/I2Quick/Mac/libI2Quick.dylib")
    set_source_files_properties("${I2QUICK_LIB}" PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)
    set_target_properties(I2Quick PROPERTIES IMPORTED_LOCATION "${I2QUICK_LIB}")
elseif (WIN32)
    set(I2QUICK_LIB "${SOURCE_DIR}/../../frameworks/I2Quick/Win32/I2Quick.lib")
    set_target_properties(I2Quick PROPERTIES IMPORTED_IMPLIB "${I2QUICK_LIB}")
    set_target_properties(I2Quick PROPERTIES IMPORTED_LOCATION "${SOURCE_DIR}/../../frameworks/I2Quick/Win32/I2Quick.dll")
else ()
    #TODO Add so for unix
endif ()

########################################################################
# executable
########################################################################
set(INGESCAPE_EDITOR_ICON "${SOURCE_DIR}/icon_IGS.icns")
if (APPLE)
    set_source_files_properties("${INGESCAPE_EDITOR_ICON}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif ()

# Create target
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    ${ingescape_editor_sources}
    ${ingescape_editor_ressources}
    ${INGESCAPE_EDITOR_ICON}
    ${I2QUICK_LIB}
)

target_link_libraries(
    ${PROJECT_NAME}
    Qt5::Qml
    Qt5::Quick
    Qt5::Svg
    Qt5::Xml
    Qt5::Concurrent
    Qt5::Sql
    Qt5::Core
    Qt5::Gui
    ${_INDESCAPE_LIBS}
    I2Quick
)

# Retrieve the absolute path to qmake and then use that path to find
# the binaries
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE YES)
    
    # Mac OS X bundle specific settings
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${INGESCAPE_EDITOR_VERSION}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${INGESCAPE_EDITOR_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${INGESCAPE_EDITOR_VERSION_MAJOR}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${INGESCAPE_EDITOR_VERSION}")
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
    #TODO Is needed ?
    #include(InstallRequiredSystemLibraries)

    if(MSVC) # Check if we are using the Visual Studio compiler
        # This line avoid console windows that appears along with the GUI program
        set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE YES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif(MSVC)

    # Copy I2Quick dependency
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${SOURCE_DIR}/../../frameworks/I2Quick/Win32/I2Quick.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
	
	# Copy Qt dependencies
	find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}" \"$<TARGET_FILE:${PROJECT_NAME}>\" -xml -concurrent -printsupport -sql -qmldir="${SOURCE_DIR}"
		COMMENT "Deploying Qt..."
	)
else ()
    #TODO Add unix target
endif ()

########################################################################
# installer
########################################################################
# Package installer just for release build
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION .)

    set(CPACK_MONOLITHIC_INSTALL ON)
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VENDOR "Ingenuity IO")
    set(CPACK_PACKAGE_CONTACT "contact@ingenuity.io")
    set(CPACK_PACKAGE_VERSION ${INGESCAPE_EDITOR_VERSION})
    set(CPACK_PACKAGE_VERSION_MAJOR "${INGESCAPE_EDITOR_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${INGESCAPE_EDITOR_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${INGESCAPE_EDITOR_VERSION_PATCH}")
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

    set(CPACK_SOURCE_IGNORE_FILES "${SOURCE_DIR}/build/;${SOURCE_DIR}/IngeScape-Editor.pro.user")

    if (WIN32)
        set(CPACK_GENERATOR "NSIS;ZIP")
        set(CPACK_SOURCE_GENERATOR "ZIP")
        # On windows we want to package dependencies with this library
        install_ingescape_dependencies(LIBSODIUM_LIBRARIES LIBSODIUM_INCLUDE_DIRS)
        install_ingescape_dependencies(LIBZMQ_LIBRARIES LIBZMQ_INCLUDE_DIRS)
        install_ingescape_dependencies(CZMQ_LIBRARIES CZMQ_INCLUDE_DIRS)
        install_ingescape_dependencies(ZYRE_LIBRARIES ZYRE_INCLUDE_DIRS)
        install_ingescape_dependencies(YAJL_LIBRARIES YAJL_INCLUDE_DIRS)
    elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
        set(CPACK_GENERATOR "DragNDrop")
        set(CPACK_SOURCE_GENERATOR "TGZ")
    else ()
        #TODO Add unix target
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
