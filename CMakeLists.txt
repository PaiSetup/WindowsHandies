cmake_minimum_required(VERSION 2.8)
set(PRODUCT_NAME Iconfigure)
project(${PRODUCT_NAME})
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

set(SOURCE_DIR "source")
file(GLOB_RECURSE SOURCE_FILES
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
)

set(RESOURCE_DIR "resources")
file(GLOB RESOURCE_FILES
    "${RESOURCE_DIR}/*.rc"
)

set(FILES ${SOURCE_FILES} ${RESOURCE_FILES})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FILES})

add_executable(${PRODUCT_NAME} ${FILES})
target_link_libraries(${PRODUCT_NAME} Shell32.lib)
target_include_directories(${PRODUCT_NAME} PUBLIC .)
target_compile_definitions(${PRODUCT_NAME} PUBLIC NOMINMAX)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${PRODUCT_NAME})
