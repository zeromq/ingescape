add_executable(igsTester
    src/tester.c
    src/common.c)

add_executable(igsPartner
    src/partner.c
    src/common.c)

target_include_directories(igsTester PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src # local headers
  $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/../packaging/windows/unix> # getopt.h on windows only
)
target_include_directories(igsPartner PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src # local headers
  $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_SOURCE_DIR}/../packaging/windows/unix> # getopt.h on windows only
)

add_dependencies(igsTester ingescape)
add_dependencies(igsPartner ingescape)

target_link_libraries(igsTester PRIVATE
  ingescape
  $<$<BOOL:${WIN32}>:ws2_32>
)
target_link_libraries(igsPartner PRIVATE
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
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
  install (TARGETS igsTester RUNTIME DESTINATION bin COMPONENT agent)
  install (TARGETS igsPartner RUNTIME DESTINATION bin COMPONENT agent)
  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/tester.cert
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/tester.cert_secret
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/partner.cert
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/partner.cert_secret
          DESTINATION "share/ingescape/certificates"
          PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
          COMPONENT agent)
endif()
