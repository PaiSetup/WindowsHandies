set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE SOURCE_FILES
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
)

set(FILES ${SOURCE_FILES} ${RESOURCE_FILES})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FILES})

set(TARGET_NAME Common)
add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER tools)
