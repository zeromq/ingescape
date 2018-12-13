if (NOT MSVC)
    include(FindPkgConfig)
    pkg_check_modules(PC_LIBZMQ "libzmq")
    if (NOT PC_LIBZMQ_FOUND)
        pkg_check_modules(PC_LIBZMQ "zmq")
    endif (NOT PC_LIBZMQ_FOUND)
    if (PC_LIBZMQ_FOUND)
        # add CFLAGS from pkg-config file, e.g. draft api.
        add_definitions(${PC_LIBZMQ_CFLAGS} ${PC_LIBZMQ_CFLAGS_OTHER})
        # some libraries install the headers is a subdirectory of the include dir
        # returned by pkg-config, so use a wildcard match to improve chances of finding
        # headers and SOs.
        set(PC_LIBZMQ_INCLUDE_HINTS ${PC_LIBZMQ_INCLUDE_DIRS} ${PC_LIBZMQ_INCLUDE_DIRS}/*)
        set(PC_LIBZMQ_LIBRARY_HINTS ${PC_LIBZMQ_LIBRARY_DIRS} ${PC_LIBZMQ_LIBRARY_DIRS}/*)
    endif(PC_LIBZMQ_FOUND)
endif (NOT MSVC)

if (DEFINED ENV{IGS_INCLUDE_DIRECTORY})
STRING(REPLACE "\"" "" IGS_INCLUDE_DIRECTORY_WITHOUT_QUOTES $ENV{IGS_INCLUDE_DIRECTORY})
endif ()
if (DEFINED ENV{IGS_LIBRARY_DIRECTORY})
STRING(REPLACE "\"" "" IGS_LIBRARY_DIRECTORY_WITHOUT_QUOTES $ENV{IGS_LIBRARY_DIRECTORY})
endif ()

find_path (
    LIBZMQ_INCLUDE_DIRS
    NAMES zmq.h
    HINTS ${PC_LIBZMQ_INCLUDE_HINTS}
	PATHS ${IGS_INCLUDE_DIRECTORY_WITHOUT_QUOTES}
)

find_library (
    LIBZMQ_LIBRARIES
    NAMES zmq
    HINTS ${PC_LIBZMQ_LIBRARY_HINTS}
	PATHS ${IGS_LIBRARY_DIRECTORY_WITHOUT_QUOTES}
)

if (${LIBZMQ_LIBRARIES} MATCHES LIBZMQ_LIBRARIES-NOTFOUND AND WIN32)
	SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll.a" ".dll" )
	find_library (
		LIBZMQ_LIBRARIES
		NAMES zmq
		HINTS ${PC_LIBZMQ_LIBRARY_HINTS}
		PATHS ${IGS_LIBRARY_DIRECTORY_WITHOUT_QUOTES}
	)
endif ()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    LIBZMQ
    REQUIRED_VARS LIBZMQ_LIBRARIES LIBZMQ_INCLUDE_DIRS
)
mark_as_advanced(
    LIBZMQ_FOUND
    LIBZMQ_LIBRARIES LIBZMQ_INCLUDE_DIRS
)

