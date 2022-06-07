macro(dll_from_lib_exists _LIB)
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
		message("File ${${_FILE_WITHOUT_EXT}_DLL_FILE} exists")		
	else()
		message(FATAL_ERROR "Could not find DLL file relative to ${_LIB}")
	endif()
endmacro()
