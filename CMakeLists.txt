cmake_minimum_required(VERSION 2.8)
project(IconSetter)
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

add_executable(IconSetter ${FILES})
target_link_libraries(IconSetter Shell32.lib)
target_include_directories(IconSetter PUBLIC .)
target_compile_definitions(IconSetter PUBLIC NOMINMAX)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT IconSetter)
