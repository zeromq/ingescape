# Some helper for ingescape editor
# Like add source, used in editor

set(macro_current_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

# Macro to add ingescape sources in given var
macro(add_ingescape_common_sources _SOURCES)
    file(GLOB_RECURSE _ingescape_common_sources "${macro_current_dir}/../../../editor/apps/IngeScape-Common/*.cpp")
    list(APPEND ${_SOURCES}
        ${_ingescape_common_sources}
    )
endmacro()

# Macro to add ingescape header in given var
macro(add_ingescape_common_headers _HEADERS)
    file(GLOB_RECURSE _ingescape_common_headers "${macro_current_dir}/../../../editor/apps/IngeScape-Common/*.h")
    list(APPEND ${_HEADERS}
        ${_ingescape_common_headers}
    )
endmacro()

# Macro to add ingescape header directory in given var
macro(add_ingescape_common_include_directory _HEADERS)
    list(APPEND ${_HEADERS} "${macro_current_dir}/../../../editor/apps/IngeScape-Common")
endmacro()