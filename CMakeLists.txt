project(GLFW C)
set(GLFW_VERSION_MAJOR "3")
set(GLFW_VERSION_MINOR "0")
set(GLFW_VERSION_MICRO "0")
set(GLFW_VERSION_EXTRA "")
set(GLFW_VERSION "${GLFW_VERSION_MAJOR}.${GLFW_VERSION_MINOR}")
set(GLFW_VERSION_FULL  
    "${GLFW_VERSION}.${GLFW_VERSION_MICRO}${GLFW_VERSION_EXTRA}")


cmake_minimum_required(VERSION 2.4)

include(CheckFunctionExists)
include(CheckSymbolExists)

# -- Stuff common to all platform 
find_package(OpenGL REQUIRED)

set(common_SOURCES 
    ${CMAKE_SOURCE_DIR}/lib/enable.c 
    ${CMAKE_SOURCE_DIR}/lib/fullscreen.c
    ${CMAKE_SOURCE_DIR}/lib/glext.c 
    ${CMAKE_SOURCE_DIR}/lib/init.c 
    ${CMAKE_SOURCE_DIR}/lib/input.c 
    ${CMAKE_SOURCE_DIR}/lib/joystick.c 
    ${CMAKE_SOURCE_DIR}/lib/time.c
    ${CMAKE_SOURCE_DIR}/lib/window.c
)

#  Deal with Win32 Specific stuffs 
if (WIN32)
    message(STATUS "Building GLFW for Win32 systems") 
    set(CMAKE_REQUIRED_LIBRARIES ${OPENGL_gl_LIBRARY})
    list(APPEND GLFW_INCLUDE_DIR ${OPENGL_INCLUDE_DIR})
    list(APPEND GLFW_LIBRARIES ${OPENGL_gl_LIBRARY})
    add_subdirectory(lib/win32)
endif (WIN32)


# Deal with Unix Specifics (Not MacOS/X) 
if (UNIX AND NOT APPLE AND NOT CYGWIN)
    set(CMAKE_REQUIRED_LIBRARIES ${X11_X11_LIB} ${OPENGL_gl_LIBRARY})
    list(APPEND GLFW_INCLUDE_DIR ${X11_X11_INCLUDE_PATH})
    list(APPEND GLFW_LIBRARIES ${X11_X11_LIB})
    list(APPEND GLFW_INCLUDE_DIR ${OPENGL_INCLUDE_DIR})
    list(APPEND GLFW_LIBRARIES ${OPENGL_gl_LIBRARY})
    message(STATUS "Building GLFW for a Unix flavor (not Mac OS X)")
    
    # Detect X11 extension 
    include(${CMAKE_SOURCE_DIR}/CMake/CHECK_X11_EXTENSIONS.cmake)
    
    # XRandR
    CHECK_X11_XRANDR()
    if (X11_XRANDR_FOUND)
        set(_GLFW_HAS_XRANDR 1) 
        list(APPEND GLFW_INCLUDE_DIR ${X11_XRANDR_INCLUDE_DIR})
        list(APPEND GLFW_LIBRARIES ${X11_XRANDR_LIBRARIES})
    endif(X11_XRANDR_FOUND)

    # xf86vidmode 
    CHECK_X11_XF86VIDMODE()
    if (X11_XF86VIDMODE_FOUND)
        set(_GLFW_HAS_XF86VIDMODE 1)
        list(APPEND GLFW_INCLUDE_DIR ${X11_XF86VIDMODE_INCLUDE_DIR})
        list(APPEND GLFW_LIBRARIES ${X11_XF86VIDMODE_LIBRARIES})
    endif(X11_XF86VIDMODE_FOUND) 

    # GLX Related check 
    CHECK_FUNCTION_EXISTS(glXGetProcAddress    _GLFW_HAS_GLXGETPROCADDRESS)
    CHECK_FUNCTION_EXISTS(glXGetProcAddressARB _GLFW_HAS_GLXGETPROCADDRESSARB)
    CHECK_FUNCTION_EXISTS(glXGetProcAddressEXT _GLFW_HAS_GLXGETPROCADDRESSEXT)

    add_subdirectory(lib/x11)
endif(UNIX AND NOT APPLE AND NOT CYGWIN)

# Deal with MacOSX Specifics 
if (UNIX AND APPLE)
    message(STATUS "Building GLFW for Mac OS X")
    # Mac OS X AGL 
    find_library(AGL_FRAMEWORK AGL)
    list(APPEND GLFW_LIBRARIES ${AGL_FRAMEWORK})
    find_library(CARBON_FRAMEWORK CARBON)
    list(APPEND GLFW_LIBRARIES ${CARBON_FRAMEWORK})
    add_subdirectory(lib/macosx)
endif(UNIX AND APPLE)

add_subdirectory(examples)
#add_subdirectory(docs/doxygen)
#add_subdirectory(docs/manuals)


#add_subdirectory(contrib) # modules is the best than contrib

#--------------------------------------------------------------------
# -- Install standard files 
#--------------------------------------------------------------------
# install headers 
install(DIRECTORY include/ DESTINATION include 
    PATTERN ".svn" EXCLUDE 
    PATTERN "include/*"
)
# install docs 
install(
    FILES 
        COPYING.txt 
        readme.html 
    DESTINATION 
        share/doc/glfw-${GLFW_VERSION_FULL}/
)
install(
    DIRECTORY 
        docs/images 
    DESTINATION 
        share/doc/glfw-${GLFW_VERSION_FULL}/
        PATTERN ".svn" EXCLUDE
        PATTERN "images/*" 
)


#--------------------------------------------------------------------
# -- Additional stuff 
#--------------------------------------------------------------------

#--------------------------------------------------------------------
# -- Documentation generation 
#--------------------------------------------------------------------
#include("${CMAKE_CURRENT_SOURCE_DIR}/documentation.cmake")
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
#               "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
#               IMMEDIATE @ONLY)
#add_doxygen_target("${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
#add_subdirectory(docs)

#--------------------------------------------------------------------
# -- Uninstall operation
# -------------------------------------------------------------------
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" 
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)
add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

