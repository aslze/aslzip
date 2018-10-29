cmake_minimum_required(VERSION 2.8.11)

set(TARGET zdemo)

project(${TARGET})

if(NOT TARGET aslzip)
	find_package( aslzip REQUIRED )
endif()

set(SRC
	demo.cpp
)

add_executable(${TARGET} ${SRC}) 

target_link_libraries ( ${TARGET} aslzip )

