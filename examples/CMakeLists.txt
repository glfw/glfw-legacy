# This line is used to link with static libraries 
# Note that the library list should be updated to be obtained from 
# the main CMakeLists.txt 
link_libraries(libglfwStatic 
    ${GLFW_LIBRARIES}
    ${OPENGL_glu_LIBRARY})

# While working on the library it is best to link to the shared library 
#link_libraries(libglfwShared
#    ${OPENGL_glu_LIBRARY})
#add_definitions(-DGLFW_DLL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include ${OPENGL_INCLUDE_DIR})

add_executable(boing WIN32 MACOSX_BUNDLE boing.c)
add_executable(gears WIN32 MACOSX_BUNDLE gears.c)
add_executable(keytest keytest.c)
add_executable(listmodes listmodes.c)
#add_executable(mimaps WIN32 mimaps.c)
#add_executable(mtbench mtbench.c)
#add_executable(mthello mthello.c)
#add_executable(particles WIN32 particles.c)
#add_executable(pong3d WIN32 pong3d.c)
add_executable(splitview WIN32 MACOSX_BUNDLE splitview.c)
add_executable(triangle WIN32 MACOSX_BUNDLE triangle.c)
add_executable(wave WIN32 MACOSX_BUNDLE wave.c)

if(MSVC)
  set_target_properties(boing gears splitview triangle wave PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif(MSVC)

if(CYGWIN)
  set_target_properties(boing gears keytest listmodes splitview triangle wave PROPERTIES COMPILE_FLAGS "-mno-cygwin")
  set_target_properties(boing gears splitview triangle wave PROPERTIES LINK_FLAGS "-mno-cygwin -mwindows")
  set_target_properties(keytest listmodes PROPERTIES LINK_FLAGS "-mno-cygwin -mconsole")
endif(CYGWIN)

