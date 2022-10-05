get_filename_component(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(GLAD_SRC_DIR ${INCLUDE_DIR}/glad)
set(GLAD_INC_DIR ${GLAD_SRC_DIR}/include)

list(APPEND GLAD_PUBLIC_HEADERS
        "${GLAD_INC_DIR}/glad/glad.h"
        "${GLAD_INC_DIR}/KHR/khrplatform.h"
    )

list(APPEND GLAD_SOURCES
        "${GLAD_SRC_DIR}/src/glad.c"
    )

set(NAME glad)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${NAME})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${NAME})

add_library(${NAME} STATIC ${GLAD_SOURCES} ${GLAD_PUBLIC_HEADERS})

target_include_directories(${NAME}
		PUBLIC  
		${GLAD_INC_DIR}
    )
	
set_target_properties(glad PROPERTIES FOLDER "Dependencies")
