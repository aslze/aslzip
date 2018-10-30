set(TARGET aslzip)

project(${TARGET})

set(SRC
    ZipFile.cpp
    GZip.cpp
    miniz.c
)

set(INC ../include/asl)

set(HEADERS
    ${INC}/ZipFile.h
    ${INC}/GZip.h
	${INC}/export.h
)

add_library(${TARGET} STATIC ${SRC} ${HEADERS}) 
target_link_libraries(${TARGET} asls) 

set_target_properties(${TARGET} PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_include_directories(${TARGET} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include> $<INSTALL_INTERFACE:${INSTALL_DIR}/include> )

set(TARGETS ${TARGETS} PARENT_SCOPE)

#export(TARGETS ${TARGET} EXPORT ${LIB_NAME} FILE ${CMAKE_BINARY_DIR}/${TARGET}Config.cmake )

install(TARGETS ${TARGET}
	EXPORT ${LIB_NAME}
	RUNTIME DESTINATION ${INSTALL_DIR}/bin
	LIBRARY DESTINATION ${INSTALL_DIR}/lib
	ARCHIVE DESTINATION ${INSTALL_DIR}/lib
	PUBLIC_HEADER DESTINATION ${INSTALL_DIR}/include/${INC}
)
