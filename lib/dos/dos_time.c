//========================================================================
// GLFW - An OpenGL framework
// File:        dos_time.c
// Platforms:   DOS
// API version: 2.4
// Author:      Marcus Geelnard (marcus.geelnard at home.se)
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2003 Marcus Geelnard
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
// $Id: dos_time.c,v 1.1 2003-11-26 20:45:03 marcus256 Exp $
//========================================================================

#include "internal.h"



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwInitTimer() - Initialize timer
//========================================================================

int _glfwInitTimer( void )
{
    // TODO
    return 1;
}


//========================================================================
// _glfwTerminateTimer() - Terminate timer
//========================================================================

void _glfwTerminateTimer( void )
{
    // TODO
}


//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetTime() - Return timer value in seconds
//========================================================================

double _glfwPlatformGetTime( void )
{
    // TODO
    return 0.0;
}


//========================================================================
// _glfwPlatformSetTime() - Set timer value in seconds
//========================================================================

void _glfwPlatformSetTime( double t )
{
    // TODO
}


//========================================================================
// _glfwPlatformSleep() - Put a thread to sleep for a specified amount of
// time
//========================================================================

void _glfwPlatformSleep( double time )
{
    // TODO
}
