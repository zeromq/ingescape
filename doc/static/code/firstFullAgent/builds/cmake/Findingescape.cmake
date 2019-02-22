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

find_path (
    INGESCAPE_INCLUDE_DIRS
    NAMES ingescape.h
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
