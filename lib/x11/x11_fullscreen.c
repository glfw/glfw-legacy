//========================================================================
// GLFW - An OpenGL framework
// File:        x11_fullscreen.c
// Platform:    X11 (Unix)
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
// $Id: x11_fullscreen.c,v 1.2 2003-02-02 21:24:56 marcus256 Exp $
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

    // Special case: BPP = 32 (I don't think this is necessary for X11??)
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


//========================================================================
// _glfwGetClosestVideoMode()
//========================================================================

int _glfwGetClosestVideoMode( int *w, int *h )
{
#ifdef _GLFW_HAS_XF86VIDMODE
    XF86VidModeModeInfo **modelist;
    int modecount, i, bestmode, bestmatch, match;
#endif

#ifdef _GLFW_HAS_XF86VIDMODE
    // Use the XF86VidMode extension to control video resolution

    // Get a list of all available display modes
    XF86VidModeGetAllModeLines( _glfwWin.Dpy, DefaultScreen(_glfwWin.Dpy),
                                &modecount, &modelist );

    // Find the best matching mode
    bestmode  = -1;
    bestmatch = 999999;
    for( i = 0; i < modecount; i ++ )
    {
        match = (*w - modelist[i]->hdisplay) *
                (*w - modelist[i]->hdisplay) +
                (*h - modelist[i]->vdisplay) *
                (*h - modelist[i]->vdisplay);
        if( match < bestmatch )
        {
            bestmatch = match;
            bestmode  = i;
        }
    }

    // Report width & height of best matching mode
    *w = modelist[ bestmode ]->hdisplay;
    *h = modelist[ bestmode ]->vdisplay;

    // Free modelist
    XFree( modelist );

    return bestmode;

#elif defined( _GLFW_HAS_XSGIVC )
    // Use the XSGIvc extension to control video resolution
    // * NOT SUPPORTED YET *

#else
    // Default: Simply use the screen resolution
    *w = DisplayWidth( _glfwWin.Dpy, DefaultScreen(_glfwWin.Dpy) );
    *h = DisplayHeight( _glfwWin.Dpy, DefaultScreen(_glfwWin.Dpy) );

    return 0;

#endif

}


//========================================================================
// _glfwSetVideoModeMODE() - Change the current video mode
//========================================================================

void _glfwSetVideoModeMODE( int mode )
{
#ifdef _GLFW_HAS_XF86VIDMODE
    XF86VidModeModeInfo **modelist;
    int modecount;
#endif

#ifdef _GLFW_HAS_XF86VIDMODE
    // Use the XF86VidMode extension to control video resolution

    // Get a list of all available display modes
    XF86VidModeGetAllModeLines( _glfwWin.Dpy, DefaultScreen(_glfwWin.Dpy),
                                &modecount, &modelist );

    // Unlock mode switch if necessary
    if( _glfwFS.ModeChanged )
    {
        XF86VidModeLockModeSwitch( _glfwWin.Dpy,
                                   DefaultScreen(_glfwWin.Dpy),
                                   0 );
    }

    // Change the video mode to the desired mode
    XF86VidModeSwitchToMode(  _glfwWin.Dpy, DefaultScreen(_glfwWin.Dpy),
                              modelist[ mode ] );

    // Set viewport to upper left corner (where our window will be)
    XF86VidModeSetViewPort( _glfwWin.Dpy, DefaultScreen(_glfwWin.Dpy),
                            0, 0 );

    // Lock mode switch
    XF86VidModeLockModeSwitch( _glfwWin.Dpy, DefaultScreen(_glfwWin.Dpy),
                               1 );

    // Remember old mode and flag that we have changed the mode
    if( !_glfwFS.ModeChanged )
    {
        _glfwFS.OldMode = *modelist[ 0 ];
        _glfwFS.ModeChanged = GL_TRUE;
    }

    // Free mode list
    XFree( modelist );

#elif defined( _GLFW_HAS_XSGIVC )
    // Use the XSGIvc extension to control video resolution
    // * NOT SUPPORTED YET *

#else
    // Default: Nothing to do...

#endif

}


//========================================================================
// _glfwSetVideoMode() - Change the current video mode
//========================================================================

void _glfwSetVideoMode( int *w, int *h )
{
    int     bestmode;

    // Find a best match mode
    bestmode = _glfwGetClosestVideoMode( w, h );

    // Change mode
    _glfwSetVideoModeMODE( bestmode );
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetVideoModes() - List available video modes
//========================================================================

int _glfwPlatformGetVideoModes( GLFWvidmode *list, int maxcount )
{
    int     count, i, j, k, r, g, b, rgba, gl;
    int     width, height, depth, bpp, m1, m2;
    Display *dpy;
    int     scrnid;
    XVisualInfo *vislist, dummy;
    int     viscount;
#if defined( _GLFW_HAS_XF86VIDMODE )
    XF86VidModeModeInfo **modelist;
    int     modecount, l;
#endif

    // Get display and screen
    dpy = XOpenDisplay( 0 );
    if( !dpy )
    {
        return 0;
    }
    scrnid = DefaultScreen( dpy );

#if defined( _GLFW_HAS_XF86VIDMODE )
    // Use the XF86VidMode extension to get list of video resolutions
    XF86VidModeGetAllModeLines( dpy, scrnid, &modecount, &modelist );
#endif

    // Get list of visuals
    vislist = XGetVisualInfo( dpy, 0, &dummy, &viscount );
    if( vislist == NULL )
    {
#if defined( _GLFW_HAS_XF86VIDMODE )
        XFree( modelist );
#endif
        XCloseDisplay( dpy );
        return 0;
    }

    // Loop through all visuals (and possibly resolutions), and extract
    // all the UNIQUE modes
    count = 0;
    for( k = 0; k < viscount && count < maxcount; k ++ )
    {
        // Does the visual support OpenGL & true color?
        glXGetConfig( dpy, &vislist[k], GLX_USE_GL, &gl );
        glXGetConfig( dpy, &vislist[k], GLX_RGBA, &rgba );
        if( gl && rgba )
        {
            // Get color depth for this visual
            depth = vislist[k].depth;

            // Convert to RGB, and back to bpp ("mask out" alpha bits etc)
            _glfwBPP2RGB( depth, &r, &g, &b );
            depth = r + g + b;

#if defined( _GLFW_HAS_XF86VIDMODE )
            for( l = 0; l < modecount && count < maxcount; l ++ )
            {
                width  = modelist[ l ]->hdisplay;
                height = modelist[ l ]->vdisplay;
#else
                width  = DisplayWidth( dpy, scrnid );
                height = DisplayHeight( dpy, scrnid );
#endif

                // Mode "code" for this mode
                m1 = (depth << 25) | (width * height);

                // Insert mode in list (sorted), and avoid duplicates
                m2 = 0;
                for( i = 0; i < count; i ++ )
                {
                    // Mode "code" for already listed mode
                    bpp = list[i].RedBits + list[i].GreenBits +
                          list[i].BlueBits;
                    m2 = (bpp << 25) | (list[i].Width * list[i].Height);
                    if( m1 <= m2 )
                    {
                        break;
                    }
                }

                // New entry at the end of the list?
                if( i >= count && count < maxcount )
                {
                    list[count].Width     = width;
                    list[count].Height    = height;
                    list[count].RedBits   = r;
                    list[count].GreenBits = g;
                    list[count].BlueBits  = b;
                    count ++;
                }
                // Insert new entry in the list?
                else if( m1 < m2  && count < maxcount )
                {
                    for( j = count; j > i; j -- )
                    {
                        list[j] = list[j-1];
                    }
                    list[i].Width     = width;
                    list[i].Height    = height;
                    list[i].RedBits   = r;
                    list[i].GreenBits = g;
                    list[i].BlueBits  = b;
                    count ++;
                }
#if defined( _GLFW_HAS_XF86VIDMODE )
            }
#endif
        }
    }

    // Free visuals list
    XFree( vislist );

#if defined( _GLFW_HAS_XF86VIDMODE )
    // Free list
    XFree( modelist );
#endif

    // Close display connection
    XCloseDisplay( dpy );

    return count;
}


//========================================================================
// _glfwPlatformGetDesktopMode() - Get the desktop video mode
//========================================================================

void _glfwPlatformGetDesktopMode( GLFWvidmode *mode )
{
    Display *dpy;
    int     scrnid, bpp;
#if defined( _GLFW_HAS_XF86VIDMODE )
    XF86VidModeModeInfo **modelist;
    int     modecount;
#endif

    // Get display and screen
    dpy = XOpenDisplay( 0 );
    if( !dpy )
    {
        mode->Width = mode->Height = mode->RedBits = mode->GreenBits =
        mode->BlueBits = 0;
        return;
    }
    scrnid = DefaultScreen( dpy );

#if defined( _GLFW_HAS_XF86VIDMODE )
    if( _glfwFS.ModeChanged )
    {
        // The old (desktop) mode is stored in _glfwFS.OldMode
        mode->Width  = _glfwFS.OldMode.hdisplay;
        mode->Height = _glfwFS.OldMode.vdisplay;
    }
    else
    {
        // Use the XF86VidMode extension to get list of video resolutions
        XF86VidModeGetAllModeLines( dpy, scrnid, &modecount, &modelist );

        // The first mode in the list is the current mode (desktop mode)
        mode->Width  = modelist[ 0 ]->hdisplay;
        mode->Height = modelist[ 0 ]->vdisplay;

        // Free list
        XFree( modelist );
    }
#else
    // Get current display width and height
    mode->Width  = DisplayWidth( dpy, scrnid );
    mode->Height = DisplayHeight( dpy, scrnid );
#endif

    // Get display depth
    bpp = DefaultDepth( dpy, scrnid );

    // Convert BPP to RGB bits
    _glfwBPP2RGB( bpp, &mode->RedBits, &mode->GreenBits,
                  &mode->BlueBits );

    // Close display connection
    XCloseDisplay( dpy );
}
