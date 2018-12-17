if (NOT MSVC)
    include(FindPkgConfig)
    pkg_check_modules(PC_ZYRE "libzyre")
    if (NOT PC_ZYRE_FOUND)
        pkg_check_modules(PC_ZYRE "zyre")
    endif (NOT PC_ZYRE_FOUND)
    if (PC_ZYRE_FOUND)
        # add CFLAGS from pkg-config file, e.g. draft api.
        add_definitions(${PC_ZYRE_CFLAGS} ${PC_ZYRE_CFLAGS_OTHER})
        # some libraries install the headers is a subdirectory of the include dir
        # returned by pkg-config, so use a wildcard match to improve chances of finding
        # headers and SOs.
        set(PC_ZYRE_INCLUDE_HINTS ${PC_ZYRE_INCLUDE_DIRS} ${PC_ZYRE_INCLUDE_DIRS}/*)
        set(PC_ZYRE_LIBRARY_HINTS ${PC_ZYRE_LIBRARY_DIRS} ${PC_ZYRE_LIBRARY_DIRS}/*)
    endif(PC_ZYRE_FOUND)
endif (NOT MSVC)

find_path (
    ZYRE_INCLUDE_DIRS
    NAMES zyre.h
    HINTS ${PC_ZYRE_INCLUDE_HINTS}
)

find_library (
    ZYRE_LIBRARIES
    NAMES zyre
    HINTS ${PC_ZYRE_LIBRARY_HINTS}
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    ZYRE
    REQUIRED_VARS ZYRE_LIBRARIES ZYRE_INCLUDE_DIRS
)
mark_as_advanced(
    ZYRE_FOUND
    ZYRE_LIBRARIES ZYRE_INCLUDE_DIRS
)
