//========================================================================
// GLFW - An OpenGL framework
// File:        win32_init.c
// Platform:    Windows
// API version: 2.4
// Author:      Marcus Geelnard (marcus.geelnard at home.se)
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2004 Marcus Geelnard
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
// Marcus Geelnard
// marcus.geelnard at home.se
//------------------------------------------------------------------------
// $Id: win32_init.c,v 1.6 2004-04-05 19:41:56 marcus256 Exp $
//========================================================================

#include "internal.h"

// With the Borland C++ compiler, we want to disable FPU exceptions
#ifdef __BORLANDC__
#include <float.h>
#endif // __BORLANDC__



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwInitLibraries() - Load necessary libraries (DLLs)
//========================================================================

static int _glfwInitLibraries( void )
{
    // gdi32.dll (OpenGL pixel format functions & SwapBuffers)
#ifndef _GLFW_NO_DLOAD_GDI32
    _glfwLibs.gdi32 = LoadLibrary( "gdi32.dll" );
    if( _glfwLibs.gdi32 != NULL )
    {
        _glfwLibs.ChoosePixelFormat   = (CHOOSEPIXELFORMAT_T)
            GetProcAddress( _glfwLibs.gdi32, "ChoosePixelFormat" );
        _glfwLibs.DescribePixelFormat = (DESCRIBEPIXELFORMAT_T)
            GetProcAddress( _glfwLibs.gdi32, "DescribePixelFormat" );
        _glfwLibs.GetPixelFormat      = (GETPIXELFORMAT_T)
            GetProcAddress( _glfwLibs.gdi32, "GetPixelFormat" );
        _glfwLibs.SetPixelFormat      = (SETPIXELFORMAT_T)
            GetProcAddress( _glfwLibs.gdi32, "SetPixelFormat" );
        _glfwLibs.SwapBuffers         = (SWAPBUFFERS_T)
            GetProcAddress( _glfwLibs.gdi32, "SwapBuffers" );
        if( _glfwLibs.ChoosePixelFormat   == NULL ||
            _glfwLibs.DescribePixelFormat == NULL ||
            _glfwLibs.GetPixelFormat      == NULL ||
            _glfwLibs.SetPixelFormat      == NULL ||
            _glfwLibs.SwapBuffers         == NULL )
        {
            FreeLibrary( _glfwLibs.gdi32 );
            _glfwLibs.gdi32 = NULL;
            return GL_FALSE;
        }
    }
    else
    {
        return GL_FALSE;
    }
#endif // _GLFW_NO_DLOAD_GDI32

    // winmm.dll (for joystick and timer support)
#ifndef _GLFW_NO_DLOAD_WINMM
    _glfwLibs.winmm = LoadLibrary( "winmm.dll" );
    if( _glfwLibs.winmm != NULL )
    {
        _glfwLibs.joyGetDevCapsA = (JOYGETDEVCAPSA_T)
            GetProcAddress( _glfwLibs.winmm, "joyGetDevCapsA" );
        _glfwLibs.joyGetPos      = (JOYGETPOS_T)
            GetProcAddress( _glfwLibs.winmm, "joyGetPos" );
        _glfwLibs.joyGetPosEx    = (JOYGETPOSEX_T)
            GetProcAddress( _glfwLibs.winmm, "joyGetPosEx" );
        _glfwLibs.timeGetTime    = (TIMEGETTIME_T)
            GetProcAddress( _glfwLibs.winmm, "timeGetTime" );
        if( _glfwLibs.joyGetDevCapsA == NULL ||
            _glfwLibs.joyGetPos      == NULL ||
            _glfwLibs.joyGetPosEx    == NULL ||
            _glfwLibs.timeGetTime    == NULL )
        {
            FreeLibrary( _glfwLibs.winmm );
            _glfwLibs.winmm = NULL;
            return GL_FALSE;
        }
    }
    else
    {
        return GL_FALSE;
    }
#endif // _GLFW_NO_DLOAD_WINMM

    return GL_TRUE;
}


//========================================================================
// _glfwFreeLibraries() - Unload used libraries (DLLs)
//========================================================================

static void _glfwFreeLibraries( void )
{
    // gdi32.dll
#ifndef _GLFW_NO_DLOAD_GDI32
    if( _glfwLibs.gdi32 != NULL )
    {
        FreeLibrary( _glfwLibs.gdi32 );
        _glfwLibs.gdi32 = NULL;
    }
#endif // _GLFW_NO_DLOAD_GDI32

    // winmm.dll
#ifndef _GLFW_NO_DLOAD_WINMM
    if( _glfwLibs.winmm != NULL )
    {
        FreeLibrary( _glfwLibs.winmm );
        _glfwLibs.winmm = NULL;
    }
#endif // _GLFW_NO_DLOAD_WINMM
}


//========================================================================
// _glfwInitThreads() - Initialize GLFW thread package
//========================================================================

static void _glfwInitThreads( void )
{
    // Initialize critical section handle
    InitializeCriticalSection( &_glfwThrd.CriticalSection );

    // The first thread (the main thread) has ID 0
    _glfwThrd.NextID = 0;

    // Fill out information about the main thread (this thread)
    _glfwThrd.First.ID       = _glfwThrd.NextID ++;
    _glfwThrd.First.Function = NULL;
    _glfwThrd.First.Handle   = GetCurrentThread();
    _glfwThrd.First.WinID    = GetCurrentThreadId();
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
        if( TerminateThread( t->Handle, 0 ) )
        {
            // Close thread handle
            CloseHandle( t->Handle );

            // Free memory allocated for this thread
            free( (void *) t );
        }

        // Select next thread in list
        t = t_next;
    }

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION

    // Delete critical section handle
    DeleteCriticalSection( &_glfwThrd.CriticalSection );
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
    OSVERSIONINFO osi;

    // To make SetForegroundWindow() work as we want, we need to fiddle
    // with the FOREGROUNDLOCKTIMEOUT system setting (we do this as early
    // as possible in the hope of still being the foreground process)
    SystemParametersInfo( SPI_GETFOREGROUNDLOCKTIMEOUT, 0,
                          &_glfwSys.ForegroundLockTimeout, 0 );
    SystemParametersInfo( SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0,
                          SPIF_SENDCHANGE );

    // Check which OS version we are running
    osi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &osi );
    _glfwSys.WinVer = _GLFW_WIN_UNKNOWN;
    if( osi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
    {
        if( osi.dwMajorVersion == 4 && osi.dwMinorVersion < 10 )
        {
            _glfwSys.WinVer = _GLFW_WIN_95;
        }
        else if( osi.dwMajorVersion == 4 && osi.dwMinorVersion < 90 )
        {
            _glfwSys.WinVer = _GLFW_WIN_98;
        }
        else if( osi.dwMajorVersion == 4 && osi.dwMinorVersion == 90 )
        {
            _glfwSys.WinVer = _GLFW_WIN_ME;
        }
        else if( osi.dwMajorVersion >= 4 )
        {
            _glfwSys.WinVer = _GLFW_WIN_UNKNOWN_9x;
        }
    }
    else if( osi.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {
        if( osi.dwMajorVersion == 4 && osi.dwMinorVersion == 0 )
        {
            _glfwSys.WinVer = _GLFW_WIN_NT4;
        }
        else if( osi.dwMajorVersion == 5 && osi.dwMinorVersion == 0 )
        {
            _glfwSys.WinVer = _GLFW_WIN_2K;
        }
        else if( osi.dwMajorVersion == 5 && osi.dwMinorVersion == 1 )
        {
            _glfwSys.WinVer = _GLFW_WIN_XP;
        }
        else if( osi.dwMajorVersion == 5 && osi.dwMinorVersion == 2 )
        {
            _glfwSys.WinVer = _GLFW_WIN_NET_SERVER;
        }
        else if( osi.dwMajorVersion >= 5 )
        {
            _glfwSys.WinVer = _GLFW_WIN_UNKNOWN_NT;
        }
    }

    // Do we have Unicode support?
    if( _glfwSys.WinVer >= _GLFW_WIN_NT4 )
    {
        // Windows NT/2000/XP/.NET has Unicode support
        _glfwSys.HasUnicode = GL_TRUE;
    }
    else
    {
        // Windows 9x/ME does not have Unicode support
        _glfwSys.HasUnicode = GL_FALSE;
    }

    // Load libraries (DLLs)
    if( !_glfwInitLibraries() )
    {
        return GL_FALSE;
    }

    // With the Borland C++ compiler, we want to disable FPU exceptions
    // (this is recommended for OpenGL applications under Windows)
#ifdef __BORLANDC__
    _control87( MCW_EM, MCW_EM );
#endif

    // System keys are not disabled
    _glfwWin.KeyboardHook = NULL;

    // Initialise thread package
    _glfwInitThreads();

    // Install atexit() routine
    atexit( _glfwTerminate_atexit );

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
    if( GetCurrentThreadId() != _glfwThrd.First.WinID )
    {
        return GL_FALSE;
    }

    // Close OpenGL window
    glfwCloseWindow();

    // Kill thread package
    _glfwTerminateThreads();

    // Enable system keys again (if they were disabled)
    glfwEnable( GLFW_SYSTEM_KEYS );

    // Unload libraries (DLLs)
    _glfwFreeLibraries();

    // Restore FOREGROUNDLOCKTIMEOUT system setting
    SystemParametersInfo( SPI_SETFOREGROUNDLOCKTIMEOUT, 0,
                          (LPVOID)_glfwSys.ForegroundLockTimeout,
                          SPIF_SENDCHANGE );

    return GL_TRUE;
}
