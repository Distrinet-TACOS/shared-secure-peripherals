project(b9b4b4c7-983f-497a-9fdc-f5df864f29c4.plugin C)

set(CFG_TEE_PLUGIN_LOAD_PATH "/usr/lib/tee-supplicant/plugins/")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_library(${PROJECT_NAME} SHARED fs-load.c)
target_compile_definitions(${PROJECT_NAME}
	PRIVATE -DBINARY_PREFIX="FS_LOAD"
)

target_link_libraries(${PROJECT_NAME} z)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CFG_TEE_PLUGIN_LOAD_PATH})
