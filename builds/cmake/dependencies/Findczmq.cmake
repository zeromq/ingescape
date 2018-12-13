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

if (DEFINED ENV{IGS_INCLUDE_DIRECTORY})
STRING(REPLACE "\"" "" IGS_INCLUDE_DIRECTORY_WITHOUT_QUOTES $ENV{IGS_INCLUDE_DIRECTORY})
endif ()
if (DEFINED ENV{IGS_LIBRARY_DIRECTORY})
STRING(REPLACE "\"" "" IGS_LIBRARY_DIRECTORY_WITHOUT_QUOTES $ENV{IGS_LIBRARY_DIRECTORY})
endif ()

find_path (
    CZMQ_INCLUDE_DIRS
    NAMES czmq.h
    HINTS ${PC_CZMQ_INCLUDE_HINTS}
	PATHS ${IGS_INCLUDE_DIRECTORY_WITHOUT_QUOTES}
)

find_library (
    CZMQ_LIBRARIES
    NAMES czmq
    HINTS ${PC_CZMQ_LIBRARY_HINTS}
	PATHS ${IGS_LIBRARY_DIRECTORY_WITHOUT_QUOTES}
)

if (${CZMQ_LIBRARIES} MATCHES CZMQ_LIBRARIES-NOTFOUND AND WIN32)
	SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll.a" ".dll" )
	find_library (
		CZMQ_LIBRARIES
		NAMES czmq
		HINTS ${PC_CZMQ_LIBRARY_HINTS}
		PATHS ${IGS_LIBRARY_DIRECTORY_WITHOUT_QUOTES}
	)
endif ()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    CZMQ
    REQUIRED_VARS CZMQ_LIBRARIES CZMQ_INCLUDE_DIRS
)
mark_as_advanced(
    CZMQ_FOUND
    CZMQ_LIBRARIES CZMQ_INCLUDE_DIRS
)
