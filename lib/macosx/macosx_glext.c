//========================================================================
// GLFW - An OpenGL framework
// File:        macosx_glext.c
// Platform:    Mac OS X
// API Version: 2.4
// Authors:     Keith Bauer (onesadcookie at hotmail.com)
//              Camilla Drefvenborg (elmindreda at home.se)
//              Marcus Geelnard (marcus.geelnard at home.se)
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
// $Id: macosx_glext.c,v 1.4 2004-02-14 20:55:00 marcus256 Exp $
//========================================================================

#include "internal.h"

int    _glfwPlatformExtensionSupported( const char *extension )
{
    // There are no AGL, CGL or NSGL extensions.
    return GL_FALSE;
}

void * _glfwPlatformGetProcAddress( const char *procname )
{
    CFStringRef symbolName = CFStringCreateWithCString( kCFAllocatorDefault,
                                                        procname,
                                                        kCFStringEncodingASCII );

    void *symbol = CFBundleGetFunctionPointerForName( _glfwLibs.OpenGLFramework,
                                                      symbolName );

    CFRelease( symbolName );

    return symbol;
}
