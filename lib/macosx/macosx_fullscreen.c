//========================================================================
// GLFW - An OpenGL framework
// File:     platform.h
// Platform: Mac OS X
// Version:  2.4
// Date:     2002.12.31
// Author:   Marcus Geelnard (marcus.geelnard@home.se)
//           Keith Bauer (onesadcookie@hotmail.com)
// WWW:      http://hem.passagen.se/opengl/glfw/
//------------------------------------------------------------------------
// Copyright (c) 2002 Marcus Geelnard
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
