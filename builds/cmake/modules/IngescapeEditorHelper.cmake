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

# Macro to add ingescape header directory in given var
macro(add_ingescape_common_ressources _RESSOURCES)
    list(APPEND ${_RESSOURCES} "${macro_current_dir}/../../../editor/apps/IngeScape-Common/igs_common_qml.qrc")
endmacro()

# Macro to get ingescape editor version from pro file
macro(get_ingescape_editor_version _MAJOR _MINOR _PATCH _BUILD)
    file(READ ${macro_current_dir}/../../../editor/apps/IngeScape-Editor/IngeScape-Editor.pro _EDITOR_PRO_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${_EDITOR_PRO_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${_EDITOR_PRO_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_PATCH = ([0-9]*)" _ ${_EDITOR_PRO_CONTENT})
    set(${_PATCH} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_BUILD = ([0-9]*)" _ ${_EDITOR_PRO_CONTENT})
    set(${_BUILD} ${CMAKE_MATCH_1})
endmacro()

# Macro to get ingescape assessments version from pro file
macro(get_ingescape_assessments_version _MAJOR _MINOR _PATCH _BUILD)
    file(READ ${macro_current_dir}/../../../editor/apps/IngeScape-Assessments/IngeScape-Assessments.pro _ASSESSMENTS_PRO_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${_ASSESSMENTS_PRO_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${_ASSESSMENTS_PRO_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_PATCH = ([0-9]*)" _ ${_ASSESSMENTS_PRO_CONTENT})
    set(${_PATCH} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_BUILD = ([0-9]*)" _ ${_ASSESSMENTS_PRO_CONTENT})
    set(${_BUILD} ${CMAKE_MATCH_1})
endmacro()

# Macro to get ingescape expe version from pro file
macro(get_ingescape_expe_version _MAJOR _MINOR _PATCH _BUILD)
    file(READ ${macro_current_dir}/../../../editor/apps/IngeScape-Expe/IngeScape-Expe.pro _EXPE_PRO_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${_EXPE_PRO_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${_EXPE_PRO_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_PATCH = ([0-9]*)" _ ${_EXPE_PRO_CONTENT})
    set(${_PATCH} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_BUILD = ([0-9]*)" _ ${_EXPE_PRO_CONTENT})
    set(${_BUILD} ${CMAKE_MATCH_1})
endmacro()
