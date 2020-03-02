# Some helper for ingescape library
# Like add source, used in library and editor

set(macro_current_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

# Macro to add ingescape sources in given var
macro(add_ingescape_sources _SOURCES)
    list(APPEND ${_SOURCES}
        ${macro_current_dir}/../../../src/definition.c
        ${macro_current_dir}/../../../src/mapping.c
        ${macro_current_dir}/../../../src/model.c
        ${macro_current_dir}/../../../src/network.c
        ${macro_current_dir}/../../../src/parser.c
        ${macro_current_dir}/../../../src/admin.c
        ${macro_current_dir}/../../../src/bus.c
        ${macro_current_dir}/../../../src/call.c
        ${macro_current_dir}/../../../src/license.c
        ${macro_current_dir}/../../../src/monitor.c
        ${macro_current_dir}/../../../src/json.c
        ${macro_current_dir}/../../../src/performance.c
        ${macro_current_dir}/../../../src/agent.c
        ${macro_current_dir}/../../../src/global.c
        ${macro_current_dir}/../../../dependencies/yajl/src/yajl_alloc.c
        ${macro_current_dir}/../../../dependencies/yajl/src/yajl_buf.c
        ${macro_current_dir}/../../../dependencies/yajl/src/yajl_encode.c
        ${macro_current_dir}/../../../dependencies/yajl/src/yajl_gen.c
        ${macro_current_dir}/../../../dependencies/yajl/src/yajl_lex.c
        ${macro_current_dir}/../../../dependencies/yajl/src/yajl_parser.c
        ${macro_current_dir}/../../../dependencies/yajl/src/yajl_tree.c
        ${macro_current_dir}/../../../dependencies/yajl/src/yajl_version.c
        ${macro_current_dir}/../../../dependencies/yajl/src/yajl.c
    )

    IF (WIN32)
        list(APPEND ${_SOURCES} ${macro_current_dir}/../../../dependencies/windows/unix/unixfunctions.c)
    ENDIF (WIN32)
endmacro()

# Macro to add ingescape header directory in given var
macro(add_ingescape_include_directory _HEADERS)
    list(APPEND ${_HEADERS} "${macro_current_dir}/../../../src/include")
    list(APPEND ${_HEADERS} "${macro_current_dir}/../../../dependencies/yajl/src/api")
	
    IF (WIN32)
		list(APPEND ${_HEADERS} "${macro_current_dir}/../../../dependencies/windows/unix")
    ENDIF (WIN32)
endmacro()

# Macro to add ingescape libs dependencies in given var
macro(add_ingescape_libraries_dependencies _LIBS _pkg_config_libs_private)
    list(APPEND CMAKE_MODULE_PATH "${macro_current_dir}/../dependencies")

    ########################################################################
    # LIBSODIUM dependency
    ########################################################################
    find_package(libsodium REQUIRED)
    IF (LIBSODIUM_FOUND)
        include_directories(${LIBSODIUM_INCLUDE_DIRS})
        list(APPEND ${_LIBS} ${LIBSODIUM_LIBRARIES})
        set(${_pkg_config_libs_private} "${${_pkg_config_libs_private}} -lsodium")
    ELSE (LIBSODIUM_FOUND)
        message( FATAL_ERROR "libsodium not found." )
    ENDIF (LIBSODIUM_FOUND)

    ########################################################################
    # LIBZMQ dependency
    ########################################################################
    find_package(libzmq REQUIRED)
    IF (LIBZMQ_FOUND)
        include_directories(${LIBZMQ_INCLUDE_DIRS})
        list(APPEND ${_LIBS} ${LIBZMQ_LIBRARIES})
        set(${_pkg_config_libs_private} "${${_pkg_config_libs_private}} -lzmq")
    ELSE (LIBZMQ_FOUND)
        message( FATAL_ERROR "libzmq not found." )
    ENDIF (LIBZMQ_FOUND)

    ########################################################################
    # CZMQ dependency
    ########################################################################
    find_package(czmq REQUIRED)
    IF (CZMQ_FOUND)
        include_directories(${CZMQ_INCLUDE_DIRS})
        list(APPEND ${_LIBS} ${CZMQ_LIBRARIES})
        set(${_pkg_config_libs_private} "${${_pkg_config_libs_private}} -lczmq")
    ELSE (CZMQ_FOUND)
        message( FATAL_ERROR "czmq not found." )
    ENDIF (CZMQ_FOUND)

    ########################################################################
    # ZYRE dependency
    ########################################################################
    find_package(zyre REQUIRED)
    IF (ZYRE_FOUND)
        include_directories(${ZYRE_INCLUDE_DIRS})
        list(APPEND ${_LIBS} ${ZYRE_LIBRARIES})
        set(${_pkg_config_libs_private} "${${_pkg_config_libs_private}} -lzyre")
    ELSE (ZYRE_FOUND)
        message( FATAL_ERROR "zyre not found." )
    ENDIF (ZYRE_FOUND)

    IF (APPLE)
        list(APPEND ${_LIBS} "-framework CoreFoundation")
    ENDIF (APPLE)
    
    if (MSVC)
        # required libraries for msvc
        list(APPEND ${_LIBS} ws2_32)
    elseif (MINGW)
        # required libraries for mingw
        list(APPEND ${_LIBS} -lws2_32)
    elseif (CYGWIN)
        # required libraries for cygwin
    endif()
endmacro()

# Macro to get ingescape version from C source file
macro(get_ingescape_version _MAJOR _MINOR _PATCH)
    file(READ ${macro_current_dir}/../../../src/admin.c _ADMIN_C_CONTENT)
    string(REGEX MATCH "#define INGESCAPE_MAJOR ([0-9]*)" _ ${_ADMIN_C_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define INGESCAPE_MINOR ([0-9]*)" _ ${_ADMIN_C_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    if (${CI_PIPELINE_ID})
        if (APPLE)
            math(EXPR moduloed_pipeline_id1 "${CI_PIPELINE_ID} / 1000" OUTPUT_FORMAT DECIMAL)
            math(EXPR moduloed_pipeline_id2 "${CI_PIPELINE_ID} % 1000" OUTPUT_FORMAT DECIMAL)
            set(${_PATCH} "${moduloed_pipeline_id1}.${moduloed_pipeline_id2}")
        else(APPLE)
            set(${_PATCH} ${CI_PIPELINE_ID})
        endif ()
    else ()
        string(REGEX MATCH "#define INGESCAPE_MICRO ([0-9]*)" _ ${_ADMIN_C_CONTENT})
        if (APPLE)
            math(EXPR moduloed_pipeline_id1 "${CMAKE_MATCH_1} / 1000" OUTPUT_FORMAT DECIMAL)
            math(EXPR moduloed_pipeline_id2 "${CMAKE_MATCH_1} % 1000" OUTPUT_FORMAT DECIMAL)
            set(${_PATCH} "${moduloed_pipeline_id1}.${moduloed_pipeline_id2}")
        else(APPLE)
            set(${_PATCH} ${CMAKE_MATCH_1})
        endif ()
    endif ()
endmacro()

# Function to install dependencies on windows
macro(install_ingescape_dependencies _LIB _HEADERS_PATH _IS_EDITOR)
    # Get path and file name from lib file
    get_filename_component(_PATH_TO_FILE ${${_LIB}} DIRECTORY)
    get_filename_component(_FILE_NAME ${${_LIB}} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" _FILE_WITHOUT_EXT ${_FILE_NAME})
    # Check if file exist
    find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)
    if (${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
        message("File ${_PATH_TO_FILE}/${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX} not found")
        find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "lib${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)

        if (${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
            message("File ${_PATH_TO_FILE}/lib${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX} not found")
        endif()
    endif()


    if (NOT ${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
        message("Add file ${${_FILE_WITHOUT_EXT}_DLL_FILE} to installed dependency")

        if (${_IS_EDITOR})
            install(FILES ${${_FILE_WITHOUT_EXT}_DLL_FILE} DESTINATION . COMPONENT library)
        else ()
            FILE(GLOB all_libs "${_PATH_TO_FILE}/${_FILE_WITHOUT_EXT}*")
            install(FILES ${all_libs} DESTINATION "lib${LIB_SUFFIX}" COMPONENT library)
            install(FILES ${${_FILE_WITHOUT_EXT}_DLL_FILE} DESTINATION "lib${LIB_SUFFIX}" COMPONENT library)
            install(DIRECTORY ${${_HEADERS_PATH}}/
                DESTINATION include
                COMPONENT library
                PATTERN ingescape_private.h EXCLUDE
            )
        endif()
    endif()
endmacro()
