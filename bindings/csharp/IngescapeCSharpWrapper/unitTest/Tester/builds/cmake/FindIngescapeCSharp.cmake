if (WIN32)
	# Check 32 or 64 bit
	# CMAKE_SIZEOF_VOID_P EQUAL 8 and CMAKE_CL_64 check current platform and no target
	if ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
		# target is 64 bit
		set(_MY_PROGRAM_FILES_PATH "ProgramW6432")
	else ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
		# target is 32 bit
		set(_MY_PROGRAM_FILES_PATH "ProgramFiles(x86)")
	endif ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
	set(PC_INGESCAPE_LIBRARY_HINTS "$ENV{${_MY_PROGRAM_FILES_PATH}}\\ingescape\\lib\\")
endif (WIN32)

# On windows find library search .dll and .lib but csharp binding do not produce .lib
find_file (
    INGESCAPE_CSHARP_LIBRARIES
    NAME IngescapeCSharp${CMAKE_SHARED_LIBRARY_SUFFIX}
    HINTS ${PC_INGESCAPE_LIBRARY_HINTS}
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
