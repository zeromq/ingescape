if (NOT MSVC)
    include(FindPkgConfig)
    pkg_check_modules(PC_INGESCAPE "libingescape")
    if (NOT PC_INGESCAPE_FOUND)
        pkg_check_modules(PC_INGESCAPE "ingescape")
    endif (NOT PC_INGESCAPE_FOUND)
    if (PC_INGESCAPE_FOUND)
        # add CFLAGS from pkg-config file, e.g. draft api.
        add_definitions(${PC_INGESCAPE_CFLAGS} ${PC_INGESCAPE_CFLAGS_OTHER})
        # some libraries install the headers is a subdirectory of the include dir
        # returned by pkg-config, so use a wildcard match to improve chances of finding
        # headers and SOs.
        set(PC_INGESCAPE_INCLUDE_HINTS ${PC_INGESCAPE_INCLUDE_DIRS} ${PC_INGESCAPE_INCLUDE_DIRS}/*)
        set(PC_INGESCAPE_LIBRARY_HINTS ${PC_INGESCAPE_LIBRARY_DIRS} ${PC_INGESCAPE_LIBRARY_DIRS}/*)
    endif(PC_INGESCAPE_FOUND)
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
	set(PC_INGESCAPE_INCLUDE_HINTS "$ENV{${_MY_PROGRAM_FILES_PATH}}\\Ingescape\\include\\")
	set(PC_INGESCAPE_LIBRARY_HINTS "$ENV{${_MY_PROGRAM_FILES_PATH}}\\Ingescape\\lib\\")
endif (WIN32)

find_path (
    INGESCAPE_INCLUDE_DIRS
    NAMES ingescape
    HINTS ${PC_INGESCAPE_INCLUDE_HINTS}
)

find_library (
    INGESCAPE_LIBRARIES
    NAMES ingescape
    HINTS ${PC_INGESCAPE_LIBRARY_HINTS}
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    INGESCAPE
    REQUIRED_VARS INGESCAPE_LIBRARIES INGESCAPE_INCLUDE_DIRS
)
mark_as_advanced(
    INGESCAPE_FOUND
    INGESCAPE_LIBRARIES INGESCAPE_INCLUDE_DIRS
)
