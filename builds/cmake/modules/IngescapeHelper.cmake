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
        list(APPEND ${_SOURCES} ${macro_current_dir}/../../../dependencies/windows/unix/unixfunctions.c)
    ENDIF (WIN32)
endmacro()

# Macro to add ingescape header directory in given var
macro(add_ingescape_include_directory _HEADERS)
    list(APPEND ${_HEADERS} "${macro_current_dir}/../../../src/include")
	
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
    string(REGEX MATCH "#define INGESCAPE_MICRO ([0-9]*)" _ ${_ADMIN_C_CONTENT})
    set(${_PATCH} ${CMAKE_MATCH_1})
endmacro()

# Function to install dependencies on windows
macro(install_ingescape_dependencies _LIB _HEADERS_PATH)
    # Get path and file name from lib file
    get_filename_component(_PATH_TO_FILE ${${_LIB}} DIRECTORY)
    get_filename_component(_FILE_NAME ${${_LIB}} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" _FILE_WITHOUT_EXT ${_FILE_NAME})
    # Check if file exist
    find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "${_FILE_WITHOUT_EXT}.dll" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)
    if (${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
        message("File ${_PATH_TO_FILE}/${_FILE_WITHOUT_EXT}.dll not found")
        find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "lib${_FILE_WITHOUT_EXT}.dll" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)

        if (${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
            message("File ${_PATH_TO_FILE}/lib${_FILE_WITHOUT_EXT}.dll not found")
        endif()
    endif()


    if (NOT ${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
        message("Add file ${${_FILE_WITHOUT_EXT}_DLL_FILE} to installed dependency")

        install(FILES ${${_LIB}} DESTINATION "lib${LIB_SUFFIX}")
        install(FILES ${${_FILE_WITHOUT_EXT}_DLL_FILE} DESTINATION "bin")
        install(DIRECTORY ${${_HEADERS_PATH}}/ DESTINATION include)
    endif()
endmacro()
