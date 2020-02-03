# Some helper for ingescape editor
# Like add source, used in editor

set(macro_current_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

# Macro to add ingescape sources in given var
macro(add_ingescape_common_sources _SOURCES)
    add_definitions(-DINGESCAPECOMMON_INTERNAL_BUILD=1)
    # Add C++ source
    file(GLOB_RECURSE _ingescape_common_sources "${macro_current_dir}/../../../applications/apps/IngeScape-Common/*.cpp")
    list(APPEND ${_SOURCES}
        ${_ingescape_common_sources}
    )
    # Add Objective-C source if needed
    if (APPLE)
       file(GLOB_RECURSE _ingescape_common_objectivec_sources "${macro_current_dir}/../../../applications/apps/IngeScape-Common/*.mm")
       list(APPEND ${_SOURCES}
           ${_ingescape_common_objectivec_sources}
       )
    endif ()
endmacro()

# Macro to add ingescape header in given var
macro(add_ingescape_common_headers _HEADERS)
    file(GLOB_RECURSE _ingescape_common_headers "${macro_current_dir}/../../../applications/apps/IngeScape-Common/*.h")
    list(APPEND ${_HEADERS}
        ${_ingescape_common_headers}
    )
endmacro()

# Macro to add ingescape header directory in given var
macro(add_ingescape_common_include_directory _HEADERS)
    list(APPEND ${_HEADERS} "${macro_current_dir}/../../../applications/apps/IngeScape-Common")
endmacro()

# Macro to add ingescape header directory in given var
macro(add_ingescape_common_ressources _RESSOURCES)
    list(APPEND ${_RESSOURCES} "${macro_current_dir}/../../../applications/apps/IngeScape-Common/igs_common_qml.qrc")
endmacro()

if (${CI_PIPELINE_ID})
    math(EXPR moduloed_pipeline_id "${CI_PIPELINE_ID} % 1000" OUTPUT_FORMAT DECIMAL)
else()
    set(moduloed_pipeline_id "0")
endif()

# Macro to get ingescape editor version from pro file
macro(get_ingescape_editor_version _MAJOR _MINOR _PATCH _BUILD)
    file(READ ${macro_current_dir}/../../../applications/apps/IngeScape-Editor/IngeScape-Editor.pro _EDITOR_PRO_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${_EDITOR_PRO_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${_EDITOR_PRO_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    set(${_PATCH} ${moduloed_pipeline_id})
    string(REGEX MATCH "VERSION_BUILD = ([0-9]*)" _ ${_EDITOR_PRO_CONTENT})
    set(${_BUILD} ${CMAKE_MATCH_1})
endmacro()

# Macro to get ingescape assessments version from pro file
macro(get_ingescape_assessments_version _MAJOR _MINOR _PATCH _BUILD)
    file(READ ${macro_current_dir}/../../../applications/apps/IngeScape-Assessments/IngeScape-Assessments.pro _ASSESSMENTS_PRO_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${_ASSESSMENTS_PRO_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${_ASSESSMENTS_PRO_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    set(${_PATCH} ${moduloed_pipeline_id})
    string(REGEX MATCH "VERSION_BUILD = ([0-9]*)" _ ${_ASSESSMENTS_PRO_CONTENT})
    set(${_BUILD} ${CMAKE_MATCH_1})
endmacro()

# Macro to get ingescape expe version from pro file
macro(get_ingescape_expe_version _MAJOR _MINOR _PATCH _BUILD)
    file(READ ${macro_current_dir}/../../../applications/apps/IngeScape-Expe/IngeScape-Expe.pro _EXPE_PRO_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${_EXPE_PRO_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${_EXPE_PRO_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    set(${_PATCH} ${moduloed_pipeline_id})
    string(REGEX MATCH "VERSION_BUILD = ([0-9]*)" _ ${_EXPE_PRO_CONTENT})
    set(${_BUILD} ${CMAKE_MATCH_1})
endmacro()
