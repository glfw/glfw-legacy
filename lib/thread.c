//========================================================================
// GLFW - An OpenGL framework
// File:     thread.c
// Platform: Any
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
// _glfwGetThreadPointer() - Find pointer to thread with a matching ID
//========================================================================

_GLFWthread * _glfwGetThreadPointer( int ID )
{
    _GLFWthread *t;

    for( t = &_glfwThrd.First; t != NULL; t = t->Next )
    {
        if( t->ID == ID )
        {
            break;
        }
    }

    return t;
}


//========================================================================
// _glfwAppendThread() - Append thread to thread list
//========================================================================

void _glfwAppendThread( _GLFWthread * t )
{
    _GLFWthread *t_tmp;

    t_tmp = &_glfwThrd.First;
    while( t_tmp->Next != NULL )
    {
        t_tmp = t_tmp->Next;
    }
    t_tmp->Next = t;
    t->Previous = t_tmp;
    t->Next     = NULL;
}


//========================================================================
// _glfwRemoveThread() - Remove thread from thread list
//========================================================================

void _glfwRemoveThread( _GLFWthread * t )
{
    if( t->Previous != NULL )
    {
        t->Previous->Next = t->Next;
    }
    if( t->Next != NULL )
    {
        t->Next->Previous = t->Previous;
    }
    free( (void *) t );
}



//************************************************************************
//****                     GLFW user functions                        ****
//************************************************************************

//========================================================================
// glfwCreateThread() - Create a new thread
//========================================================================

GLFWAPI GLFWthread GLFWAPIENTRY glfwCreateThread( GLFWthreadfun fun,
    void *arg )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return -1;
    }

    // Return the GLFW thread ID
    return _glfwPlatformCreateThread( fun, arg );
}


//========================================================================
// glfwDestroyThread() - Kill a thread. NOTE: THIS IS A VERY DANGEROUS
// OPERATION, AND SHOULD NOT BE USED EXCEPT IN EXTREME SITUATIONS!
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwDestroyThread( GLFWthread ID )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return;
    }

    // Is it a valid thread? (killing the main thread is not allowed)
    if( ID < 1 )
    {
        return;
    }

    _glfwPlatformDestroyThread( ID );
}


//========================================================================
// glfwWaitThread() - Wait for a thread to die
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwWaitThread( GLFWthread ID, int waitmode )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return GL_TRUE;
    }

    // Is it a valid thread? (waiting for the main thread is not allowed)
    if( ID < 1 )
    {
        return GL_TRUE;
    }

    return _glfwPlatformWaitThread( ID, waitmode );
}


//========================================================================
// glfwGetThreadID() - Return the thread ID for the current thread
//========================================================================

GLFWAPI GLFWthread GLFWAPIENTRY glfwGetThreadID( void )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return 0;
    }

    return _glfwPlatformGetThreadID();
}


//========================================================================
// glfwCreateMutex() - Create a mutual exclusion object
//========================================================================

GLFWAPI GLFWmutex GLFWAPIENTRY glfwCreateMutex( void )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return (GLFWmutex) 0;
    }

    return _glfwPlatformCreateMutex();
}


//========================================================================
// glfwDestroyMutex() - Destroy a mutual exclusion object
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwDestroyMutex( GLFWmutex mutex )
{
    // Initialized & valid mutex (no real way of assuring this)?
    if( !_glfwInitialized || !mutex )
    {
        return;
    }

    _glfwPlatformDestroyMutex( mutex );
}


//========================================================================
// glfwLockMutex() - Request access to a mutex
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwLockMutex( GLFWmutex mutex )
{
    // Initialized & valid mutex (no real way of assuring this)?
    if( !_glfwInitialized && !mutex )
    {
        return;
    }

    _glfwPlatformLockMutex( mutex );
}


//========================================================================
// glfwUnlockMutex() - Release a mutex
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwUnlockMutex( GLFWmutex mutex )
{
    // Initialized & valid mutex (no real way of assuring this)?
    if( !_glfwInitialized && !mutex )
    {
        return;
    }

    _glfwPlatformUnlockMutex( mutex );
}


//========================================================================
// glfwCreateCond() - Create a new condition variable object
//========================================================================

GLFWAPI GLFWcond GLFWAPIENTRY glfwCreateCond( void )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return (GLFWcond) 0;
    }

    return _glfwPlatformCreateCond();
}


//========================================================================
// glfwDestroyCond() - Destroy a condition variable object
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwDestroyCond( GLFWcond cond )
{
    // Initialized & valid condition variable?
    if( !_glfwInitialized || !cond )
    {
        return;
    }

    _glfwPlatformDestroyCond( cond );
}


//========================================================================
// glfwWaitCond() - Wait for a condition to be raised
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwWaitCond( GLFWcond cond, GLFWmutex mutex,
    double timeout )
{
    // Initialized & valid condition variable and mutex?
    if( !_glfwInitialized || !cond || !mutex )
    {
        return;
    }

    _glfwPlatformWaitCond( cond, mutex, timeout );
}


//========================================================================
// glfwSignalCond() - Signal a condition to one waiting thread
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwSignalCond( GLFWcond cond )
{
    // Initialized & valid condition variable?
    if( !_glfwInitialized || !cond )
    {
        return;
    }

    _glfwPlatformSignalCond( cond );
}


//========================================================================
// glfwBroadcastCond() - Broadcast a condition to all waiting threads
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwBroadcastCond( GLFWcond cond )
{
    // Initialized & valid condition variable?
    if( !_glfwInitialized || !cond )
    {
        return;
    }

    _glfwPlatformBroadcastCond( cond );
}


//========================================================================
// glfwGetNumberOfProcessors() - Return the number of processors in the
// system. This information can be useful for determining the optimal
// number of threads to use for performing a certain task.
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwGetNumberOfProcessors( void )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return 0;
    }

    return _glfwPlatformGetNumberOfProcessors();
}
