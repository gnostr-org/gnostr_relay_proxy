cmake_minimum_required(VERSION 3.5)
project(relay_middleware VERSION 0.1.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LOWER)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(relay_middleware 
  src/main.cpp
)
target_include_directories(relay_middleware PUBLIC "include" "uWebSockets/src" "uWebSockets/uSockets/src")

add_library(uWs STATIC
  uWebSockets/uSockets/src/udp.c
  uWebSockets/uSockets/src/socket.c
  uWebSockets/uSockets/src/quic.c
  uWebSockets/uSockets/src/loop.c
  uWebSockets/uSockets/src/context.c
  uWebSockets/uSockets/src/bsd.c
  uWebSockets/uSockets/src/crypto/openssl.c
  uWebSockets/uSockets/src/crypto/sni_tree.cpp
  uWebSockets/uSockets/src/eventing/asio.cpp
  uWebSockets/uSockets/src/eventing/epoll_kqueue.c
  uWebSockets/uSockets/src/eventing/gcd.c
  uWebSockets/uSockets/src/eventing/libuv.c
)
target_include_directories(uWs PRIVATE "uWebSockets/src" "uWebSockets/uSockets/src" "/opt/homebrew/opt/openssl@3.0/include")
target_compile_definitions(uWs PRIVATE LIBUS_USE_OPENSSL)
find_library(LIBSSL_LIB ssl)
find_library(LIBCRYPTO_LIB crypto)
target_link_libraries(uWs PRIVATE z ${LIBCRYPTO_LIB} ${LIBSSL_LIB})

if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU OR ${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
  target_link_libraries(relay_middleware PRIVATE uWs)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_include_directories(relay_middleware PUBLIC ${VCPKG_INCLUDE_DIR})
  target_link_directories(relay_middleware PRIVATE ${VCPKG_LIB_DIR})
endif()

install(TARGETS relay_middleware RUNTIME DESTINATION bin)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "Nostr Relay middleware")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://git.v0l.io/Kieran/relay_middleware")
set(CPACK_PACKAGE_CONTACT "v0l <relay_middleware@v0l.io>")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
if(${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
  set(CPACK_GENERATOR "DragNDrop")
else()
  set(CPACK_GENERATOR "DEB" "TGZ" "TXZ")
endif()
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_STRIP_FILES true)

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_SECTION "other")
set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
include(CPack)