if (NOT MSVC)
    include(FindPkgConfig)
    pkg_check_modules(PC_YAJL "libyajl")
    if (NOT PC_YAJL_FOUND)
        pkg_check_modules(PC_YAJL "yajl")
    endif (NOT PC_YAJL_FOUND)
    if (PC_YAJL_FOUND)
        # add CFLAGS from pkg-config file, e.g. draft api.
        add_definitions(${PC_YAJL_CFLAGS} ${PC_YAJL_CFLAGS_OTHER})
        # some libraries install the headers is a subdirectory of the include dir
        # returned by pkg-config, so use a wildcard match to improve chances of finding
        # headers and SOs.
        set(PC_YAJL_INCLUDE_HINTS ${PC_YAJL_INCLUDE_DIRS} ${PC_YAJL_INCLUDE_DIRS}/*)
        set(PC_YAJL_LIBRARY_HINTS ${PC_YAJL_LIBRARY_DIRS} ${PC_YAJL_LIBRARY_DIRS}/*)
    endif(PC_YAJL_FOUND)
endif (NOT MSVC)

find_path (
    YAJL_INCLUDE_DIRS
    NAMES yajl
    HINTS ${PC_YAJL_INCLUDE_HINTS}
)

find_library (
    YAJL_LIBRARIES
    NAMES yajl
    HINTS ${PC_YAJL_LIBRARY_HINTS}
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    YAJL
    REQUIRED_VARS YAJL_LIBRARIES YAJL_INCLUDE_DIRS
)
mark_as_advanced(
    YAJL_FOUND
    YAJL_LIBRARIES YAJL_INCLUDE_DIRS
)

