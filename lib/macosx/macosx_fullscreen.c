//========================================================================
// GLFW - An OpenGL framework
// File:        macosx_fullscreen.c
// Platform:    Mac OS X
// API Version: 2.4
// Authors:     Keith Bauer (onesadcookie at hotmail.com)
//              Camilla Drefvenborg (elmindreda at home.se)
//              Marcus Geelnard (marcus.geelnard at home.se)
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
// $Id: macosx_fullscreen.c,v 1.3 2003-10-30 20:49:51 marcus256 Exp $
//========================================================================

#include "internal.h"

void _glfwCGToGLFWVideoMode(CFDictionaryRef cgMode,
                            GLFWvidmode* glfwMode)
{
    CFNumberGetValue(CFDictionaryGetValue(cgMode, kCGDisplayWidth),
                     kCFNumberIntType,
                     &(glfwMode->Width));
    CFNumberGetValue(CFDictionaryGetValue(cgMode, kCGDisplayHeight),
                     kCFNumberIntType,
                     &(glfwMode->Height));

    int bitsPerSample;
    CFNumberGetValue(CFDictionaryGetValue(cgMode, kCGDisplayBitsPerSample),
                     kCFNumberIntType,
                     &bitsPerSample);

    glfwMode->RedBits = bitsPerSample;
    glfwMode->GreenBits = bitsPerSample;
    glfwMode->BlueBits = bitsPerSample;
}

int  _glfwPlatformGetVideoModes( GLFWvidmode *list, int maxcount )
{
    CFArrayRef availableModes = CGDisplayAvailableModes( kCGDirectMainDisplay );
    CFIndex numberOfAvailableModes = CFArrayGetCount( availableModes );

    int maxModes = ( numberOfAvailableModes < maxcount ?
                     numberOfAvailableModes :
                     maxcount );

    int i;
    for ( i = 0; i < maxModes; ++i )
    {
        _glfwCGToGLFWVideoMode( CFArrayGetValueAtIndex( availableModes, i ),
                                &( list[i] ) );
    }

    // TO DO: eliminate "duplicate" modes
    // TO DO: sort the video modes before returning.

    return maxModes;
}

void _glfwPlatformGetDesktopMode( GLFWvidmode *mode )
{
    _glfwCGToGLFWVideoMode( _glfwDesktopVideoMode, mode );
}
