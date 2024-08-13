if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "MacOS Universal Binaries")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()
add_executable(igsTester
    src/tester.c
    src/common.c)

add_executable(igsPartner
    src/partner.c
    src/common.c)

add_executable(igsStresser
    src/stresser.c)

# Override default release flags to not define NDEBUG, which CMake sets by default for Release builds
# NB: The NDEBUG flag compile out all assert() calls, skipping whatever was done in the assert call (function call, computation, etc.)
# It changes the compiled code from the source code and we don't want that.
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
   # Enable assertions in release mode
   message(STATUS "Enable C asserts in release mode (instead of noop)")
   if (MSVC)
       string(REPLACE "/D NDEBUG" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
       string(REPLACE "/D NDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
       string(REPLACE "/D NDEBUG" "" CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})

       string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
       string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
       string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})

       string(REPLACE "/D NDEBUG" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
       string(REPLACE "/D NDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})
       string(REPLACE "/D NDEBUG" "" CMAKE_C_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_MINSIZEREL})

       string(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
       string(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})
       string(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_MINSIZEREL})
   else() 
       string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
       string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
       string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")

       string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
       string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
       string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
   endif()
endif()

target_include_directories(igsTester PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src # local headers
  $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/../packaging/windows/unix> # getopt.h on windows only
)
target_include_directories(igsPartner PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src # local headers
  $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/../packaging/windows/unix> # getopt.h on windows only
)
target_include_directories(igsStresser PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src # local headers
  $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/../packaging/windows/unix> # getopt.h on windows only
)

add_dependencies(igsTester ingescape)
add_dependencies(igsPartner ingescape)
add_dependencies(igsStresser ingescape)

target_link_libraries(igsTester PRIVATE
  ingescape
  $<$<BOOL:${WIN32}>:ws2_32>
)
target_link_libraries(igsPartner PRIVATE
  ingescape
  $<$<BOOL:${WIN32}>:ws2_32>
)
target_link_libraries(igsStresser PRIVATE
  ingescape
  $<$<BOOL:${WIN32}>:ws2_32>
)

if (WITH_DEPS)
  target_link_libraries(igsTester PRIVATE sodium)
  target_link_libraries(igsTester PRIVATE libzmq)
  target_link_libraries(igsTester PRIVATE czmq)
  target_link_libraries(igsTester PRIVATE zyre)

  target_link_libraries(igsPartner PRIVATE sodium)
  target_link_libraries(igsPartner PRIVATE libzmq)
  target_link_libraries(igsPartner PRIVATE czmq)
  target_link_libraries(igsPartner PRIVATE zyre)

  target_link_libraries(igsStresser PRIVATE sodium)
  target_link_libraries(igsStresser PRIVATE libzmq)
  target_link_libraries(igsStresser PRIVATE czmq)
  target_link_libraries(igsStresser PRIVATE zyre)
else ()
  target_link_libraries(igsTester PRIVATE ${LIBSODIUM_LIBRARIES})
  target_include_directories(igsTester PRIVATE ${LIBSODIUM_INCLUDE_DIRS})
  target_link_libraries(igsTester PRIVATE libzmq)
  target_include_directories(igsTester PRIVATE ${ZeroMQ_INCLUDE_DIR})
  target_link_libraries(igsTester PRIVATE czmq)
  target_include_directories(igsTester PRIVATE ${CZMQ_PUBLIC_HEADERS_DIR})
  target_link_libraries(igsTester PRIVATE zyre)
  target_include_directories(igsTester PRIVATE ${zyre_INCLUDES_DIR})

  target_link_libraries(igsPartner PRIVATE ${LIBSODIUM_LIBRARIES})
  target_include_directories(igsPartner PRIVATE ${LIBSODIUM_INCLUDE_DIRS})
  target_link_libraries(igsPartner PRIVATE libzmq)
  target_include_directories(igsPartner PRIVATE ${ZeroMQ_INCLUDE_DIR})
  target_link_libraries(igsPartner PRIVATE czmq)
  target_include_directories(igsPartner PRIVATE ${CZMQ_PUBLIC_HEADERS_DIR})
  target_link_libraries(igsPartner PRIVATE zyre)
  target_include_directories(igsPartner PRIVATE ${zyre_INCLUDES_DIR})

  target_link_libraries(igsStresser PRIVATE ${LIBSODIUM_LIBRARIES})
  target_include_directories(igsStresser PRIVATE ${LIBSODIUM_INCLUDE_DIRS})
  target_link_libraries(igsStresser PRIVATE libzmq)
  target_include_directories(igsStresser PRIVATE ${ZeroMQ_INCLUDE_DIR})
  target_link_libraries(igsStresser PRIVATE czmq)
  target_include_directories(igsStresser PRIVATE ${CZMQ_PUBLIC_HEADERS_DIR})
  target_link_libraries(igsStresser PRIVATE zyre)
  target_include_directories(igsStresser PRIVATE ${zyre_INCLUDES_DIR})
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
  install (TARGETS igsTester RUNTIME DESTINATION bin COMPONENT agent)
  install (TARGETS igsPartner RUNTIME DESTINATION bin COMPONENT agent)
  install (TARGETS igsStresser RUNTIME DESTINATION bin COMPONENT agent)
  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/tester.cert
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/tester.cert_secret
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/partner.cert
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/partner.cert_secret
          DESTINATION "share/ingescape/certificates"
          PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
          COMPONENT agent)
endif()
