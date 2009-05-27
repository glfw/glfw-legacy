
link_libraries(libglfwStatic ${GLFW_LIBRARIES} ${OPENGL_glu_LIBRARY})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include ${OPENGL_INCLUDE_DIR})

add_executable(accuracy accuracy.c)
add_executable(events events.c)
add_executable(peter peter.c)
add_executable(reopen reopen.c)
add_executable(version version.c)

if(APPLE)
  # Set fancy names for bundles
  add_executable(FSAA MACOSX_BUNDLE fsaa.c)
  add_executable(Tearing MACOSX_BUNDLE tearing.c)
else()
  # Set boring names for executables
  add_executable(tearing WIN32 tearing.c)
  add_executable(fsaa WIN32 fsaa.c)
endif(APPLE)

if(MSVC)
  # Tell MSVC to use main instead of WinMain for Windows subsystem executables
  set_target_properties(accuracy events fsaa peter reopen tearing version PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif(MSVC)

if(CYGWIN)
  # Set cross-compile and subsystem compile and link flags
  set_target_properties(accuracy events fsaa peter reopen tearing version PROPERTIES COMPILE_FLAGS "-mno-cygwin")
  set_target_properties(fsaa tearing PROPERTIES LINK_FLAGS "-mno-cygwin -mwindows")
  set_target_properties(accuracy events peter reopen version joytest PROPERTIES LINK_FLAGS "-mno-cygwin -mconsole")
endif(CYGWIN)

