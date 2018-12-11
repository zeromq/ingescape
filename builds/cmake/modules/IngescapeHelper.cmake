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
        ${macro_current_dir}/../../../src/token.c
    )

    IF (WIN32)
        list(APPEND ${_SOURCES} dependencies/windows/unix/unixfunctions.c)
    ENDIF (WIN32)
    
    #The MSVC C compiler is too out of date,
    #so the sources have to be compiled as c++
    if (MSVC)
        enable_language(CXX)
        set_source_files_properties(
            ${${_SOURCES}}
            PROPERTIES LANGUAGE CXX
        )
    endif()
endmacro()

# Macro to add ingescape header directory in given var
macro(add_ingescape_include_directory _HEADERS)
    list(APPEND ${_HEADERS} "${macro_current_dir}/../../../src/include")
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

    ########################################################################
    # YAJL dependency
    ########################################################################
    find_package(yajl REQUIRED)
    IF (YAJL_FOUND)
        include_directories(${YAJL_INCLUDE_DIRS})
        list(APPEND ${_LIBS} ${YAJL_LIBRARIES})
        set(${_pkg_config_libs_private} "${${_pkg_config_libs_private}} -lyajl")
    ELSE (YAJL_FOUND)
        message( FATAL_ERROR "yajl not found." )
    ENDIF (YAJL_FOUND)
    
    if (MSVC)
        # required libraries for msvc
        set(MORE_LIBRARIES ws2_32)
    elseif (MINGW)
        # required libraries for mingw
        set(MORE_LIBRARIES -lws2_32)
    elseif (CYGWIN)
        # required libraries for cygwin
        set(MORE_LIBRARIES)
    endif()
endmacro()

# Macro to get ingescape version from C source file
macro(get_ingescape_version _MAJOR _MINOR _PATCH)
    file(READ ${macro_current_dir}/../../../src/admin.c _ADMIN_C_CONTENT)
    string(REGEX MATCH "#define INGESCAPE_MAJOR ([0-9]*)" _ ${_ADMIN_C_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define INGESCAPE_MINOR ([0-9]*)" _ ${_ADMIN_C_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define INGESCAPE_MICRO ([0-9]*)" _ ${_ADMIN_C_CONTENT})
    set(${_PATCH} ${CMAKE_MATCH_1})
endmacro()
