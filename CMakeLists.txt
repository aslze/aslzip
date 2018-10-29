cmake_minimum_required(VERSION 2.8.11)

set(LIB_NAME aslzip)

project(${LIB_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX} CACHE PATH "Install directory")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX "d")

find_package(ASL REQUIRED)

add_subdirectory(src)

option(ASLZIP_BUILD_SAMPLES "Build samples")

if(ASLZIP_BUILD_SAMPLES)
	add_subdirectory(samples)
endif()

export( TARGETS ${TARGETS} FILE ${LIB_NAME}Config.cmake )

if(CMAKE_SOURCE_DIR STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	export( PACKAGE ${LIB_NAME} )
endif()

install(EXPORT ${LIB_NAME} DESTINATION ${INSTALL_DIR}/cmake FILE ${LIB_NAME}Config.cmake )

#install(FILES ${CMAKE_BINARY_DIR}/${LIB_NAME}Config.cmake DESTINATION ${INSTALL_DIR}/cmake)
