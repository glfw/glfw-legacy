//========================================================================
// GLFW - An OpenGL framework
// File:     amigaos_init.c
// Platform: AmigaOS (3.x)
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
// _glfwInitLibraries() - Load shared libraries
//========================================================================

static int _glfwInitLibraries( void )
{
    // Note: exec.library & dos.library are always opened (by C startup)

    // Start by clearing all handles
    GfxBase       = NULL;
    IntuitionBase = NULL;
    KeymapBase    = NULL;
    UtilityBase   = NULL;
    TimerBase     = NULL;
    TimerMP       = NULL;
    TimerIO       = NULL;

    // graphics.library
    GfxBase = (struct GfxBase *) OpenLibrary( "graphics.library", 40 );
    if( !GfxBase )
    {
        return 0;
    }

    // intuition.library
    IntuitionBase = (struct IntuitionBase *) OpenLibrary( "intuition.library", 40 );
    if( !IntuitionBase )
    {
        return 0;
    }

    // keymap.library
    KeymapBase = OpenLibrary( "keymap.library", 40 );
    if( !KeymapBase )
    {
        return 0;
    }

    // Utility.library
    UtilityBase = (struct UtilityBase *) OpenLibrary( "utility.library", 40 );
    if( !UtilityBase )
    {
        return 0;
    }

    // timer.device (used as a library for ReadEClock)
    if( TimerMP = CreatePort( NULL, 0 ) )
    {
        // Create the I/O request
        if( TimerIO = (struct timerequest *)
            CreateExtIO( TimerMP, sizeof(struct timerequest) ) )
        {
            // Open the timer device
            if( !( OpenDevice( "timer.device", UNIT_MICROHZ,
                               (struct IORequest *) TimerIO, 0 ) ) )
            {
                // Set up pointer for timer functions
                TimerBase = (struct Device *)TimerIO->tr_node.io_Device;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

    return 1;
}


//========================================================================
// _glfwTerminateLibraries() - Unload shared libraries
//========================================================================

static void _glfwTerminateLibraries( void )
{
    // Close graphics.library
    if( GfxBase )
    {
        CloseLibrary( (struct Library *) GfxBase );
        GfxBase = NULL;
    }

    // Close intuition.library
    if( IntuitionBase )
    {
        CloseLibrary( (struct Library *) IntuitionBase );
        IntuitionBase = NULL;
    }

    // Close keymap.library
    if( KeymapBase )
    {
        CloseLibrary( KeymapBase );
        KeymapBase = NULL;
    }

    // Close utility.library
    if( UtilityBase )
    {
        CloseLibrary( (struct Library *) UtilityBase );
        UtilityBase = NULL;
    }

    // Empty the timer.device message port queue
    if( TimerMP )
    {
        struct Message *msg;
        while( NULL != (msg = GetMsg( TimerMP )) ) ReplyMsg( msg );
    }

    // Close timer.device
    if( TimerBase )
    {
        CloseDevice( (struct IORequest *) TimerIO );
        TimerBase = NULL;
    }

    // Delete timer.device I/O request
    if( TimerIO )
    {
        DeleteExtIO( (struct IORequest *) TimerIO );
        TimerIO = NULL;
    }

    // Delete timer.device message port
    if( TimerMP )
    {
        DeletePort( TimerMP );
        TimerMP = NULL;
    }
}


//========================================================================
// _glfwInitThreads() - Initialize GLFW thread package
//========================================================================

static int _glfwInitThreads( void )
{
    int waitSig;

    // Allocate a signal to use for waiting (glfwWaitThread and
    // glfwWaitCond)
    waitSig = AllocSignal( -1 );
    if( waitSig == -1 )
    {
        return 0;
    }

    // Initialize critical section handle
    memset(&_glfwThrd.CriticalSection,0,sizeof(struct SignalSemaphore));
    InitSemaphore( &_glfwThrd.CriticalSection );

    // The first thread (the main thread) has ID 0
    _glfwThrd.NextID = 0;

    // The first condition variable has ID 0xFFFFFFFF
    _glfwThrd.NextCondID = 0xFFFFFFFF;

    // Fill out information about the main thread (this thread)
    _glfwThrd.First.Previous = NULL;
    _glfwThrd.First.Next     = NULL;
    _glfwThrd.First.ID       = _glfwThrd.NextID ++;
    _glfwThrd.First.Function = NULL;
    _glfwThrd.First.Arg      = NULL;
    _glfwThrd.First.AmiTask  = FindTask( NULL );
    _glfwThrd.First.AmiProc  = (struct Process *) _glfwThrd.First.AmiTask;
    _glfwThrd.First.WaitFor  = NULL;
    _glfwThrd.First.WaitSig  = waitSig;

    // Store GLFW thread struct pointer in task user data
    _glfwThrd.First.AmiTask->tc_UserData = (APTR) &_glfwThrd.First;

    return 1;
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
        // ?? How about Process resources ??
        RemTask( t->AmiTask );

        // Free memory allocated for this thread
        free( (void *) t );

        // Select next thread in list
        t = t_next;
    }

    // Free waiting signal for main thread
    FreeSignal( _glfwThrd.First.WaitSig );

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION
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
    // Load shared libraries
    if( !_glfwInitLibraries() )
    {
        _glfwTerminateLibraries();
        return GL_FALSE;
    }

    // Initialize thread package
    if( !_glfwInitThreads() )
    {
        _glfwTerminateLibraries();
        return GL_FALSE;
    }

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
    if( FindTask( NULL ) != _glfwThrd.First.AmiTask )
    {
        return GL_FALSE;
    }

    // Close OpenGL window
    glfwCloseWindow();

    // Kill thread package
    _glfwTerminateThreads();

    // Unload shared libraries
    _glfwTerminateLibraries();

    return GL_TRUE;
}
