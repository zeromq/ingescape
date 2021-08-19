if (NOT MSVC)
    include(FindPkgConfig)
    pkg_check_modules(PC_SODIUM "libsodium")
    if (NOT PC_SODIUM_FOUND)
        pkg_check_modules(PC_SODIUM "sodium")
    endif (NOT PC_SODIUM_FOUND)
    if (PC_SODIUM_FOUND)
        # add CFLAGS from pkg-config file, e.g. draft api.
        add_definitions(${PC_SODIUM_CFLAGS} ${PC_SODIUM_CFLAGS_OTHER})
        # some libraries install the headers is a subdirectory of the include dir
        # returned by pkg-config, so use a wildcard match to improve chances of finding
        # headers and SOs.
        set(PC_SODIUM_INCLUDE_HINTS ${PC_SODIUM_INCLUDE_DIRS} ${PC_SODIUM_INCLUDE_DIRS}/*)
        set(PC_SODIUM_LIBRARY_HINTS ${PC_SODIUM_LIBRARY_DIRS} ${PC_SODIUM_LIBRARY_DIRS}/*)
    endif(PC_SODIUM_FOUND)
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
	set(PC_SODIUM_INCLUDE_HINTS "C:\\Users\\admin\\dev\\install_dir\\include")
	set(PC_SODIUM_LIBRARY_HINTS "C:\\Users\\admin\\dev\\install_dir\\lib\\x64\\Debug")
endif (WIN32)

find_path (
    SODIUM_INCLUDE_DIRS
    NAMES sodium.h
    HINTS ${PC_SODIUM_INCLUDE_HINTS}
)

find_library (
    SODIUM_LIBRARIES
    NAMES sodium
    HINTS ${PC_SODIUM_LIBRARY_HINTS}
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    SODIUM
    REQUIRED_VARS SODIUM_LIBRARIES SODIUM_INCLUDE_DIRS
)
mark_as_advanced(
    SODIUM_FOUND
    SODIUM_LIBRARIES SODIUM_INCLUDE_DIRS
)
