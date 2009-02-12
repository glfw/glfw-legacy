//========================================================================
// GLFW - An OpenGL framework
// File:        x11_init.c
// Platform:    X11 (Unix)
// API version: 2.7
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Camilla Berglund
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "internal.h"



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// Terminate GLFW when exiting application
//========================================================================

void _glfwTerminate_atexit( void )
{
    glfwTerminate();
}


//========================================================================
// Initialize X11 display
//========================================================================

static int _glfwInitDisplay( void )
{
    // Open display
    _glfwLibrary.display = XOpenDisplay( 0 );
    if( !_glfwLibrary.display )
    {
        return GL_FALSE;
    }

    // Check for XF86VidMode extension
#ifdef _GLFW_HAS_XF86VIDMODE
    _glfwLibrary.XF86VidMode.Available =
        XF86VidModeQueryExtension( _glfwLibrary.display,
	                           &_glfwLibrary.XF86VidMode.EventBase,
	                           &_glfwLibrary.XF86VidMode.ErrorBase);
#else
    _glfwLibrary.XF86VidMode.Available = 0;
#endif

    // Check for XRandR extension
#ifdef _GLFW_HAS_XRANDR
    _glfwLibrary.XRandR.Available =
        XRRQueryExtension( _glfwLibrary.display,
	                   &_glfwLibrary.XRandR.EventBase,
			   &_glfwLibrary.XRandR.ErrorBase );
#else
    _glfwLibrary.XRandR.Available = 0;
#endif

     return GL_TRUE;
}


//========================================================================
// Terminate X11 display
//========================================================================

static void _glfwTerminateDisplay( void )
{
    // Open display
    if( _glfwLibrary.display )
    {
        XCloseDisplay( _glfwLibrary.display );
        _glfwLibrary.display = NULL;
    }
}


//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// Initialize various GLFW state
//========================================================================

int _glfwPlatformInit( void )
{
    // Initialize display
    if( !_glfwInitDisplay() )
    {
        return GL_FALSE;
    }

    // Install atexit() routine
    atexit( _glfwTerminate_atexit );

    // Initialize joysticks
    _glfwInitJoysticks();

    // Start the timer
    _glfwInitTimer();

    return GL_TRUE;
}


//========================================================================
// Close window
//========================================================================

int _glfwPlatformTerminate( void )
{
    // Close OpenGL window
    glfwCloseWindow();

    // Terminate display
    _glfwTerminateDisplay();

    // Terminate joysticks
    _glfwTerminateJoysticks();

    return GL_TRUE;
}

