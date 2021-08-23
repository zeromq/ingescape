#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "sodium" for configuration "MinSizeRel"
set_property(TARGET sodium APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(sodium PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/sodium.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/libsodium.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS sodium )
list(APPEND _IMPORT_CHECK_FILES_FOR_sodium "${_IMPORT_PREFIX}/lib/sodium.lib" "${_IMPORT_PREFIX}/bin/libsodium.dll" )

# Import target "sodium-static" for configuration "MinSizeRel"
set_property(TARGET sodium-static APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(sodium-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "C"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/libsodium.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS sodium-static )
list(APPEND _IMPORT_CHECK_FILES_FOR_sodium-static "${_IMPORT_PREFIX}/lib/libsodium.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
