cmake_minimum_required(VERSION 3.2)
project(fs-load)

set (CMAKE_SYSTEM_NAME Linux)

include(GNUInstallDirs)

add_compile_options(-Wall)

find_program(CCACHE_FOUND ccache)

if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

add_subdirectory(plugin)
