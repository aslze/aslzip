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

