if (NOT MSVC)
    include(FindPkgConfig)
    pkg_check_modules(PC_CZMQ "libczmq")
    if (NOT PC_CZMQ_FOUND)
        pkg_check_modules(PC_CZMQ "czmq")
    endif (NOT PC_CZMQ_FOUND)
    if (PC_CZMQ_FOUND)
        # add CFLAGS from pkg-config file, e.g. draft api.
        add_definitions(${PC_CZMQ_CFLAGS} ${PC_CZMQ_CFLAGS_OTHER})
        # some libraries install the headers is a subdirectory of the include dir
        # returned by pkg-config, so use a wildcard match to improve chances of finding
        # headers and SOs.
        set(PC_CZMQ_INCLUDE_HINTS ${PC_CZMQ_INCLUDE_DIRS} ${PC_CZMQ_INCLUDE_DIRS}/*)
        set(PC_CZMQ_LIBRARY_HINTS ${PC_CZMQ_LIBRARY_DIRS} ${PC_CZMQ_LIBRARY_DIRS}/*)
    endif(PC_CZMQ_FOUND)
endif (NOT MSVC)

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
	set(PC_CZMQ_INCLUDE_HINTS "$ENV{${_MY_PROGRAM_FILES_PATH}}\\Ingescape\\include\\")
	set(PC_CZMQ_LIBRARY_HINTS "$ENV{${_MY_PROGRAM_FILES_PATH}}\\Ingescape\\lib\\")
endif (WIN32)

find_path (
    CZMQ_INCLUDE_DIRS
    NAMES czmq.h
    HINTS ${PC_CZMQ_INCLUDE_HINTS}
)

find_library (
    CZMQ_LIBRARIES
    NAMES czmq
    HINTS ${PC_CZMQ_LIBRARY_HINTS}
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    CZMQ
    REQUIRED_VARS CZMQ_LIBRARIES CZMQ_INCLUDE_DIRS
)
mark_as_advanced(
    CZMQ_FOUND
    CZMQ_LIBRARIES CZMQ_INCLUDE_DIRS
)
