//========================================================================
// GLFW - An OpenGL framework
// File:        dos_fullscreen.c
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
// $Id: dos_fullscreen.c,v 1.1 2003-11-26 20:45:03 marcus256 Exp $
//========================================================================

#include "internal.h"


//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwBPP2RGB() - Convert BPP to RGB bits (based on "best guess")
//========================================================================

static void _glfwBPP2RGB( int bpp, int *r, int *g, int *b )
{
    int delta;
    int bpp2;

    // Special case: bpp = 32
    if( bpp == 32 ) bpp = 24;

    // Convert "bits per pixel" to red, green & blue sizes
    *r = *g = *b = bpp / 3;
    delta = bpp - (*r * 3);
    if( delta >= 1 )
    {
        *g = *g + 1;
    }
    if( delta == 2 )
    {
        *r = *r + 1;
    }
}




//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetVideoModes() - List available video modes
//========================================================================

int _glfwPlatformGetVideoModes( GLFWvidmode *list, int maxcount )
{
    if( maxcount <= 0 ) return 0;

    // Dummy...
    list[0].Width     = 640;
    list[0].Height    = 480;
    list[0].RedBits   = 8;
    list[0].GreenBits = 8;
    list[0].BlueBits  = 8;

    return 1;
}


//========================================================================
// _glfwPlatformGetDesktopMode() - Get the desktop video mode
//========================================================================

void _glfwPlatformGetDesktopMode( GLFWvidmode *mode )
{
    // Dummy...
    mode->Width     = 640;
    mode->Height    = 480;
    mode->RedBits   = 8;
    mode->GreenBits = 8;
    mode->BlueBits  = 8;
}
