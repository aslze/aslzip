cmake_minimum_required(VERSION 3.0)

set(LIB_NAME aslzip)

project(${LIB_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX "d")

if(NOT TARGET asls)
	find_package(ASL REQUIRED)
endif()

add_subdirectory(src)

option(ASLZIP_BUILD_SAMPLES "Build samples")

if(ASLZIP_BUILD_SAMPLES)
	add_subdirectory(samples)
endif()

export(EXPORT ${LIB_NAME} FILE ${LIB_NAME}Config.cmake)

if(CMAKE_SOURCE_DIR STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	export( PACKAGE ${LIB_NAME} )
endif()

install(EXPORT ${LIB_NAME} DESTINATION cmake FILE ${LIB_NAME}Config.cmake )
