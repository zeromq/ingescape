# CMake build script for wrapper test

########################################################################
# Project setup
########################################################################

cmake_minimum_required(VERSION 3.13.1)
project(igstester HOMEPAGE_URL https://ingescape.com LANGUAGES CSharp)

if (WITH_CSHARP_WRAPPER)
	set(FROM_INGESCAPE ON)
endif ()
if (WITH_CSHARP_TEST)
	set(FROM_CSHARP ON)
endif ()

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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DllFromLib)

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

add_library(${PROJECT_NAME} SHARED ${test_sources})

set_property(TARGET ${PROJECT_NAME} PROPERTY
  VS_GLOBAL_TargetFramework "net472"
)
set_property(TARGET ${PROJECT_NAME} PROPERTY
  VS_GLOBAL_EnableDefaultItems false
)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_PACKAGE_REFERENCES
	"MSTest.TestFramework_2.2.10;MSTest.TestAdapter_2.2.10"
)

if (NOT FROM_CSHARP)
	# Find the CSharp library on system
	find_package (IngescapeCSharp)
    if (NOT INGESCAPE_CSHARP_FOUND)
        message(FATAL_ERROR "Dependency not found")
    endif (NOT INGESCAPE_CSHARP_FOUND)
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES
		"Microsoft.CSharp"
		"System"
		"System.Core"
		"${INGESCAPE_CSHARP_LIBRARIES}"
	)
else ()
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES
		"Microsoft.CSharp"
		"System"
		"System.Core"
	)
endif ()

if ((NOT FROM_INGESCAPE) AND FROM_CSHARP)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set(_INGESCAPE_DLL_NAME "ingescaped")
		set(_LIBZMQ_DLL_NAME "libzmq-v142-mt-gd-4_3_5")
	else()
		set(_INGESCAPE_DLL_NAME "ingescape")
		set(_LIBZMQ_DLL_NAME "libzmq-v142-mt-4_3_5")
	endif()
	# From CSharp cmake only. Find LIBS on system
	find_library(SODIUM NAMES sodium REQUIRED)
	find_library(ZMQ NAMES ${_LIBZMQ_DLL_NAME} REQUIRED)
	find_library(CZMQ NAMES czmq REQUIRED)
	find_library(ZYRE NAMES zyre REQUIRED)
	find_library(INGESCAPE NAMES ${_INGESCAPE_DLL_NAME} REQUIRED)	

	dll_from_lib_exists(SODIUM)
	dll_from_lib_exists(ZMQ)
	dll_from_lib_exists(CZMQ)
	dll_from_lib_exists(ZYRE)
	dll_from_lib_exists(INGESCAPE)

	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${${_INGESCAPE_DLL_NAME}_DLL_FILE}" "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/net472/"
		COMMAND ${CMAKE_COMMAND} -E echo "Copying ${${_INGESCAPE_DLL_NAME}_DLL_FILE} library"
		COMMAND ${CMAKE_COMMAND} -E copy "${sodium_DLL_FILE}" "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/net472/"
		COMMAND ${CMAKE_COMMAND} -E echo "Copying ${sodium_DLL_FILE} library"
		COMMAND ${CMAKE_COMMAND} -E copy "${${_LIBZMQ_DLL_NAME}_DLL_FILE}" "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/net472/"
		COMMAND ${CMAKE_COMMAND} -E echo "Copying ${${_LIBZMQ_DLL_NAME}_DLL_FILE} library"
		COMMAND ${CMAKE_COMMAND} -E copy "${czmq_DLL_FILE}" "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/net472/"
		COMMAND ${CMAKE_COMMAND} -E echo "Copying ${czmq_DLL_FILE} library"
		COMMAND ${CMAKE_COMMAND} -E copy "${zyre_DLL_FILE}" "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/net472/"
		COMMAND ${CMAKE_COMMAND} -E echo "Copying ${zyre_DLL_FILE} library"
	)
endif ()