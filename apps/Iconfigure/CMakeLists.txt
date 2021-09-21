set(SOURCE_DIR "source")
file(GLOB_RECURSE SOURCE_FILES
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
)

set(RESOURCE_DIR "resources")
file(GLOB RESOURCE_FILES
    "${RESOURCE_DIR}/*.rc"
    "${RESOURCE_DIR}/*.ico"
    "${RESOURCE_DIR}/*.h"
)

set(DATA_DIR "data")
file(GLOB DATA_FILES
    "${DATA_DIR}/*.txt"
)

set(FILES ${SOURCE_FILES} ${RESOURCE_FILES} ${DATA_FILES})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FILES})

set(TARGET_NAME Iconfigure)
add_executable(${TARGET_NAME} ${FILES})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} Common Shell32.lib)
target_compile_definitions(${TARGET_NAME} PUBLIC NOMINMAX)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER apps)

foreach(DATA_FILE ${DATA_FILES})
    get_filename_component(DATA_FILE_NAME ${DATA_FILE} NAME)
    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${DATA_FILE} $<TARGET_FILE_DIR:${TARGET_NAME}>/${DATA_FILE_NAME}
    )
endforeach()
