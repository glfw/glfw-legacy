//========================================================================
// GLFW - An OpenGL framework
// File:        dos_window.c
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
// $Id: dos_window.c,v 1.2 2003-11-26 20:50:42 marcus256 Exp $
//========================================================================

#include "internal.h"


//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwTranslateChar() - Translates a DOS key code to Unicode
//========================================================================

static int _glfwTranslateChar( int keycode )
{
    // TODO
    return 'A';
}


//========================================================================
// _glfwTranslateKey() - Translates a DOS key code to internal coding
//========================================================================

static int _glfwTranslateKey( int keycode )
{
    // TODO
    return 'A';
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformOpenWindow() - Here is where the window is created, and
// the OpenGL rendering context is created
//========================================================================

int _glfwPlatformOpenWindow( int width, int height, int redbits,
    int greenbits, int bluebits, int alphabits, int depthbits,
    int stencilbits, int mode, int accumredbits, int accumgreenbits,
    int accumbluebits, int accumalphabits, int auxbuffers, int stereo,
    int refreshrate )
{
    // TODO
    return GL_FALSE;
}


//========================================================================
// _glfwPlatformCloseWindow() - Properly kill the window/video display
//========================================================================

void _glfwPlatformCloseWindow( void )
{
    // TODO
}


//========================================================================
// _glfwPlatformSetWindowTitle() - Set the window title.
//========================================================================

void _glfwPlatformSetWindowTitle( const char *title )
{
    // Nothing to do here...
}


//========================================================================
// _glfwPlatformSetWindowSize() - Set the window size.
//========================================================================

void _glfwPlatformSetWindowSize( int width, int height )
{
    // TODO
}


//========================================================================
// _glfwPlatformSetWindowPos() - Set the window position.
//========================================================================

void _glfwPlatformSetWindowPos( int x, int y )
{
    // Nothing to do here...
}


//========================================================================
// _glfwPlatformIconfyWindow() - Window iconification
//========================================================================

void _glfwPlatformIconifyWindow( void )
{
    // Nothing to do here...
}


//========================================================================
// _glfwPlatformRestoreWindow() - Window un-iconification
//========================================================================

void _glfwPlatformRestoreWindow( void )
{
    // Nothing to do here...
}


//========================================================================
// _glfwPlatformSwapBuffers() - Swap buffers (double-buffering) and poll
// any new events.
//========================================================================

void _glfwPlatformSwapBuffers( void )
{
    // TODO
}


//========================================================================
// _glfwPlatformSwapInterval() - Set double buffering swap interval
//========================================================================

void _glfwPlatformSwapInterval( int interval )
{
    // TODO
}


//========================================================================
// _glfwPlatformRefreshWindowParams()
//========================================================================

void _glfwPlatformRefreshWindowParams( void )
{
    GLint     x;
    GLboolean b;

    // TODO

    // Fill out information
    _glfwWin.Accelerated    = GL_TRUE;
    glGetIntegerv( GL_RED_BITS, &x );
    _glfwWin.RedBits        = x;
    glGetIntegerv( GL_GREEN_BITS, &x );
    _glfwWin.GreenBits      = x;
    glGetIntegerv( GL_BLUE_BITS, &x );
    _glfwWin.BlueBits       = x;
    glGetIntegerv( GL_ALPHA_BITS, &x );
    _glfwWin.AlphaBits      = x;
    glGetIntegerv( GL_DEPTH_BITS, &x );
    _glfwWin.DepthBits      = x;
    glGetIntegerv( GL_STENCIL_BITS, &x );
    _glfwWin.StencilBits    = x;
    glGetIntegerv( GL_ACCUM_RED_BITS, &x );
    _glfwWin.AccumRedBits   = x;
    glGetIntegerv( GL_ACCUM_GREEN_BITS, &x );
    _glfwWin.AccumGreenBits = x;
    glGetIntegerv( GL_ACCUM_BLUE_BITS, &x );
    _glfwWin.AccumBlueBits  = x;
    glGetIntegerv( GL_ACCUM_ALPHA_BITS, &x );
    _glfwWin.AccumAlphaBits = x;
    glGetIntegerv( GL_AUX_BUFFERS, &x );
    _glfwWin.AuxBuffers     = x;
    glGetBooleanv( GL_AUX_BUFFERS, &b );
    _glfwWin.Stereo         = b ? GL_TRUE : GL_FALSE;
    _glfwWin.RefreshRate    = 0;
}


//========================================================================
// _glfwPlatformPollEvents() - Poll for new window and input events
//========================================================================

void _glfwPlatformPollEvents( void )
{
    // TODO
}


/*
//========================================================================
// _glfwPlatformWaitEvents() - Wait for new window and input events
//========================================================================

void _glfwPlatformWaitEvents( void )
{
    // Wait for new window events
    // TODO

    // Process events
    _glfwPlatformPollEvents();
}
*/


//========================================================================
// _glfwPlatformHideMouseCursor() - Hide mouse cursor (lock it)
//========================================================================

void _glfwPlatformHideMouseCursor( void )
{
    // TODO
}


//========================================================================
// _glfwPlatformShowMouseCursor() - Show mouse cursor (unlock it)
//========================================================================

void _glfwPlatformShowMouseCursor( void )
{
    // TODO
}


//========================================================================
// _glfwPlatformSetMouseCursorPos() - Set physical mouse cursor position
//========================================================================

void _glfwPlatformSetMouseCursorPos( int x, int y )
{
    // TODO
}
