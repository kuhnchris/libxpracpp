project(libxpracpp)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_TOOLS "Build tools" ON)
find_library(ZLIB NAMES zlib z libz REQUIRED)
find_library(RENCODEPLUS NAMES rencodeplus librencodeplus REQUIRED)
find_library(PTHREAD NAMES pthread REQUIRED)


add_library(xpracpp libxpra.cpp)
target_link_libraries(xpracpp ${RENCODEPLUS} ${ZLIB})

if (BUILD_TOOLS)
add_executable(decode-xpra-packet-from-file tools/decode-xpra-packet-from-file.cpp)
set_target_properties(decode-xpra-packet-from-file PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tools)
target_link_libraries(decode-xpra-packet-from-file ${RENCODEPLUS} ${ZLIB} xpracpp)
endif()

add_executable(xpra-cpp xpra-c.cpp)
set_target_properties(xpra-cpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
target_link_libraries(xpra-cpp ${RENCODEPLUS} ${ZLIB} ${PTHREAD} xpracpp)

file(COPY data DESTINATION ${PROJECT_BINARY_DIR}/data)
