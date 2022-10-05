get_filename_component(INLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(IMGUI_SRC_DIR ${INLUDE_DIR}/imgui)

list(APPEND IMGUI_SOURCES
        "${IMGUI_SRC_DIR}/imgui.cpp"
        "${IMGUI_SRC_DIR}/imgui_draw.cpp"
        "${IMGUI_SRC_DIR}/imgui_demo.cpp"
        "${IMGUI_SRC_DIR}/imgui_impl_opengl3.cpp"
        "${IMGUI_SRC_DIR}/imgui_impl_glfw.cpp"
        "${IMGUI_SRC_DIR}/imgui_widgets.cpp"
    )

list(APPEND IMGUI_PUBLIC_HEADERS
        "${IMGUI_SRC_DIR}/imgui.h"
    )


list(APPEND IMGUI_PRIVATE_HEADERS
        "${IMGUI_SRC_DIR}/imgui_internal.h"
        "${IMGUI_SRC_DIR}/imconfig.h"
        "${IMGUI_SRC_DIR}/imgui_impl_glfw.h"
        "${IMGUI_SRC_DIR}/imgui_impl_opengl3.h"
        "${IMGUI_SRC_DIR}/imstb_rectpack.h"
        "${IMGUI_SRC_DIR}/imstb_textedit.h"
        "${IMGUI_SRC_DIR}/imstb_truetype.h"
    )


list(APPEND IMGUI_PUBLIC_LIBRARIES glfw glad)

set(NAME imgui)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${NAME})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${NAME})

add_library(${NAME} STATIC ${IMGUI_SOURCES} ${IMGUI_PRIVATE_HEADERS} ${IMGUI_PUBLIC_HEADERS})

target_link_libraries(${NAME} PUBLIC 
		${IMGUI_PUBLIC_LIBRARIES}
	)

target_include_directories(${NAME} PUBLIC  
		${IMGUI_SRC_DIR}
    )

set_target_properties(imgui PROPERTIES FOLDER "Dependencies")