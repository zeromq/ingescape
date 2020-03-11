# Some helper for ingescape library
# Like add source, used in library and editor

set(macro_current_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

# Macro to add ingescape header directory in given var
macro(add_ingescape_include_directory _HEADERS)
    list(APPEND ${_HEADERS} "${macro_current_dir}/../../../src/include")
    list(APPEND ${_HEADERS} "${macro_current_dir}/../../../dependencies/yajl/src/api")
	
    IF (WIN32)
		list(APPEND ${_HEADERS} "${macro_current_dir}/../../../dependencies/windows/unix")
    ENDIF (WIN32)
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
