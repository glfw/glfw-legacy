include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/lib)
SET(libglfw_SOURCES
    ${common_SOURCES}
    win32_enable.c 
    win32_fullscreen.c
    win32_glext.c
    win32_init.c 
    win32_joystick.c
    win32_time.c 
    win32_window.c
    win32_dllmain.c)
ADD_LIBRARY(libglfwStatic STATIC ${libglfw_SOURCES})
ADD_LIBRARY(libglfwShared SHARED ${libglfw_SOURCES}) 
TARGET_LINK_LIBRARIES(libglfwShared ${OPENGL_gl_LIBRARY})
SET_TARGET_PROPERTIES(libglfwStatic libglfwShared PROPERTIES 
    CLEAN_DIRECT_OUTPUT 1
    OUTPUT_NAME glfw
    DEFINE_SYMBOL GLFW_BUILD_DLL
)

INSTALL(TARGETS libglfwStatic libglfwShared DESTINATION lib)

