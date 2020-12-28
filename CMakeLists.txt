cmake_minimum_required(VERSION 3.9.0)
project(WordReminder)

include(CheckIPOSupported)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE SOURCE_LIST "./*.c")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

add_executable(${PROJECT_NAME} WIN32 ${SOURCE_LIST})
target_link_libraries(${PROJECT_NAME} Comctl32 winhttp ws2_32)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	check_ipo_supported(RESULT isIPOSupported)
	if(isIPOSupported)
		set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
endif()

install(TARGETS ${PROJECT_NAME})