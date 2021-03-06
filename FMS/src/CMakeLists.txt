include_directories(client api
  ${CONFIG_SOURCE_DIR}
  ${EMF4CPP_INCLUDE_DIR}
  ${EMF_DATA_DIR} 
  ${UMS_EMF_DATA_DIR}
  ${FMS_EMF_DATA_DIR}
  ${UMS_CLIENT_PROXY_SOURCE_DIR}
  ${UTILVISHNU_SOURCE_DIR}
  ${VISHNU_EXCEPTION_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${ZMQ_INCLUDE_DIR}
  ${COMMUNICATION_INCLUDE_DIR}
  ${VISHNU_SOURCE_DIR}/FMS/src/sed
  ${LIBJANSSON_INCLUDE_DIR}
  ${XMS_SED_SOURCE_DIR}
)

set(client_SRCS client/FileProxy
  client/LocalFileProxy
  client/RemoteFileProxy
  client/FileProxyFactory
  client/FileTransferProxy)

set(api_SRCS api/api_fms.cpp)
set(api_HEADERS api/api_fms.hpp)


## client CLI
if(COMPILE_CLIENT_FMS)
  add_subdirectory(cli)
  add_library(vishnu-fms-client ${client_SRCS} ${api_SRCS} ${utils_SRCS})
  SET_TARGET_PROPERTIES(vishnu-fms-client PROPERTIES VERSION ${VISHNU_VERSION})
  target_link_libraries(vishnu-fms-client
    ${Boost_LIBRARIES}
    emf4cpp-vishnu
    vishnu-core
    vishnu-ums-client
    ${LIBCRYPT_LIB}
    ${ZMQ_LIBRARIES}
    zmq_helper)
  install(TARGETS vishnu-fms-client DESTINATION ${LIB_INSTALL_DIR})
  install(FILES ${api_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})
endif()

if(COMPILE_SERVER_FMS)
  include_directories(${DATA_BASE_INCLUDE_DIR}
    ${REGISTRY_SOURCE_DIR}
    ${UMS_SERVER_SOURCE_DIR}
    ${FMS_SERVER_SOURCE_DIR})


  set(server_SRCS
    server/File.cpp
    server/SSHFile.cpp
    server/FileFactory.cpp
    server/FileTransferCommand.cpp
    server/FileTransferServer.cpp)

  add_library(vishnu-fms-server ${server_SRCS})
  set_target_properties(vishnu-fms-server PROPERTIES VERSION ${VISHNU_VERSION})
  install(TARGETS vishnu-fms-server DESTINATION ${LIB_INSTALL_DIR})

  target_link_libraries(vishnu-fms-server vishnu-core vishnu-core-server ${CMAKE_DL_LIBS})

endif(COMPILE_SERVER_FMS)
