
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libglfw.pc.cmake 
    ${CMAKE_CURRENT_BINARY_DIR}/libglfw.pc @ONLY)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_SOURCE_DIR}/lib 
    ${GLFW_INCLUDE_DIR})

set(libglfw_SOURCES
    ${common_SOURCES}
    macosx_enable.c
    macosx_fullscreen.c
    macosx_glext.c
    macosx_init.c
    macosx_joystick.c
    macosx_time.c
    macosx_window.c)

add_library(libglfwStatic STATIC ${libglfw_SOURCES})
add_library(libglfwShared SHARED ${libglfw_SOURCES}) 
target_link_libraries(libglfwShared  ${GLFW_LIBRARIES})
set_target_properties(libglfwStatic libglfwShared PROPERTIES 
    CLEAN_DIRECT_OUTPUT 1
    OUTPUT_NAME glfw
)

install(TARGETS libglfwStatic libglfwShared DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libglfw.pc DESTINATION lib/pkgconfig)

