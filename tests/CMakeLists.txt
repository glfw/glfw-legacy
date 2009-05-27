
link_libraries(libglfwStatic ${GLFW_LIBRARIES} ${OPENGL_glu_LIBRARY})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include ${OPENGL_INCLUDE_DIR})

add_executable(accuracy accuracy.c)
add_executable(events events.c)
add_executable(fsaa fsaa.c)
add_executable(peter peter.c)
add_executable(reopen reopen.c)
add_executable(tearing tearing.c)
add_executable(version version.c)

