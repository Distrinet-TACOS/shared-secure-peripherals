cmake_minimum_required(VERSION 3.2)
project(fs-load C)

include(GNUInstallDirs)

add_compile_options(-Wall)

find_program(CCACHE_FOUND ccache)

if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

set(SRC host/main.c)

add_executable(${PROJECT_NAME} ${SRC})

# target_include_directories(${PROJECT_NAME}
	# PRIVATE include)

target_link_libraries(${PROJECT_NAME}
	PRIVATE teec)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory(plugin)
