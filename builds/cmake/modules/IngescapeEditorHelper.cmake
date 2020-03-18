# Some helper for ingescape editor
# Like add source, used in editor

set(macro_current_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

include(IngescapeHelper)

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
    math(EXPR pipeline_id "${CI_PIPELINE_ID}" OUTPUT_FORMAT DECIMAL)
else()
    set(pipeline_id "0")
endif()

if (APPLE)
    math(EXPR pipeline_id1 "${pipeline_id} / 1000" OUTPUT_FORMAT DECIMAL)
    math(EXPR pipeline_id2 "${pipeline_id} % 1000" OUTPUT_FORMAT DECIMAL)
    set(pipeline_id "${pipeline_id1}.${pipeline_id2}")
endif ()

# Macro to get ingescape editor version from pro file
macro(get_ingescape_editor_version _MAJOR _MINOR _BUILD)
    file(READ ${macro_current_dir}/../../../applications/apps/IngeScape-Editor/IngeScape-Editor.pro _EDITOR_PRO_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${_EDITOR_PRO_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${_EDITOR_PRO_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    set(${_BUILD} ${pipeline_id})
endmacro()

# Macro to get ingescape assessments version from pro file
macro(get_ingescape_assessments_version _MAJOR _MINOR _BUILD)
    file(READ ${macro_current_dir}/../../../applications/apps/IngeScape-Assessments/IngeScape-Assessments.pro _ASSESSMENTS_PRO_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${_ASSESSMENTS_PRO_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${_ASSESSMENTS_PRO_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    set(${_BUILD} ${pipeline_id})
endmacro()

# Macro to get ingescape expe version from pro file
macro(get_ingescape_expe_version _MAJOR _MINOR _BUILD)
    file(READ ${macro_current_dir}/../../../applications/apps/IngeScape-Expe/IngeScape-Expe.pro _EXPE_PRO_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${_EXPE_PRO_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${_EXPE_PRO_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    set(${_BUILD} ${pipeline_id})
endmacro()

macro(install_apps_dependencies)
  # NB: libsodium library is already defined by Findlibsodium.cmake (LIBSODIUM_LIBRARIES)

  # ZeroMQ dll/lib built with MSVC follow the Boost naming convention
  # https://github.com/zeromq/czmq/issues/577
  # https://github.com/zeromq/czmq/issues/1972
  set(_zmq_version ${ZeroMQ_VERSION_MAJOR}_${ZeroMQ_VERSION_MINOR}_${ZeroMQ_VERSION_PATCH})
  set(_zmq_debug_names
      "libzmq-${CMAKE_VS_PLATFORM_TOOLSET}-mt-gd-${_zmq_version}" # Debug, BUILD_SHARED
      "libzmq-${CMAKE_VS_PLATFORM_TOOLSET}-mt-sgd-${_zmq_version}" # Debug, BUILD_STATIC
      "libzmq-mt-gd-${_zmq_version}" # Debug, BUILD_SHARED
      "libzmq-mt-sgd-${_zmq_version}" # Debug, BUILD_STATIC
  )

  set(_zmq_release_names
      "libzmq-${CMAKE_VS_PLATFORM_TOOLSET}-mt-${_zmq_version}" # Release|RelWithDebInfo|MinSizeRel, BUILD_SHARED
      "libzmq-${CMAKE_VS_PLATFORM_TOOLSET}-mt-s-${_zmq_version}" # Release|RelWithDebInfo|MinSizeRel, BUILD_STATIC
      "libzmq-mt-${_zmq_version}" # Release|RelWithDebInfo|MinSizeRel, BUILD_SHARED
      "libzmq-mt-s-${_zmq_version}" # Release|RelWithDebInfo|MinSizeRel, BUILD_STATIC
  )
    
  find_library(
    ZEROMQ_LIB
    NAMES
      zmq
      ${_zmq_release_names}
      ${_zmq_debug_names}
  )

  find_library(CZMQ_LIB NAMES czmq)
  find_library(ZYRE_LIB NAMES zyre)
  find_library(IGS_LIB NAMES ingescape)

  if (LIBSODIUM_LIBRARIES STREQUAL "LIBSODIUM_LIBRARIES-NOTFOUND")
    # ERROR
    message(WARNING "The libsodium library could not be found. It won't be copyed during the installation phase.")
  else()
    _install_dll_from_lib(LIBSODIUM_LIBRARIES TRUE)
  endif()
  if (ZEROMQ_LIB STREQUAL "ZEROMQ_LIB-NOTFOUND")
    # ERROR
    message(WARNING "The ZeroMQ library could not be found. It won't be copyed during the installation phase.")
  else()
    _install_dll_from_lib(ZEROMQ_LIB TRUE)
  endif()
  if (CZMQ_LIB STREQUAL "CZMQ_LIB-NOTFOUND")
    # ERROR
    message(WARNING "The czmq library could not be found. It won't be copyed during the installation phase.")
  else()
    _install_dll_from_lib(CZMQ_LIB TRUE)
  endif()
  if (ZYRE_LIB STREQUAL "ZYRE_LIB-NOTFOUND")
    # ERROR
    message(WARNING "The zyre library could not be found. It won't be copyed during the installation phase.")
  else()
    _install_dll_from_lib(ZYRE_LIB TRUE)
  endif()
  if (IGS_LIB STREQUAL "IGS_LIB-NOTFOUND")
    # ERROR
    message(WARNING "The ingescape library could not be found. It won't be copyed during the installation phase.")
  else()
    _install_dll_from_lib(IGS_LIB TRUE)
  endif()
endmacro()
