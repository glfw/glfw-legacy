//========================================================================
// GLFW - An OpenGL framework
// File:        init.c
// Platform:    Any
// API version: 2.4
// Author:      Marcus Geelnard (marcus.geelnard at home.se)
// WWW:         http://glfw.sourceforge.net
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
// marcus.geelnard at home.se
//------------------------------------------------------------------------
// $Id: init.c,v 1.2 2003-02-02 22:39:18 marcus256 Exp $
//========================================================================

#define _init_c_
#include "internal.h"


//************************************************************************
//****                    GLFW user functions                         ****
//************************************************************************

//========================================================================
// glfwInit() - Initialize various GLFW state
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwInit( void )
{
    // Is GLFW already initialized?
    if( _glfwInitialized )
    {
        return GL_TRUE;
    }

    // Window is not yet opened
    _glfwWin.Opened = GL_FALSE;

    // Default enable/disable settings
    _glfwWin.SysKeysDisabled = GL_FALSE;

    // Clear window hints
    _glfwWinHints.RefreshRate    = 0;
    _glfwWinHints.AccumRedBits   = 0;
    _glfwWinHints.AccumGreenBits = 0;
    _glfwWinHints.AccumBlueBits  = 0;
    _glfwWinHints.AccumAlphaBits = 0;
    _glfwWinHints.AuxBuffers     = 0;
    _glfwWinHints.Stereo         = 0;

    // Platform specific initialization
    if( !_glfwPlatformInit() )
    {
        return GL_FALSE;
    }

    // Form now on, GLFW state is valid
    _glfwInitialized = GL_TRUE;

    return GL_TRUE;
}



//========================================================================
// glfwTerminate() - Close window and kill all threads.
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwTerminate( void )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return;
    }

    // Platform specific termination
    if( !_glfwPlatformTerminate() )
    {
        return;
    }

    // GLFW is no longer initialized
    _glfwInitialized = GL_FALSE;
}


//========================================================================
// glfwGetVersion() - Get GLFW version
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwGetVersion( int *major, int *minor,
    int *rev )
{
    if( major != NULL ) *major = GLFW_VERSION_MAJOR;
    if( minor != NULL ) *minor = GLFW_VERSION_MINOR;
    if( rev   != NULL ) *rev   = GLFW_VERSION_REVISION;
}
