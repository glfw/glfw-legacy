//========================================================================
// GLFW - An OpenGL framework
// File:        x11_fullscreen.c
// Platform:    X11 (Unix)
// API version: 2.5
// Author:      Marcus Geelnard (marcus.geelnard at home.se)
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2005 Marcus Geelnard
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
// $Id: x11_fullscreen.c,v 1.8 2005-03-14 20:26:17 marcus256 Exp $
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

int _glfwGetClosestVideoMode( int scrn, int *w, int *h )
{
#ifdef _GLFW_HAS_XF86VIDMODE
    XF86VidModeModeInfo **modelist;
    int modecount, i, bestmode, bestmatch, match;
#endif

    // Use the XF86VidMode extension to control video resolution?
#ifdef _GLFW_HAS_XF86VIDMODE
    if( _glfwDisplay.Has_XF86VidMode )
    {
        // Get a list of all available display modes
        XF86VidModeGetAllModeLines( _glfwDisplay.Dpy, scrn,
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
    }
    else
    {
        // Default: Simply use the screen resolution
        *w = DisplayWidth( _glfwDisplay.Dpy, scrn );
        *h = DisplayHeight( _glfwDisplay.Dpy, scrn );

        return 0;
    }
#else
    // Default: Simply use the screen resolution
    *w = DisplayWidth( _glfwDisplay.Dpy, scrn );
    *h = DisplayHeight( _glfwDisplay.Dpy, scrn );

    return 0;
#endif
}


//========================================================================
// _glfwSetVideoModeMODE() - Change the current video mode
//========================================================================

void _glfwSetVideoModeMODE( int scrn, int mode )
{
#ifdef _GLFW_HAS_XF86VIDMODE
    XF86VidModeModeInfo **modelist;
    int modecount;
#endif

    // Use the XF86VidMode extension to control video resolution?
#ifdef _GLFW_HAS_XF86VIDMODE
    if( _glfwDisplay.Has_XF86VidMode )
    {
        // Get a list of all available display modes
        XF86VidModeGetAllModeLines( _glfwDisplay.Dpy, scrn,
                                    &modecount, &modelist );

        // Unlock mode switch if necessary
        if( _glfwFS.ModeChanged )
        {
            XF86VidModeLockModeSwitch( _glfwDisplay.Dpy, scrn, 0 );
        }

        // Change the video mode to the desired mode
        XF86VidModeSwitchToMode(  _glfwDisplay.Dpy, scrn,
                                  modelist[ mode ] );

        // Set viewport to upper left corner (where our window will be)
        XF86VidModeSetViewPort( _glfwDisplay.Dpy, scrn, 0, 0 );

        // Lock mode switch
        XF86VidModeLockModeSwitch( _glfwDisplay.Dpy, scrn, 1 );

        // Remember old mode and flag that we have changed the mode
        if( !_glfwFS.ModeChanged )
        {
            _glfwFS.OldMode = *modelist[ 0 ];
            _glfwFS.ModeChanged = GL_TRUE;
        }

        // Free mode list
        XFree( modelist );
    }
#endif
}


//========================================================================
// _glfwSetVideoMode() - Change the current video mode
//========================================================================

void _glfwSetVideoMode( int scrn, int *w, int *h )
{
    int     bestmode;

    // Find a best match mode
    bestmode = _glfwGetClosestVideoMode( scrn, w, h );

    // Change mode
    _glfwSetVideoModeMODE( scrn, bestmode );
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetVideoModes() - List available video modes
//========================================================================

#define MAX_COLOR_MODES 32
#define MAX_RES_MODES   256

struct _res {
    int w,h;
};

int _glfwPlatformGetVideoModes( GLFWvidmode *list, int maxcount )
{
    int     count, k, l, r, g, b, rgba, gl;
    int     width, height, depth, scrn;
    Display *dpy;
    XVisualInfo *vislist, dummy;
    int     viscount, rgbcount, rescount;
    int     rgbarray[MAX_COLOR_MODES];
    struct _res resarray[MAX_RES_MODES];
#ifdef _GLFW_HAS_XF86VIDMODE
    XF86VidModeModeInfo **modelist;
    int     modecount;
#endif

    // Get display and screen
    dpy = _glfwDisplay.Dpy;
    scrn = DefaultScreen( dpy );

    // Get list of video resolutions (XF86VidMode)
#ifdef _GLFW_HAS_XF86VIDMODE
    if( _glfwDisplay.Has_XF86VidMode )
    {
        XF86VidModeGetAllModeLines( dpy, scrn, &modecount, &modelist );
    }
#endif

    // Get list of visuals
    vislist = XGetVisualInfo( dpy, 0, &dummy, &viscount );
    if( vislist == NULL )
    {
#ifdef _GLFW_HAS_XF86VIDMODE
        if( _glfwDisplay.Has_XF86VidMode ) XFree( modelist );
#endif
        return 0;
    }

    // Build RGB array
    rgbcount = 0;
    for( k = 0; k < viscount && rgbcount < MAX_COLOR_MODES; ++ k )
    {
        // Does the visual support OpenGL & true color?
        glXGetConfig( dpy, &vislist[k], GLX_USE_GL, &gl );
        glXGetConfig( dpy, &vislist[k], GLX_RGBA, &rgba );
        if( gl && rgba )
        {
            // Get color depth for this visual
            depth = vislist[k].depth;

            // Convert to RGB
            _glfwBPP2RGB( depth, &r, &g, &b );
            depth = (r<<16) | (g<<8) | b;

            // Is this mode unique?
            for( l = 0; l < rgbcount; ++ l )
            {
                if( depth == rgbarray[ l ] )
                {
                    break;
                }
            }
            if( l >= rgbcount )
            {
                rgbarray[ rgbcount ] = depth;
                ++ rgbcount;
            }
        }
    }

    // Build resolution array
#ifdef _GLFW_HAS_XF86VIDMODE
    if( _glfwDisplay.Has_XF86VidMode )
    {
        rescount = 0;
        for( k = 0; k < modecount && rescount < MAX_RES_MODES; ++ k )
        {
            width  = modelist[ k ]->hdisplay;
            height = modelist[ k ]->vdisplay;

            // Is this mode unique?
            for( l = 0; l < rescount; ++ l )
            {
                if( width == resarray[ l ].w && height == resarray[ l ].h )
                {
                    break;
                }
            }
            if( l >= rescount )
            {
                resarray[ rescount ].w = width;
                resarray[ rescount ].h = height;
                ++ rescount;
            }
        }
    }
    else
    {
        rescount = 1;
        resarray[ 0 ].w = DisplayWidth( dpy, scrn );
        resarray[ 0 ].h = DisplayHeight( dpy, scrn );
    }
#else
    rescount = 1;
    resarray[ 0 ].w = DisplayWidth( dpy, scrn );
    resarray[ 0 ].h = DisplayHeight( dpy, scrn );
#endif

    // Build permutations of colors and resolutions
    count = 0;
    for( k = 0; k < rgbcount && count < maxcount; ++ k )
    {
        for( l = 0; l < rescount && count < maxcount; ++ l )
        {
            list[count].Width     = resarray[ l ].w;
            list[count].Height    = resarray[ l ].h;
            list[count].RedBits   = (rgbarray[ k ] >> 16) & 255;
            list[count].GreenBits = (rgbarray[ k ] >> 8) & 255;
            list[count].BlueBits  = rgbarray[ k ] & 255;
            ++ count;
        }
    }

    // Free visuals list
    XFree( vislist );

#ifdef _GLFW_HAS_XF86VIDMODE
    // Free list
    if( _glfwDisplay.Has_XF86VidMode ) XFree( modelist );
#endif

    return count;
}


//========================================================================
// _glfwPlatformGetDesktopMode() - Get the desktop video mode
//========================================================================

void _glfwPlatformGetDesktopMode( GLFWvidmode *mode )
{
    Display *dpy;
    int     bpp, scrn;
#ifdef _GLFW_HAS_XF86VIDMODE
    XF86VidModeModeInfo **modelist;
    int     modecount;
#endif

    // Get display and screen
    dpy = _glfwDisplay.Dpy;
    scrn = DefaultScreen( dpy );

#ifdef _GLFW_HAS_XF86VIDMODE
    if( _glfwDisplay.Has_XF86VidMode )
    {
        if( _glfwFS.ModeChanged )
        {
            // The old (desktop) mode is stored in _glfwFS.OldMode
            mode->Width  = _glfwFS.OldMode.hdisplay;
            mode->Height = _glfwFS.OldMode.vdisplay;
        }
        else
        {
            // Use the XF86VidMode extension to get list of video modes
            XF86VidModeGetAllModeLines( dpy, scrn, &modecount,
                                        &modelist );

            // The first mode in the list is the current (desktio) mode
            mode->Width  = modelist[ 0 ]->hdisplay;
            mode->Height = modelist[ 0 ]->vdisplay;

            // Free list
            XFree( modelist );
        }
    }
    else
    {
        // Get current display width and height
        mode->Width  = DisplayWidth( dpy, scrn );
        mode->Height = DisplayHeight( dpy, scrn );
    }
#else
    // Get current display width and height
    mode->Width  = DisplayWidth( dpy, scrn );
    mode->Height = DisplayHeight( dpy, scrn );
#endif

    // Get display depth
    bpp = DefaultDepth( dpy, scrn );

    // Convert BPP to RGB bits
    _glfwBPP2RGB( bpp, &mode->RedBits, &mode->GreenBits,
                  &mode->BlueBits );
}
