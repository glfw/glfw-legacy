//========================================================================
// GLFW - An OpenGL framework
// File:        macosx_init.c
// Platform:    Mac OS X
// API Version: 2.4
// Author:      Marcus Geelnard (marcus.geelnard@home.se)
//              Keith Bauer (onesadcookie@hotmail.com)
//              Camilla Drefvenborg (elmindreda@home.se)
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002 Marcus Geelnard
//
// GLFW is provided "as-is", without any express or implied warranty,
// without even the implied warranty of fitness for a particular purpose.
// In no event will the author be held liable for any damage araising from
// the use of GLFW.
//
// Permission is granted to anyone to use GLFW for any purpose, including
// commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. You must not claim that you wrote GLFW.
// 2. If you modify any parts of GLFW, it must be clearly stated which
//    modifications have been made, and that those modifications are not
//    the work of the author of GLFW.
// 3. If you use GLFW in a product, an acknowledgment in the product
//    documentation would be appreciated, but is not required.
// 4. This notice may not be removed or altered from any source
//    distribution of GLFW.
//
// Marcus Geelnard
// marcus.geelnard@home.se
//------------------------------------------------------------------------
// $Id: macosx_init.c,v 1.2 2003-10-20 23:02:13 marcus256 Exp $
//========================================================================

#include "internal.h"

//========================================================================
// _glfwInitThreads() - Initialize GLFW thread package
//========================================================================

static void _glfwInitThreads( void )
{
    // Initialize critical section handle
    (void) pthread_mutex_init( &_glfwThrd.CriticalSection, NULL );

    // The first thread (the main thread) has ID 0
    _glfwThrd.NextID = 0;

    // Fill out information about the main thread (this thread)
    _glfwThrd.First.ID       = _glfwThrd.NextID ++;
    _glfwThrd.First.Function = NULL;
    _glfwThrd.First.PosixID  = pthread_self();
    _glfwThrd.First.Previous = NULL;
    _glfwThrd.First.Next     = NULL;
}

int  _glfwChangeToResourcesDirectory( void )
{
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL( mainBundle );
    char resourcesPath[GLFW_MAX_PATH_LENGTH];

    if ( !CFURLGetFileSystemRepresentation( resourcesURL,
                                            TRUE,
                                            (UInt8*)resourcesPath,
                                            GLFW_MAX_PATH_LENGTH ) )
    {
        CFRelease( resourcesURL );
        return GL_FALSE;
    }

    CFRelease( resourcesURL );

    if ( chdir( resourcesPath ) != 0 )
    {
        return GL_FALSE;
    }

    return GL_TRUE;
}

int _glfwPlatformInit( void )
{
    _glfwLibs.OpenGLFramework
        = CFBundleGetBundleWithIdentifier( CFSTR( "com.apple.opengl" ) );
    if ( _glfwLibs.OpenGLFramework == NULL )
    {
        return GL_FALSE;
    }

    _glfwDesktopVideoMode = CGDisplayCurrentMode( kCGDirectMainDisplay );
    if ( _glfwDesktopVideoMode == NULL )
    {
        return GL_FALSE;
    }

    _glfwInitThreads();

    if ( !_glfwChangeToResourcesDirectory() )
    {
        return GL_FALSE;
    }

    if ( !_glfwInstallEventHandlers() )
    {
        return GL_FALSE;
    }

    _glfwTimer.t0 = GetCurrentEventTime();

    _glfwWin.MacWindow = NULL;
    _glfwWin.AGLContext = NULL;

    _glfwInput.Modifiers = 0;

    return GL_TRUE;
}

int _glfwPlatformTerminate( void )
{
    return GL_TRUE;
}
