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
// $Id: dos_window.c,v 1.4 2003-12-01 21:41:09 marcus256 Exp $
//========================================================================

#include "internal.h"



//************************************************************************
//****                           PC_HW                                ****
//************************************************************************

#include <dpmi.h>
#include <fcntl.h>
#include <sys/stat.h> /* for mode definitions */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/*
 * standard redirection
 */
static char *outname = "c:\\stdout00.txt";
static int h_out, h_outbak;
static char *errname = "c:\\stderr00.txt";
static int h_err, h_errbak;

int pc_open_stdout (void)
{
// tmpnam(outname);

 if ((h_out=open(outname, O_WRONLY | O_CREAT | O_TEXT | O_TRUNC, S_IREAD | S_IWRITE)) > 0) {
    h_outbak = dup(STDOUT_FILENO);
    fflush(stdout);
    dup2(h_out, STDOUT_FILENO);
 }

 return h_out;
}

void pc_close_stdout (void)
{
 FILE *f;
 char *line = alloca(512);

 if (h_out > 0) {
    dup2(h_outbak, STDOUT_FILENO);
    close(h_out);
    close(h_outbak);

    f = fopen(outname, "rt");
    while (fgets(line, 512, f)) {
          fputs(line, stdout);
    }
    fclose(f);

    remove(outname);
 }
}

int pc_open_stderr (void)
{
// tmpnam(errname);

 if ((h_err=open(errname, O_WRONLY | O_CREAT | O_TEXT | O_TRUNC, S_IREAD | S_IWRITE)) > 0) {
    h_errbak = dup(STDERR_FILENO);
    fflush(stderr);
    dup2(h_err, STDERR_FILENO);
 }

 return h_err;
}

void pc_close_stderr (void)
{
 FILE *f;
 char *line = alloca(512);

 if (h_err > 0) {
    dup2(h_errbak, STDERR_FILENO);
    close(h_err);
    close(h_errbak);

    f = fopen(errname, "rt");
    while (fgets(line, 512, f)) {
          fputs(line, stderr);
    }
    fclose(f);

    remove(errname);
 }
}



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
    GLint params[2];

    // Clear window resources
    _glfwWin.Visual  = NULL;
    _glfwWin.Context = NULL;
    _glfwWin.Buffer  = NULL;

    // For now, we only support 640x480, 800x600 and 1024x768
    if( (width*height) < (700*500) )
    {
        width  = 640;
        height = 480;
    }
    else if( (width*height) < (900*700) )
    {
        width  = 800;
        height = 600;
    }
    else
    {
        width  = 1024;
        height = 768;
    }

    // For now, we only support 5,6,5 and 8,8,8 color formats
    if( (redbits+greenbits+bluebits) < 20 )
    {
        redbits   = 5;
        greenbits = 6;
        bluebits  = 5;
    }
    else
    {
        redbits   = 8;
        greenbits = 8;
        bluebits  = 8;
    }

    // For now, we always set refresh rate = 0 (default)
    refreshrate = 0;

    // Create visual
    _glfwWin.Visual = DMesaCreateVisual(
                        width, height,
                        redbits+greenbits+bluebits,
                        refreshrate,
                        GL_TRUE,                    // Double buffer
                        GL_TRUE,                    // RGB mode
                        alphabits?GL_TRUE:GL_FALSE, // Alpha buffer?
                        depthbits,
                        stencilbits,
                        (accumredbits+accumgreenbits+
                         accumbluebits+accumalphabits)>>2
                      );
    if( _glfwWin.Visual == NULL )
    {
        printf("Unable to create visual\n");
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // stdout/stderr redirection
    pc_open_stdout();
    pc_open_stderr();

    // Create context
    _glfwWin.Context = DMesaCreateContext( _glfwWin.Visual, NULL );
    if( _glfwWin.Context == NULL )
    {
        printf("Unable to create context\n");
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Create buffer
    _glfwWin.Buffer = DMesaCreateBuffer( _glfwWin.Visual, 0, 0,
                                         width, height );
    if( _glfwWin.Buffer == NULL )
    {
        printf("Unable to create buffer\n");
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Make current context
    if( !DMesaMakeCurrent( _glfwWin.Context, _glfwWin.Buffer ) )
    {
        printf("Unable to make current context\n");
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Remember actual screen/window size
    _glfwWin.Width  = width;
    _glfwWin.Height = height;

    return GL_TRUE;
}


//========================================================================
// _glfwPlatformCloseWindow() - Properly kill the window/video display
//========================================================================

void _glfwPlatformCloseWindow( void )
{
    // Destroy buffer
    if( _glfwWin.Buffer != NULL )
    {
        DMesaDestroyBuffer( _glfwWin.Buffer );
        _glfwWin.Buffer = NULL;
    }

    // Destroy context
    if( _glfwWin.Context != NULL )
    {
        DMesaDestroyContext( _glfwWin.Context );
        _glfwWin.Context = NULL;
    }

    // Destroy visual
    if( _glfwWin.Visual != NULL )
    {
        DMesaDestroyVisual( _glfwWin.Visual );
        _glfwWin.Visual = NULL;

        // stdout/stderr redirection
        pc_close_stdout();
        pc_close_stderr();
    }
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
    DMesaSwapBuffers( _glfwWin.Buffer );
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
