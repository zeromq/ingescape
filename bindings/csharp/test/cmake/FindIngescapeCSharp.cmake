if (WIN32)
	if(CMAKE_CL_64)
		if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
			set(_PATH_CSHARP "${CMAKE_CURRENT_SOURCE_DIR}/../build/DebugX64/Debug/netstandard2.0")
		else()
			set(_PATH_CSHARP "${CMAKE_CURRENT_SOURCE_DIR}/../build/ReleaseX64/Release/netstandard2.0")
		endif()
	else()
		if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
			set(_PATH_CSHARP "${CMAKE_CURRENT_SOURCE_DIR}/../build/DebugX86/Debug/netstandard2.0")
		else()
			set(_PATH_CSHARP "${CMAKE_CURRENT_SOURCE_DIR}/../build/ReleaseX86/Release/netstandard2.0")
		endif()
	endif()
endif (WIN32)

message(STATUS "${_PATH_CSHARP}")

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
mark_as_advanced(
    INGESCAPE_CSHARP_FOUND
    INGESCAPE_CSHARP_LIBRARIES
)
