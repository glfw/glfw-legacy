//========================================================================
// GLFW - An OpenGL framework
// File:     amigaos_time.c
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
// _glfwInitTimer() - Initialize timer
//========================================================================

void _glfwInitTimer( void )
{
    ULONG            freq;
    struct EClockVal t;

    // Get current time
    freq = ReadEClock( &t );

    // Calculate resolution
    _glfwTimer.Resolution = 1.0 / (double) freq;

    // Convert to 64-bit integer
    _glfwTimer.t0 = (long long) t.ev_hi * (long long) 4294967296 +
                    (long long) t.ev_lo;
}


//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetTime() - Return timer value in seconds
//========================================================================

double _glfwPlatformGetTime( void )
{
    struct EClockVal t;
    long long        t64;

    // Get current time
    (void) ReadEClock( &t );

    // Convert to 64-bit integer
    t64 = (long long) t.ev_hi * (long long) 4294967296 +
          (long long) t.ev_lo;

    return (double)(t64 - _glfwTimer.t0) * _glfwTimer.Resolution;
}


//========================================================================
// _glfwPlatformSetTime() - Set timer value in seconds
//========================================================================

void _glfwPlatformSetTime( double t )
{
    struct EClockVal t0;
    long long        t64;

    // Get current time
    (void) ReadEClock( &t0 );

    // Convert to 64-bit integer
    t64 = (long long) t0.ev_hi * (long long) 4294967296 +
          (long long) t0.ev_lo;

    // Calulate new starting time
    _glfwTimer.t0 = t64 - (long long)(t/_glfwTimer.Resolution);
}


//========================================================================
// _glfwPlatformSleep() - Put a thread to sleep for a specified amount of
// time
//========================================================================

void _glfwPlatformSleep( double time )
{
    ULONG ticks;

    // Too short time?
    if( time <= 0.0 )
    {
        return;
    }

    // Calculate Delay ticks (should be 50 ticks per second)
    ticks = (ULONG) ((double)TICKS_PER_SECOND * time + 0.5);
    if( ticks == 0 )
    {
        ticks = 1;
    }

    // Put process to sleep
    Delay( ticks );
}
