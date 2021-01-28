# Some helper for ingescape library
# Like add source, used in library and editor
set(macro_current_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

# Macro to get ingescape version from C source file
macro(get_ingescape_version _MAJOR _MINOR _PATCH)
    file(READ ${macro_current_dir}/../../../src/igs_admin.c _ADMIN_C_CONTENT)
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

# Function to install DLL files corresponding to the given LIB files.
#NB: Coded to work to install SO and DYLIB files. Not extensively tested though.
macro(install_dll_from_lib _LIB)
  # Get path and file name from lib file
  get_filename_component(_PATH_TO_FILE ${${_LIB}} DIRECTORY)
  get_filename_component(_FILE_NAME ${${_LIB}} NAME)
  string(REGEX REPLACE "\\.[^.]*$" "" _FILE_WITHOUT_EXT ${_FILE_NAME})

  # Check if DLL file exists next to the given LIB file
  find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)
  if (${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
    message(DEBUG "File ${_PATH_TO_FILE}/${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX} not found")

    # Check if libXXX.dll exists next to the given LIB file
    find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "lib${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)
    if (${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
      message(DEBUG "File ${_PATH_TO_FILE}/lib${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX} not found")

      # Check if the DLL file exists in ../bin relatively to the given LIB file
      get_filename_component(_PATH_TO_FILE ${_PATH_TO_FILE} DIRECTORY)
      set(_PATH_TO_FILE ${_PATH_TO_FILE}/bin)
      find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)
      if (${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
        message(DEBUG "File ${_PATH_TO_FILE}/${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX} not found")

        # Check if libXXX.dll exists in ../bin relatively to the given LIB file
        find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "lib${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)
      endif()
    endif()
  endif()

  if (NOT ${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
    message("Add file ${${_FILE_WITHOUT_EXT}_DLL_FILE} to installed dependency")
    #FIXME Do we also need .lib to be installed or just .dll ??
    FILE(GLOB all_libs "${_PATH_TO_FILE}/${_FILE_WITHOUT_EXT}*")
    install(FILES ${all_libs} DESTINATION "lib${LIB_SUFFIX}" COMPONENT library)
    install(FILES ${${_FILE_WITHOUT_EXT}_DLL_FILE} DESTINATION "lib${LIB_SUFFIX}" COMPONENT library)
  else()
    message(WARNING "Could not find DLL file relative to ${_LIB}. It won't be installed.")
  endif()
endmacro()
