//========================================================================
// GLFW - An OpenGL framework
// File:        fullscreen.c
// Platform:    Any
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
// $Id: fullscreen.c,v 1.3 2003-10-30 20:41:14 marcus256 Exp $
//========================================================================

#include "internal.h"


//************************************************************************
//****                    GLFW user functions                         ****
//************************************************************************

//========================================================================
// glfwGetVideoModes() - Get a list of available video modes
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwGetVideoModes( GLFWvidmode *list,
    int maxcount )
{
    if( !_glfwInitialized || maxcount <= 0 || list == (GLFWvidmode*) 0 )
    {
        return 0;
    }

    return _glfwPlatformGetVideoModes( list, maxcount );
}


//========================================================================
// glfwGetDesktopMode() - Get the desktop video mode
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwGetDesktopMode( GLFWvidmode *mode )
{
    if( !_glfwInitialized || mode == (GLFWvidmode*) 0 )
    {
        return;
    }

    _glfwPlatformGetDesktopMode( mode );
}
