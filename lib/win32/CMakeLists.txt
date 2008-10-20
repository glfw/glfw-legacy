
if(CYGWIN)
    configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/libglfw.pc.cmake 
	${CMAKE_CURRENT_BINARY_DIR}/libglfw.pc @ONLY)
endif(CYGWIN)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/lib)

set(libglfw_SOURCES
    ${common_SOURCES}
    win32_enable.c 
    win32_fullscreen.c
    win32_glext.c
    win32_init.c 
    win32_joystick.c
    win32_time.c 
    win32_window.c
    win32_dllmain.c)

add_library(libglfwStatic STATIC ${libglfw_SOURCES})
add_library(libglfwShared SHARED ${libglfw_SOURCES}) 

target_link_libraries(libglfwShared ${OPENGL_gl_LIBRARY})
set_target_properties(libglfwShared PROPERTIES 
    DEFINE_SYMBOL GLFW_BUILD_DLL)
set_target_properties(libglfwStatic libglfwShared PROPERTIES 
    CLEAN_DIRECT_OUTPUT 1
    OUTPUT_NAME glfw)

install(TARGETS libglfwStatic libglfwShared DESTINATION lib)

if(CYGWIN)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libglfw.pc DESTINATION lib/pkgconfig)
endif(CYGWIN)

