//========================================================================
// GLFW - An OpenGL framework
// File:     x11_init.c
// Platform: X11 (Unix)
// Version:  2.4
// Date:     2003.01.08
// Author:   Marcus Geelnard (marcus.geelnard@home.se)
// WWW:      http://hem.passagen.se/opengl/glfw/
//------------------------------------------------------------------------
// Copyright (c) 2002-2003 Marcus Geelnard
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
//========================================================================

#include "internal.h"



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

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


//========================================================================
// _glfwTerminateThreads() - Terminate GLFW thread package
//========================================================================

static void _glfwTerminateThreads( void )
{
    _GLFWthread *t, *t_next;

    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // Kill all threads (NOTE: THE USER SHOULD WAIT FOR ALL THREADS TO
    // DIE, _BEFORE_ CALLING glfwTerminate()!!!)
    t = _glfwThrd.First.Next;
    while( t != NULL )
    {
        // Get pointer to next thread
        t_next = t->Next;

        // Simply murder the process, no mercy!
        pthread_kill( t->PosixID, SIGKILL );

        // Free memory allocated for this thread
        free( (void *) t );

        // Select next thread in list
        t = t_next;
    }

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION

    // Delete critical section handle
    pthread_mutex_destroy( &_glfwThrd.CriticalSection );
}


//========================================================================
// _glfwInitLibraries() - Dynamically load libraries
//========================================================================

#ifdef _GLFW_DLOPEN_LIBGL
#define _GLFW_NUM_LIBGL_NAMES 4
static char * _glfw_libGL_name[ _GLFW_NUM_LIBGL_NAMES ] =
{
    "libGL.so",
    "libGL.so.1",
    "/usr/lib/libGL.so",
    "/usr/lib/libGL.so.1"
};
#endif

void _glfwInitLibraries( void )
{
#ifdef _GLFW_DLOPEN_LIBGL
    int i;

    _glfwLibs.libGL = NULL;
    for( i = 0; !_glfwLibs.libGL && (i < _GLFW_NUM_LIBGL_NAMES); i ++ )
    {
        _glfwLibs.libGL = dlopen( _glfw_libGL_name[ i ],
                                  RTLD_LAZY | RTLD_GLOBAL );
    }
#endif
}


//========================================================================
// _glfwTerminate_atexit() - Terminate GLFW when exiting application
//========================================================================

void _glfwTerminate_atexit( void )
{
    glfwTerminate();
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformInit() - Initialize various GLFW state
//========================================================================

int _glfwPlatformInit( void )
{
    // Initialize thread package
    _glfwInitThreads();

    // Try to load libGL.so if necessary
    _glfwInitLibraries();

    // Install atexit() routine
    atexit( _glfwTerminate_atexit );

    // Initialize joysticks
    _glfwInitJoysticks();

    // Start the timer
    _glfwInitTimer();

    return GL_TRUE;
}


//========================================================================
// _glfwPlatformTerminate() - Close window and kill all threads
//========================================================================

int _glfwPlatformTerminate( void )
{
    // Only the main thread is allowed to do this...
    if( pthread_self() != _glfwThrd.First.PosixID )
    {
        return GL_FALSE;
    }

    // Close OpenGL window
    glfwCloseWindow();

    // Kill thread package
    _glfwTerminateThreads();

    // Terminate joysticks
    _glfwTerminateJoysticks();

    // Unload libGL.so if necessary
#ifdef _GLFW_DLOPEN_LIBGL
    if( _glfwLibs.libGL != NULL )
    {
        dlclose( _glfwLibs.libGL );
        _glfwLibs.libGL = NULL;
    }
#endif

    return GL_TRUE;
}
