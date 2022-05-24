# CMake build script for wrapper test

########################################################################
# Project setup
########################################################################

cmake_minimum_required(VERSION 3.13.1)
project(igstester HOMEPAGE_URL https://ingescape.com LANGUAGES CSharp)

if (WITH_CSHARP_WRAPPER)
	set(FROM_INGESCAPE ON)
endif ()
if (WITH_TEST)
	set(FROM_CSHARP ON)
endif ()

# Little hack because without this line, CMAKE_CXX_SIZEOF_VOID_P is not initialised
# And installer will alwayse end with win32, not win64
#enable_language(CXX)

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
	# From CSharp cmake only. Find LIBS on system
	find_library(SODIUM NAMES sodium REQUIRED)
	find_library(ZMQ NAMES libzmq-v142-mt-4_3_5 REQUIRED)
	find_library(CZMQ NAMES czmq REQUIRED)
	find_library(ZYRE NAMES zyre REQUIRED)
	find_library(INGESCAPE NAMES ingescape REQUIRED)	

	dll_from_lib_exists(SODIUM)
	dll_from_lib_exists(ZMQ)
	dll_from_lib_exists(CZMQ)
	dll_from_lib_exists(ZYRE)
	dll_from_lib_exists(INGESCAPE)

	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${ingescape_DLL_FILE} ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/net472/
		COMMAND ${CMAKE_COMMAND} -E echo "Copying ingescape library"
		COMMAND ${CMAKE_COMMAND} -E copy ${sodium_DLL_FILE} ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/net472/
		COMMAND ${CMAKE_COMMAND} -E echo "Copying sodium library"
		COMMAND ${CMAKE_COMMAND} -E copy ${libzmq-v142-mt-4_3_5_DLL_FILE} ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/net472/
		COMMAND ${CMAKE_COMMAND} -E echo "Copying zmq library"
		COMMAND ${CMAKE_COMMAND} -E copy ${czmq_DLL_FILE} ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/net472/
		COMMAND ${CMAKE_COMMAND} -E echo "Copying czmq library"
		COMMAND ${CMAKE_COMMAND} -E copy ${zyre_DLL_FILE} ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/net472/
		COMMAND ${CMAKE_COMMAND} -E echo "Copying zyre library"
	)
endif ()

if (FROM_INGESCAPE AND FROM_CSHARP)
	#From CSharp and Ingescape CMakes.

	macro(print_all_variables)
		message(STATUS "print_all_variables------------------------------------------{")
		get_cmake_property(_variableNames VARIABLES)
		foreach (_variableName ${_variableNames})
			message(STATUS "${_variableName}=${${_variableName}}")
		endforeach()
		message(STATUS "print_all_variables------------------------------------------}")
	endmacro()

	#print_all_variables()

	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set(_INGESCAPE_DLL_NAME "ingescaped")
		set(_LIBZMQ_DLL_NAME "libzmq-v142-mt-gd-4_3_5")
	else()
		set(_INGESCAPE_DLL_NAME "ingescape")
		set(_LIBZMQ_DLL_NAME "libzmq-v142-mt-4_3_5")
	endif()
	
	message("TEST : ${CMAKE_PROJECT_NAME}")
	message("BINARY DIR : ${CMAKE_BINARY_DIR}")

	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${_INGESCAPE_DLL_NAME}.dll "./${_INGESCAPE_DLL_NAME}.dll"
		COMMENT "Copie d'ingescape"
		VERBATIM
	)

	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/dependencies/sodium/${CMAKE_BUILD_TYPE}/libsodium.dll "./libsodium.dll"
		COMMENT "Copie de sodium"
		VERBATIM
	)

	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/dependencies/libzmq/bin/${CMAKE_BUILD_TYPE}/${_LIBZMQ_DLL_NAME}.dll "./${_LIBZMQ_DLL_NAME}.dll"
		COMMENT "Copie de zmq"
		VERBATIM
	)

	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/dependencies/czmq/${CMAKE_BUILD_TYPE}/libczmq.dll "./libczmq.dll"
		COMMENT "Copie de czmq"
		VERBATIM
	)

	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/dependencies/zyre/${CMAKE_BUILD_TYPE}/libzyre.dll "./libzyre.dll"
		COMMENT "Copie de zyre"
		VERBATIM
	)

endif ()