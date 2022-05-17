# CMake build script for wrapper test

########################################################################
# Project setup
########################################################################

cmake_minimum_required(VERSION 3.13.1)
project(igstester HOMEPAGE_URL https://ingescape.com LANGUAGES CSharp)

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

set(GVR_CMAKE_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake")
list(APPEND CMAKE_MODULE_PATH ${GVR_CMAKE_MODULES_DIR})

########################################################################
# sources files
########################################################################

set (test_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cs
)

source_group("Source Files" FILES ${test_sources})

########################################################################
# Library
########################################################################
set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")

add_library(${PROJECT_NAME} SHARED ${test_sources})

find_package (IngescapeCSharp)
if (NOT INGESCAPE_CSHARP_FOUND)
	message(FATAL_ERROR "Dependency not found")
endif (NOT INGESCAPE_CSHARP_FOUND)


set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES
	"Microsoft.CSharp"
	"System"
	"System.Core"
	${INGESCAPE_CSHARP_LIBRARIES}
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES VS_PACKAGE_REFERENCES
	"MSTest.TestFramework_2.2.10;MSTest.TestAdapter_2.2.10"
)

if (WIN32)
	if(CMAKE_CL_64)
		if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
			set(_INGESCAPE_ROOT_FOLDER_NAME "DebugX64")
			set(_INGESCAPE_SUFFIX "d")
			set(_LIBZMQ_NAME "libzmq-v142-mt-gd-4_3_5")
		else()
			set(_INGESCAPE_ROOT_FOLDER_NAME "ReleaseX64")
			set(_INGESCAPE_SUFFIX "")
			set(_LIBZMQ_NAME "libzmq-v142-mt-4_3_5")
		endif()
	else()
		if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
			set(_INGESCAPE_ROOT_FOLDER_NAME "DebugX86")
			set(_INGESCAPE_SUFFIX "d")
			set(_LIBZMQ_NAME "libzmq-v142-mt-gd-4_3_5")
		else()
			set(_INGESCAPE_ROOT_FOLDER_NAME "ReleaseX86")
			set(_INGESCAPE_SUFFIX "")
			set(_LIBZMQ_NAME "libzmq-v142-mt-4_3_5")
		endif()
	endif()
endif (WIN32)

#import .pdb in debug
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
add_custom_target(COPY_INGESCAPE_DEBUG ALL
	#ingescape
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}/ingescaped.pdb" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	#czmq
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/czmq/${CMAKE_BUILD_TYPE}/libczmq.pdb" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	#zmq
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/libzmq/bin/${CMAKE_BUILD_TYPE}/libzmq-v142-mt-gd-4_3_5.dll" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	#sodium
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/sodium/${CMAKE_BUILD_TYPE}/libsodium.pdb" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	#zyre
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/zyre/${CMAKE_BUILD_TYPE}/libzyre.pdb" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	DEPENDS ${PROJECT_NAME}
)
endif()

add_custom_target(COPY_INGESCAPE ALL
	#ingescape
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}/ingescape${_INGESCAPE_SUFFIX}.dll" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}/ingescape${_INGESCAPE_SUFFIX}.lib" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}/ingescape${_INGESCAPE_SUFFIX}.exp" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	#czmq
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/czmq/${CMAKE_BUILD_TYPE}/libczmq.dll" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/czmq/${CMAKE_BUILD_TYPE}/libczmq.lib" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/czmq/${CMAKE_BUILD_TYPE}/czmq.lib" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/czmq/${CMAKE_BUILD_TYPE}/czmq.exp" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	#zmq
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/libzmq/bin/${CMAKE_BUILD_TYPE}/${_LIBZMQ_NAME}.dll" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	#sodium
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/sodium/${CMAKE_BUILD_TYPE}/libsodium.dll" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/sodium/${CMAKE_BUILD_TYPE}/libsodium.lib" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/sodium/${CMAKE_BUILD_TYPE}/sodium.lib" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/sodium/${CMAKE_BUILD_TYPE}/sodium.exp" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	#zyre
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/zyre/${CMAKE_BUILD_TYPE}/libzyre.dll" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/zyre/${CMAKE_BUILD_TYPE}/libzyre.lib" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/zyre/${CMAKE_BUILD_TYPE}/zyre.lib" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/${_INGESCAPE_ROOT_FOLDER_NAME}/dependencies/zyre/${CMAKE_BUILD_TYPE}/zyre.exp" "${CMAKE_CURRENT_SOURCE_DIR}/build/${_INGESCAPE_ROOT_FOLDER_NAME}/${CMAKE_BUILD_TYPE}"
	DEPENDS ${PROJECT_NAME}
)



