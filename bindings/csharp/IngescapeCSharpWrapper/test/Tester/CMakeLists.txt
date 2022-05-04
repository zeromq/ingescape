# CMake build script for wrapper test

########################################################################
# Project setup
########################################################################

cmake_minimum_required(VERSION 3.13.1)
project(Tester HOMEPAGE_URL https://ingescape.com LANGUAGES CSharp)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/Main.cs
)

source_group("Source Files" FILES ${test_sources})

########################################################################
# Library
########################################################################
set(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")

add_library(${PROJECT_NAME} SHARED ${test_sources})

#find_package (IngescapeCSharp)
#if (NOT INGESCAPE_CSHARP_FOUND)
#	message(FATAL_ERROR "Dependency not found")
#endif (NOT INGESCAPE_CSHARP_FOUND)

if (WIN32)
	if(CMAKE_CL_64)
		if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
			set(_PATH_CSHARP "${CMAKE_CURRENT_SOURCE_DIR}/../../IngescapeCSharp/build/DebugX64/Debug/netstandard2.0/IngescapeCSharp.dll")
		else()
			set(_PATH_CSHARP "${CMAKE_CURRENT_SOURCE_DIR}/../../IngescapeCSharp/build/ReleaseX64/Release/netstandard2.0/IngescapeCSharp.dll")
	else ()
		if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
			set(_PATH_CSHARP "${CMAKE_CURRENT_SOURCE_DIR}/../../IngescapeCSharp/build/DebugX86/Debug/netstandard2.0/IngescapeCSharp.dll")
		else()
			set(_PATH_CSHARP "${CMAKE_CURRENT_SOURCE_DIR}/../../IngescapeCSharp/build/ReleaseX86/Release/netstandard2.0/IngescapeCSharp.dll")
endif (WIN32)

# On windows find library search .dll and .lib but csharp binding do not produce .lib
find_file (
    INGESCAPE_CSHARP_LIBRARIES
    NAME IngescapeCSharp${CMAKE_SHARED_LIBRARY_SUFFIX}
    HINTS ${_PATH_CSHARP}
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    INGESCAPE_CSHARP
    REQUIRED_VARS INGESCAPE_CSHARP_LIBRARIES
)

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES
	"Microsoft.CSharp"
	"System"
	"System.Core"
	${INGESCAPE_CSHARP_LIBRARIES}
	"${CMAKE_CURRENT_SOURCE_DIR}/../packages/MSTest.TestFramework.1.3.2/lib/net45/Microsoft.VisualStudio.TestPlatform.TestFramework.dll"
	"${CMAKE_CURRENT_SOURCE_DIR}/../packages/MSTest.TestFramework.1.3.2/lib/net45/Microsoft.VisualStudio.TestPlatform.TestFramework.Extensions.dll"
	"${CMAKE_CURRENT_SOURCE_DIR}/../packages/MSTest.TestAdapter.1.3.2/build/_common/Microsoft.VisualStudio.TestPlatform.MSTest.TestAdapter.dll"
	"${CMAKE_CURRENT_SOURCE_DIR}/../packages/MSTest.TestAdapter.1.3.2/build/_common/Microsoft.VisualStudio.TestPlatform.MSTestAdapter.PlatformServices.dll"
	"${CMAKE_CURRENT_SOURCE_DIR}/../packages/MSTest.TestAdapter.1.3.2/build/_common/Microsoft.VisualStudio.TestPlatform.MSTestAdapter.PlatformServices.Interface.dll"
)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_PACKAGE_REFERENCES
   	"MSTest.TestAdapter_1.3.2"
)


