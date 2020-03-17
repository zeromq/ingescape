# Some helper for ingescape library
# Like add source, used in library and editor
set(macro_current_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

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

# Function to install DLL files corresponding to the given LIB files.
#NB: Coded to work to install SO and DYLIB files. Not extensively tested though.
macro(_install_dll_from_lib _LIB _IS_EDITOR)
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
        if (${_IS_EDITOR})
            install(FILES ${${_FILE_WITHOUT_EXT}_DLL_FILE} DESTINATION . COMPONENT library)
        else ()
		    #FIXME Do we also need .lib to be installed or just .dll ??
            FILE(GLOB all_libs "${_PATH_TO_FILE}/${_FILE_WITHOUT_EXT}*")
            install(FILES ${all_libs} DESTINATION "lib${LIB_SUFFIX}" COMPONENT library)
            install(FILES ${${_FILE_WITHOUT_EXT}_DLL_FILE} DESTINATION "lib${LIB_SUFFIX}" COMPONENT library)
        endif()
	else()
		message(WARNING "Could not find DLL file relative to ${_LIB}. It won't be installed.")
    endif()
endmacro()

macro(install_igs_lib_dependencies)
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

  if (LIBSODIUM_LIBRARIES STREQUAL "LIBSODIUM_LIBRARIES-NOTFOUND")
    # ERROR
    message(WARNING "The libsodium library could not be found. It won't be copyed during the installation phase.")
  else()
	_install_dll_from_lib(LIBSODIUM_LIBRARIES FALSE)
  endif()
  if (ZEROMQ_LIB STREQUAL "ZEROMQ_LIB-NOTFOUND")
    # ERROR
    message(WARNING "The ZeroMQ library could not be found. It won't be copyed during the installation phase.")
  else()
	_install_dll_from_lib(ZEROMQ_LIB FALSE)
  endif()
  if (CZMQ_LIB STREQUAL "CZMQ_LIB-NOTFOUND")
    # ERROR
    message(WARNING "The czmq library could not be found. It won't be copyed during the installation phase.")
  else()
	_install_dll_from_lib(CZMQ_LIB FALSE)
  endif()
  if (ZYRE_LIB STREQUAL "ZYRE_LIB-NOTFOUND")
    # ERROR
    message(WARNING "The zyre library could not be found. It won't be copyed during the installation phase.")
  else()
	_install_dll_from_lib(ZYRE_LIB FALSE)
  endif()
endmacro()
