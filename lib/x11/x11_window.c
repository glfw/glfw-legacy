//========================================================================
// GLFW - An OpenGL framework
// File:        x11_window.c
// Platform:    X11 (Unix)
// API version: 2.7
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Camilla Berglund
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
//========================================================================

#include "internal.h"


/* KDE decoration values */
enum {
  KDE_noDecoration = 0,
  KDE_normalDecoration = 1,
  KDE_tinyDecoration = 2,
  KDE_noFocus = 256,
  KDE_standaloneMenuBar = 512,
  KDE_desktopIcon = 1024 ,
  KDE_staysOnTop = 2048
};


/* Define GLX 1.4 FSAA tokens if not already defined */
#ifndef GLX_VERSION_1_4

#define GLX_SAMPLE_BUFFERS  100000
#define GLX_SAMPLES         100001

#endif /*GLX_VERSION_1_4*/


/* Declare GLX_ARB_create_context API if not already declared */
#ifndef GLX_ARB_create_context

/* Tokens for glXCreateContextAttribs */
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#define GLX_CONTEXT_FLAGS_ARB 0x2094
#define GLX_CONTEXT_DEBUG_BIT_ARB 0x0001
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

/* Prototype for glXCreateContextAttribs */
typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)( Display *display,
                                                         GLXFBConfig config,
                                                         GLXContext share_context,
                                                         Bool direct,
                                                         const int *attrib_list);

#endif /*GLX_ARB_create_context*/


//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// Checks whether the event is a MapNotify for the specified window
//========================================================================

static Bool IsMapNotify( Display *d, XEvent *e, char *arg )
{
    return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
}


//========================================================================
// Turn off window decorations
// Based on xawdecode: src/wmhooks.c
//========================================================================

#define MWM_HINTS_DECORATIONS (1L << 1)

static void disableDecorations( void )
{
    GLboolean removedDecorations;
    Atom hintAtom;
    XSetWindowAttributes attributes;

    removedDecorations = GL_FALSE;

    // First try to set MWM hints
    hintAtom = XInternAtom( _glfwLibrary.display, "_MOTIF_WM_HINTS", True );
    if ( hintAtom != None )
    {
        struct {
            unsigned long flags;
            unsigned long functions;
            unsigned long decorations;
                     long input_mode;
            unsigned long status;
        } MWMHints = { MWM_HINTS_DECORATIONS, 0, 0, 0, 0 };

        XChangeProperty( _glfwLibrary.display, _glfwWin.window, hintAtom, hintAtom,
                         32, PropModeReplace, (unsigned char *)&MWMHints,
                         sizeof(MWMHints) / 4 );
        removedDecorations = GL_TRUE;
    }

    // Now try to set KWM hints
    hintAtom = XInternAtom( _glfwLibrary.display, "KWM_WIN_DECORATION", True );
    if ( hintAtom != None )
    {
        long KWMHints = KDE_tinyDecoration;

        XChangeProperty( _glfwLibrary.display, _glfwWin.window, hintAtom, hintAtom,
                         32, PropModeReplace, (unsigned char *)&KWMHints,
                         sizeof(KWMHints) / 4 );
        removedDecorations = GL_TRUE;
    }

    // Now try to set GNOME hints
    hintAtom = XInternAtom( _glfwLibrary.display, "_WIN_HINTS", True );
    if ( hintAtom != None )
    {
        long GNOMEHints = 0;

        XChangeProperty( _glfwLibrary.display, _glfwWin.window, hintAtom, hintAtom,
                         32, PropModeReplace, (unsigned char *)&GNOMEHints,
                         sizeof(GNOMEHints) / 4 );
        removedDecorations = GL_TRUE;
    }

    // Now try to set KDE NET_WM hints
    hintAtom = XInternAtom( _glfwLibrary.display, "_NET_WM_WINDOW_TYPE", True );
    if ( hintAtom != None )
    {
        Atom NET_WMHints[2];

        NET_WMHints[0] = XInternAtom( _glfwLibrary.display,
                                      "_KDE_NET_WM_WINDOW_TYPE_OVERRIDE",
                                      True );
        /* define a fallback... */
        NET_WMHints[1] = XInternAtom( _glfwLibrary.display,
                                      "_NET_WM_WINDOW_TYPE_NORMAL",
                                      True );

        XChangeProperty( _glfwLibrary.display, _glfwWin.window, hintAtom, XA_ATOM,
                         32, PropModeReplace, (unsigned char *)&NET_WMHints,
                         2 );
        removedDecorations = GL_TRUE;
    }

    // Set ICCCM fullscreen WM hint
    hintAtom = XInternAtom( _glfwLibrary.display, "_NET_WM_STATE", True );
    if ( hintAtom != None )
    {
        Atom NET_WMHints[1];

        NET_WMHints[0] = XInternAtom( _glfwLibrary.display,
                                      "_NET_WM_STATE_FULLSCREEN",
                                      True );

        XChangeProperty( _glfwLibrary.display, _glfwWin.window, hintAtom, XA_ATOM,
                         32, PropModeReplace, (unsigned char *)&NET_WMHints, 1 );
    }

    // Did we sucessfully remove the window decorations?
    if( removedDecorations )
    {
        // Finally set the transient hints
        XSetTransientForHint( _glfwLibrary.display,
                              _glfwWin.window,
                              RootWindow( _glfwLibrary.display, _glfwWin.screen) );
        XUnmapWindow( _glfwLibrary.display, _glfwWin.window );
        XMapWindow( _glfwLibrary.display, _glfwWin.window );
    }
    else
    {
        // The Butcher way of removing window decorations
        attributes.override_redirect = True;
        XChangeWindowAttributes( _glfwLibrary.display,
                                 _glfwWin.window,
                                 CWOverrideRedirect,
                                 &attributes );
        _glfwWin.OverrideRedirect = GL_TRUE;
    }
}


//========================================================================
// Turn on window decorations
//========================================================================

static void enableDecorations( void )
{
    GLboolean activatedDecorations;
    Atom hintAtom;

    // If this is an override redirect window, skip it...
    if( _glfwWin.OverrideRedirect )
    {
        return;
    }

    activatedDecorations = 0;

    // First try to unset MWM hints
    hintAtom = XInternAtom( _glfwLibrary.display, "_MOTIF_WM_HINTS", True );
    if ( hintAtom != None )
    {
        XDeleteProperty( _glfwLibrary.display, _glfwWin.window, hintAtom );
        activatedDecorations = GL_TRUE;
    }

    // Now try to unset KWM hints
    hintAtom = XInternAtom( _glfwLibrary.display, "KWM_WIN_DECORATION", True );
    if ( hintAtom != None )
    {
        XDeleteProperty( _glfwLibrary.display, _glfwWin.window, hintAtom );
        activatedDecorations = GL_TRUE;
    }

    // Now try to unset GNOME hints
    hintAtom = XInternAtom( _glfwLibrary.display, "_WIN_HINTS", True );
    if ( hintAtom != None )
    {
        XDeleteProperty( _glfwLibrary.display, _glfwWin.window, hintAtom );
        activatedDecorations = GL_TRUE;
    }

    // Now try to unset NET_WM hints
    hintAtom = XInternAtom( _glfwLibrary.display, "_NET_WM_WINDOW_TYPE", True );
    if ( hintAtom != None )
    {
        Atom NET_WMHints = XInternAtom( _glfwLibrary.display,
                                        "_NET_WM_WINDOW_TYPE_NORMAL",
                                        True);
        if( NET_WMHints != None )
        {
            XChangeProperty( _glfwLibrary.display, _glfwWin.window,
                            hintAtom, XA_ATOM, 32, PropModeReplace,
                            (unsigned char *)&NET_WMHints, 1 );
            activatedDecorations = GL_TRUE;
        }
    }

    // Finally unset the transient hints if necessary
    if( activatedDecorations )
    {
        // NOTE: Does this work?
        XSetTransientForHint( _glfwLibrary.display, _glfwWin.window, None );
        XUnmapWindow( _glfwLibrary.display, _glfwWin.window );
        XMapWindow( _glfwLibrary.display, _glfwWin.window );
    }
}


//========================================================================
// Translates an X Window key to internal coding
//========================================================================

static int TranslateKey( int keycode )
{
    KeySym key, key_lc, key_uc;

    // Try secondary keysym, for numeric keypad keys
    // Note: This way we always force "NumLock = ON", which at least
    // enables GLFW users to detect numeric keypad keys
    key = XKeycodeToKeysym( _glfwLibrary.display, keycode, 1 );
    switch( key )
    {
        // Numeric keypad
        case XK_KP_0:         return GLFW_KEY_KP_0;
        case XK_KP_1:         return GLFW_KEY_KP_1;
        case XK_KP_2:         return GLFW_KEY_KP_2;
        case XK_KP_3:         return GLFW_KEY_KP_3;
        case XK_KP_4:         return GLFW_KEY_KP_4;
        case XK_KP_5:         return GLFW_KEY_KP_5;
        case XK_KP_6:         return GLFW_KEY_KP_6;
        case XK_KP_7:         return GLFW_KEY_KP_7;
        case XK_KP_8:         return GLFW_KEY_KP_8;
        case XK_KP_9:         return GLFW_KEY_KP_9;
        case XK_KP_Separator:
        case XK_KP_Decimal:   return GLFW_KEY_KP_DECIMAL;
        case XK_KP_Equal:     return GLFW_KEY_KP_EQUAL;
        case XK_KP_Enter:     return GLFW_KEY_KP_ENTER;
        default:              break;
    }

    // Now try pimary keysym
    key = XKeycodeToKeysym( _glfwLibrary.display, keycode, 0 );
    switch( key )
    {
        // Special keys (non character keys)
        case XK_Escape:       return GLFW_KEY_ESC;
        case XK_Tab:          return GLFW_KEY_TAB;
        case XK_Shift_L:      return GLFW_KEY_LSHIFT;
        case XK_Shift_R:      return GLFW_KEY_RSHIFT;
        case XK_Control_L:    return GLFW_KEY_LCTRL;
        case XK_Control_R:    return GLFW_KEY_RCTRL;
        case XK_Meta_L:
        case XK_Alt_L:        return GLFW_KEY_LALT;
        case XK_Mode_switch:  // Mapped to Alt_R on many keyboards
        case XK_Meta_R:
        case XK_ISO_Level3_Shift: // AltGr on at least some machines
        case XK_Alt_R:        return GLFW_KEY_RALT;
        case XK_Super_L:      return GLFW_KEY_LSUPER;
        case XK_Super_R:      return GLFW_KEY_RSUPER;
        case XK_Menu:         return GLFW_KEY_MENU;
        case XK_Num_Lock:     return GLFW_KEY_KP_NUM_LOCK;
        case XK_Caps_Lock:    return GLFW_KEY_CAPS_LOCK;
        case XK_Scroll_Lock:  return GLFW_KEY_SCROLL_LOCK;
        case XK_Pause:        return GLFW_KEY_PAUSE;
        case XK_KP_Delete:
        case XK_Delete:       return GLFW_KEY_DEL;
        case XK_BackSpace:    return GLFW_KEY_BACKSPACE;
        case XK_Return:       return GLFW_KEY_ENTER;
        case XK_KP_Home:
        case XK_Home:         return GLFW_KEY_HOME;
        case XK_KP_End:
        case XK_End:          return GLFW_KEY_END;
        case XK_KP_Page_Up:
        case XK_Page_Up:      return GLFW_KEY_PAGEUP;
        case XK_KP_Page_Down:
        case XK_Page_Down:    return GLFW_KEY_PAGEDOWN;
        case XK_KP_Insert:
        case XK_Insert:       return GLFW_KEY_INSERT;
        case XK_KP_Left:
        case XK_Left:         return GLFW_KEY_LEFT;
        case XK_KP_Right:
        case XK_Right:        return GLFW_KEY_RIGHT;
        case XK_KP_Down:
        case XK_Down:         return GLFW_KEY_DOWN;
        case XK_KP_Up:
        case XK_Up:           return GLFW_KEY_UP;
        case XK_F1:           return GLFW_KEY_F1;
        case XK_F2:           return GLFW_KEY_F2;
        case XK_F3:           return GLFW_KEY_F3;
        case XK_F4:           return GLFW_KEY_F4;
        case XK_F5:           return GLFW_KEY_F5;
        case XK_F6:           return GLFW_KEY_F6;
        case XK_F7:           return GLFW_KEY_F7;
        case XK_F8:           return GLFW_KEY_F8;
        case XK_F9:           return GLFW_KEY_F9;
        case XK_F10:          return GLFW_KEY_F10;
        case XK_F11:          return GLFW_KEY_F11;
        case XK_F12:          return GLFW_KEY_F12;
        case XK_F13:          return GLFW_KEY_F13;
        case XK_F14:          return GLFW_KEY_F14;
        case XK_F15:          return GLFW_KEY_F15;
        case XK_F16:          return GLFW_KEY_F16;
        case XK_F17:          return GLFW_KEY_F17;
        case XK_F18:          return GLFW_KEY_F18;
        case XK_F19:          return GLFW_KEY_F19;
        case XK_F20:          return GLFW_KEY_F20;
        case XK_F21:          return GLFW_KEY_F21;
        case XK_F22:          return GLFW_KEY_F22;
        case XK_F23:          return GLFW_KEY_F23;
        case XK_F24:          return GLFW_KEY_F24;
        case XK_F25:          return GLFW_KEY_F25;

        // Numeric keypad (should have been detected in secondary keysym!)
        case XK_KP_Divide:    return GLFW_KEY_KP_DIVIDE;
        case XK_KP_Multiply:  return GLFW_KEY_KP_MULTIPLY;
        case XK_KP_Subtract:  return GLFW_KEY_KP_SUBTRACT;
        case XK_KP_Add:       return GLFW_KEY_KP_ADD;
        case XK_KP_Equal:     return GLFW_KEY_KP_EQUAL;
        case XK_KP_Enter:     return GLFW_KEY_KP_ENTER;

        // The rest (should be printable keys)
        default:
            // Make uppercase
            XConvertCase( key, &key_lc, &key_uc );
            key = key_uc;

            // Valid ISO 8859-1 character?
            if( (key >=  32 && key <= 126) ||
                (key >= 160 && key <= 255) )
            {
                return (int) key;
            }
            return GLFW_KEY_UNKNOWN;
    }
}


//========================================================================
// Translates an X Window event to Unicode
//========================================================================

static int TranslateChar( XKeyEvent *event )
{
    KeySym keysym;

    // Get X11 keysym
    XLookupString( event, NULL, 0, &keysym, NULL );

    // Convert to Unicode (see x11_keysym2unicode.c)
    return (int) _glfwKeySym2Unicode( keysym );
}



//========================================================================
// Get next X event (called by glfwPollEvents)
//========================================================================

static int GetNextEvent( void )
{
    XEvent event, next_event;

    // Pull next event from event queue
    XNextEvent( _glfwLibrary.display, &event );

    // Handle certain window messages
    switch( event.type )
    {
        // Is a key being pressed?
        case KeyPress:
        {
            // Translate and report key press
            _glfwInputKey( TranslateKey( event.xkey.keycode ), GLFW_PRESS );

            // Translate and report character input
            if( _glfwWin.CharCallback )
            {
                _glfwInputChar( TranslateChar( &event.xkey ), GLFW_PRESS );
            }
            break;
        }

        // Is a key being released?
        case KeyRelease:
        {
            // Do not report key releases for key repeats. For key repeats
            // we will get KeyRelease/KeyPress pairs with identical time
            // stamps. User selected key repeat filtering is handled in
            // _glfwInputKey()/_glfwInputChar().
            if( XEventsQueued( _glfwLibrary.display, QueuedAfterReading ) )
            {
                XPeekEvent( _glfwLibrary.display, &next_event );
                if( next_event.type == KeyPress &&
                    next_event.xkey.window == event.xkey.window &&
                    next_event.xkey.keycode == event.xkey.keycode )
                {
                    // This last check is a hack to work around key repeats
                    // leaking through due to some sort of time drift
                    // Toshiyuki Takahashi can press a button 16 times per
                    // second so it's fairly safe to assume that no human is
                    // pressing the key 50 times per second (value is ms)
                    if( ( next_event.xkey.time - event.xkey.time ) < 20 )
                    {
                        // Do not report anything for this event
                        break;
                    }
                }
            }

            // Translate and report key release
            _glfwInputKey( TranslateKey( event.xkey.keycode ), GLFW_RELEASE );

            // Translate and report character input
            if( _glfwWin.CharCallback )
            {
                _glfwInputChar( TranslateChar( &event.xkey ), GLFW_RELEASE );
            }
            break;
        }

        // Were any of the mouse-buttons pressed?
        case ButtonPress:
        {
            if( event.xbutton.button == Button1 )
            {
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS );
            }
            else if( event.xbutton.button == Button2 )
            {
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS );
            }
            else if( event.xbutton.button == Button3 )
            {
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS );
            }

            // XFree86 3.3.2 and later translates mouse wheel up/down into
            // mouse button 4 & 5 presses
            else if( event.xbutton.button == Button4 )
            {
                _glfwInput.WheelPos++;  // To verify: is this up or down?
                if( _glfwWin.MouseWheelCallback )
                {
                    _glfwWin.MouseWheelCallback( _glfwInput.WheelPos );
                }
            }
            else if( event.xbutton.button == Button5 )
            {
                _glfwInput.WheelPos--;
                if( _glfwWin.MouseWheelCallback )
                {
                    _glfwWin.MouseWheelCallback( _glfwInput.WheelPos );
                }
            }
            break;
        }

        // Were any of the mouse-buttons released?
        case ButtonRelease:
        {
            if( event.xbutton.button == Button1 )
            {
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_LEFT,
                                      GLFW_RELEASE );
            }
            else if( event.xbutton.button == Button2 )
            {
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_MIDDLE,
                                      GLFW_RELEASE );
            }
            else if( event.xbutton.button == Button3 )
            {
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_RIGHT,
                                      GLFW_RELEASE );
            }
            break;
        }

        // Was the mouse moved?
        case MotionNotify:
        {
            if( event.xmotion.x != _glfwInput.CursorPosX ||
                event.xmotion.y != _glfwInput.CursorPosY )
            {
                if( _glfwWin.MouseLock )
                {
                    _glfwInput.MousePosX += event.xmotion.x -
                                            _glfwInput.CursorPosX;
                    _glfwInput.MousePosY += event.xmotion.y -
                                            _glfwInput.CursorPosY;
                }
                else
                {
                    _glfwInput.MousePosX = event.xmotion.x;
                    _glfwInput.MousePosY = event.xmotion.y;
                }
                _glfwInput.CursorPosX = event.xmotion.x;
                _glfwInput.CursorPosY = event.xmotion.y;
                _glfwInput.MouseMoved = GL_TRUE;

                // Call user callback function
                if( _glfwWin.MousePosCallback )
                {
                    _glfwWin.MousePosCallback( _glfwInput.MousePosX,
                                               _glfwInput.MousePosY );
                }
            }
            break;
        }

        // Was the window resized?
        case ConfigureNotify:
        {
            if( event.xconfigure.width != _glfwWin.Width ||
                event.xconfigure.height != _glfwWin.Height )
            {
                _glfwWin.Width = event.xconfigure.width;
                _glfwWin.Height = event.xconfigure.height;
                if( _glfwWin.WindowSizeCallback )
                {
                    _glfwWin.WindowSizeCallback( _glfwWin.Width,
                                                 _glfwWin.Height );
                }
            }
            break;
        }

        // Was the window closed by the window manager?
        case ClientMessage:
        {
            if( (Atom) event.xclient.data.l[ 0 ] == _glfwWin.WMDeleteWindow )
            {
                return GL_TRUE;
            }

            if( (Atom) event.xclient.data.l[ 0 ] == _glfwWin.WMPing )
            {
                XSendEvent( _glfwLibrary.display,
                        RootWindow( _glfwLibrary.display, _glfwWin.screen ),
                        False, SubstructureNotifyMask | SubstructureRedirectMask, &event );
            }
            break;
        }

        // Was the window mapped (un-iconified)?
        case MapNotify:
            _glfwWin.MapNotifyCount++;
            break;

        // Was the window unmapped (iconified)?
        case UnmapNotify:
            _glfwWin.MapNotifyCount--;
            break;

        // Was the window activated?
        case FocusIn:
            _glfwWin.FocusInCount++;
            break;

        // Was the window de-activated?
        case FocusOut:
            _glfwWin.FocusInCount--;
            break;

        // Was the window contents damaged?
        case Expose:
        {
            // Call user callback function
            if( _glfwWin.WindowRefreshCallback )
            {
                _glfwWin.WindowRefreshCallback();
            }
            break;
        }

        // Was the window destroyed?
        case DestroyNotify:
            return GL_FALSE;

        default:
        {
#if defined( _GLFW_HAS_XRANDR )
            switch( event.type - _glfwLibrary.XRandR.EventBase )
            {
                case RRScreenChangeNotify:
                {
                    // Show XRandR that we really care
                    XRRUpdateConfiguration( &event );
                    break;
                }
            }
#endif
                break;
        }
    }

    // The window was not destroyed
    return GL_FALSE;
}


//========================================================================
// Create a blank cursor (for locked mouse mode)
//========================================================================

static Cursor CreateNULLCursor( Display *display, Window root )
{
    Pixmap    cursormask;
    XGCValues xgc;
    GC        gc;
    XColor    col;
    Cursor    cursor;

    cursormask = XCreatePixmap( display, root, 1, 1, 1 );
    xgc.function = GXclear;
    gc = XCreateGC( display, cursormask, GCFunction, &xgc );
    XFillRectangle( display, cursormask, gc, 0, 0, 1, 1 );
    col.pixel = 0;
    col.red = 0;
    col.flags = 4;
    cursor = XCreatePixmapCursor( display, cursormask, cursormask,
                                  &col,&col, 0,0 );
    XFreePixmap( display, cursormask );
    XFreeGC( display, gc );

    return cursor;
}


//========================================================================
// Return a list of available and usable framebuffer configs
//========================================================================

static _GLFWfbconfig *GetFBConfigs( unsigned int *found )
{
    GLXFBConfig *fbconfigs;
    _GLFWfbconfig *result;
    XID platformID;
    int i, value, count = 0;

    *found = 0;

    if( _glfwLibrary.glxMajor == 1 && _glfwLibrary.glxMinor < 3 )
    {
        if( !_glfwPlatformExtensionSupported( "GLX_SGIX_fbconfig" ) )
        {
            fprintf(stderr, "GLXFBConfigs are not supported by the X server\n");
            return NULL;
        }
    }

    fbconfigs = glXGetFBConfigs( _glfwLibrary.display, _glfwWin.screen, &count );
    if( !count )
    {
        fprintf(stderr, "No GLXFBConfigs returned");
        return NULL;
    }

    result = (_GLFWfbconfig*) malloc(sizeof(_GLFWfbconfig) * count);

    for( i = 0;  i < count;  i++ )
    {
        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_DOUBLEBUFFER,
                              &value );
        if( !value )
        {
            // Only consider doublebuffered GLXFBConfigs
            continue;
        }

        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_RENDER_TYPE,
                              &value );
        if( !( value & GLX_RGBA_BIT )  )
        {
            // Only consider RGBA GLXFBConfigs
            continue;
        }

        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_DRAWABLE_TYPE,
                              &value );
        if( !( value & GLX_WINDOW_BIT )  )
        {
            // Only consider window GLXFBConfigs
            continue;
        }

        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_VISUAL_ID,
                              &value );
        if( !value )
        {
            // Only consider GLXFBConfigs with associated visuals
            continue;
        }

        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_RED_SIZE,
                              &result[*found].redBits );
        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_GREEN_SIZE,
                              &result[*found].greenBits );
        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_BLUE_SIZE,
                              &result[*found].blueBits );

        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_ALPHA_SIZE,
                              &result[*found].alphaBits );
        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_DEPTH_SIZE,
                              &result[*found].depthBits );
        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_STENCIL_SIZE,
                              &result[*found].stencilBits );

        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_ACCUM_RED_SIZE,
                              &result[*found].accumRedBits );
        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_ACCUM_GREEN_SIZE,
                              &result[*found].accumGreenBits );
        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_ACCUM_BLUE_SIZE,
                              &result[*found].accumBlueBits );
        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_ACCUM_ALPHA_SIZE,
                              &result[*found].accumAlphaBits );

        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_AUX_BUFFERS,
                              &result[*found].auxBuffers );
        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_STEREO,
                              &result[*found].stereo );
        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_SAMPLES,
                              &result[*found].samples );

        glXGetFBConfigAttrib( _glfwLibrary.display, fbconfigs[i], GLX_FBCONFIG_ID,
                              (int*) &platformID );
        result[*found].platformID = (GLFWintptr) platformID;

        (*found)++;
    }

    return result;
}


//========================================================================
// Create the OpenGL context
//========================================================================

#define SetGLXattrib( attribs, index, attribName, attribValue ) \
    attribs[index++] = attribName; \
    attribs[index++] = attribValue;

static int CreateContext( const _GLFWwndconfig *wndconfig, GLXFBConfigID fbconfigID )
{
    int attribs[40];
    int flags, dummy, index;
    const GLubyte *extensions;
    GLXFBConfig *fbconfig;
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = NULL;

    if( wndconfig->glMajor > 2 )
    {
        extensions = (const GLubyte*) glXQueryExtensionsString( _glfwLibrary.display,
                                                                _glfwWin.screen );

        if( !_glfwStringInExtensionString( "GLX_ARB_create_context", extensions ) )
        {
            fprintf(stderr, "GLX_ARB_create_context extension not found\n");
            return GL_FALSE;
        }

        glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress(
                                         (const GLubyte*) "glXCreateContextAttribsARB" );
        if( glXCreateContextAttribsARB == NULL )
        {
            fprintf(stderr, "glXCreateContextAttribsARB entry point not found\n");
            return GL_FALSE;
        }
    }

    // Retrieve the previously selected GLXFBConfig
    {
        index = 0;

        SetGLXattrib( attribs, index, GLX_FBCONFIG_ID, (int) fbconfigID );
        SetGLXattrib( attribs, index, None, None );

        fbconfig = glXChooseFBConfig( _glfwLibrary.display, _glfwWin.screen, attribs, &dummy );
        if( fbconfig == NULL )
        {
            fprintf(stderr, "Unable to retrieve the selected GLXFBConfig\n");
            return GL_FALSE;
        }
    }

    // Retrieve the corresponding visual
    _glfwWin.visual = glXGetVisualFromFBConfig( _glfwLibrary.display, *fbconfig );
    if( _glfwWin.visual == NULL )
    {
        XFree( fbconfig );

        fprintf(stderr, "Unable to retrieve visual for GLXFBconfig\n");
        return GL_FALSE;
    }

    if( glXCreateContextAttribsARB )
    {
        index = 0;

        SetGLXattrib( attribs, index, GLX_CONTEXT_MAJOR_VERSION_ARB, wndconfig->glMajor );
        SetGLXattrib( attribs, index, GLX_CONTEXT_MINOR_VERSION_ARB, wndconfig->glMinor );

        if( wndconfig->glForward || wndconfig->glDebug )
        {
            flags = 0;

            if( wndconfig->glForward )
            {
                flags |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
            }

            if( wndconfig->glDebug )
            {
                flags |= GLX_CONTEXT_DEBUG_BIT_ARB;
            }

            SetGLXattrib( attribs, index, GLX_CONTEXT_FLAGS_ARB, flags );
        }

        SetGLXattrib( attribs, index, None, None );

        _glfwWin.context = glXCreateContextAttribsARB( _glfwLibrary.display,
                                                       *fbconfig,
                                                       NULL,
                                                       True,
                                                       attribs );
    }
    else
    {
        _glfwWin.context = glXCreateNewContext( _glfwLibrary.display,
                                                *fbconfig,
                                                GLX_RGBA_TYPE,
                                                NULL,
                                                True );
    }

    XFree( fbconfig );

    if( _glfwWin.context == NULL )
    {
        fprintf(stderr, "Unable to create OpenGL context\n");
        return GL_FALSE;
    }

    _glfwWin.fbconfigID = fbconfigID;

    return GL_TRUE;
}


//========================================================================
// Initialize GLX-specific extensions
//========================================================================

static void InitGLXExtensions( void )
{
    int has_swap_control;

    // Initialize OpenGL extension: GLX_SGI_swap_control
    has_swap_control = _glfwPlatformExtensionSupported( "GLX_SGI_swap_control" );

    if( has_swap_control )
    {
        _glfwWin.SwapInterval = (PFNGLXSWAPINTERVALSGIPROC)
        _glfw_glXGetProcAddress( (GLubyte*) "glXSwapIntervalSGI" );
    }
    else
    {
        _glfwWin.SwapInterval = NULL;
    }
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// Here is where the window is created, and
// the OpenGL rendering context is created
//========================================================================

int _glfwPlatformOpenWindow( int width, int height,
                             const _GLFWwndconfig* wndconfig,
                             const _GLFWfbconfig* fbconfig )
{
    XSetWindowAttributes wa;
    XEvent      event;
    Atom protocols[2];
    XSizeHints *sizehints;
    unsigned long wamask;
    unsigned int fbcount;
    _GLFWfbconfig *fbconfigs;
    const _GLFWfbconfig *closest;

    // Clear platform specific GLFW window state
    _glfwWin.visual           = (XVisualInfo*)NULL;
    _glfwWin.colormap         = (Colormap)0;
    _glfwWin.context          = (GLXContext)NULL;
    _glfwWin.window           = (Window)0;
    _glfwWin.PointerGrabbed   = GL_FALSE;
    _glfwWin.KeyboardGrabbed  = GL_FALSE;
    _glfwWin.OverrideRedirect = GL_FALSE;
    _glfwWin.FS.ModeChanged   = GL_FALSE;
    _glfwWin.Saver.Changed    = GL_FALSE;
    _glfwWin.RefreshRate      = wndconfig->refreshRate;
    _glfwWin.WindowNoResize   = wndconfig->windowNoResize;

    // As the 2.x API doesn't understand screens, we hardcode this choice and
    // hope for the best
    _glfwWin.screen = DefaultScreen( _glfwLibrary.display );

    // Choose the best available fbconfig
    {
        fbconfigs = GetFBConfigs( &fbcount );
        if( !fbconfigs )
        {
            _glfwPlatformCloseWindow();
            return GL_FALSE;
        }

        closest = _glfwChooseFBConfig( fbconfig, fbconfigs, fbcount );
        if( !closest )
        {
            _glfwPlatformCloseWindow();
            return GL_FALSE;
        }
    }

    if( !CreateContext( wndconfig, (GLXFBConfigID) closest->platformID ) )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    if( wndconfig->mode == GLFW_FULLSCREEN )
    {
        // Change video mode
        _glfwSetVideoMode( _glfwWin.screen, &_glfwWin.Width,
                           &_glfwWin.Height, &_glfwWin.RefreshRate );

        // Remember old screen saver settings
        XGetScreenSaver( _glfwLibrary.display, &_glfwWin.Saver.Timeout,
                         &_glfwWin.Saver.Interval, &_glfwWin.Saver.Blanking,
                         &_glfwWin.Saver.Exposure );

        // Disable screen saver
        XSetScreenSaver( _glfwLibrary.display, 0, 0, DontPreferBlanking,
                         DefaultExposures );
    }

    _glfwWin.colormap = XCreateColormap( _glfwLibrary.display,
                                         RootWindow( _glfwLibrary.display,
                                                     _glfwWin.screen ),
                                         _glfwWin.visual->visual,
                                         AllocNone );

    // Create the window
    {
        wamask = CWBorderPixel | CWColormap | CWEventMask;

        wa.colormap = _glfwWin.colormap;
        wa.border_pixel = 0;
        wa.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
            PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
            ExposureMask | FocusChangeMask | VisibilityChangeMask;

        if( wndconfig->mode == GLFW_WINDOW )
        {
            // The /only/ reason we are setting the background pixel here is
            // because otherwise our window wont get any decorations on systems
            // using Compiz on Intel hardware
            wa.background_pixel = BlackPixel( _glfwLibrary.display, _glfwWin.screen );
            wamask |= CWBackPixel;
        }

        _glfwWin.window = XCreateWindow(
            _glfwLibrary.display,
            RootWindow( _glfwLibrary.display, _glfwWin.screen ),
            0, 0,                            // Upper left corner of this window on root
            _glfwWin.Width, _glfwWin.Height,
            0,                               // Border width
            _glfwWin.visual->depth,          // Color depth
            InputOutput,
            _glfwWin.visual->visual,
            wamask,
            &wa
        );
        if( !_glfwWin.window )
        {
            _glfwPlatformCloseWindow();
            return GL_FALSE;
        }
    }

    // Declare which WM protocols we support
    {
        // Basic window close notification protocol
        _glfwWin.WMDeleteWindow = XInternAtom( _glfwLibrary.display,
                                               "WM_DELETE_WINDOW",
                                               False );

        // Tells the WM to ping our window and flag us as unresponsive if we
        // don't reply within a few seconds
        _glfwWin.WMPing = XInternAtom( _glfwLibrary.display, "_NET_WM_PING", False );

        protocols[0] = _glfwWin.WMDeleteWindow;
        protocols[1] = _glfwWin.WMPing;

        XSetWMProtocols( _glfwLibrary.display, _glfwWin.window, protocols,
                         sizeof(protocols) / sizeof(Atom) );
    }

    if( wndconfig->mode == GLFW_FULLSCREEN )
    {
        disableDecorations();
    }

    // Set window size hints
    {
        sizehints = XAllocSizeHints();
        sizehints->flags = 0;

        if( wndconfig->windowNoResize )
        {
            sizehints->flags |= (PMinSize | PMaxSize);
            sizehints->min_width  = sizehints->max_width  = _glfwWin.Width;
            sizehints->min_height = sizehints->max_height = _glfwWin.Height;
        }

        if( wndconfig->mode == GLFW_FULLSCREEN )
        {
            sizehints->flags |= PPosition;
            sizehints->x = 0;
            sizehints->y = 0;
        }

        XSetWMNormalHints( _glfwLibrary.display, _glfwWin.window, sizehints );
        XFree( sizehints );
    }

    XMapWindow( _glfwLibrary.display, _glfwWin.window );

    // Wait for map notification
    XIfEvent( _glfwLibrary.display, &event, IsMapNotify,
              (char*)_glfwWin.window );

    // Make sure that our window ends up on top of things
    XRaiseWindow( _glfwLibrary.display, _glfwWin.window );

    if( wndconfig->mode == GLFW_FULLSCREEN )
    {
        // Fullscreen mode post-window-creation work

#if defined( _GLFW_HAS_XRANDR )
        // Request screen change notifications
        if( _glfwLibrary.XRandR.Available )
        {
            XRRSelectInput( _glfwLibrary.display,
                            _glfwWin.window,
                            RRScreenChangeNotifyMask );
        }
#endif

        // Force window position/size (some WMs do their own window
        // geometry, which we want to override)
        XMoveWindow( _glfwLibrary.display, _glfwWin.window, 0, 0 );
        XResizeWindow( _glfwLibrary.display, _glfwWin.window, _glfwWin.Width,
                       _glfwWin.Height );

        if( XGrabKeyboard( _glfwLibrary.display, _glfwWin.window, True,
                           GrabModeAsync, GrabModeAsync, CurrentTime ) ==
            GrabSuccess )
        {
            _glfwWin.KeyboardGrabbed = GL_TRUE;
        }

        if( XGrabPointer( _glfwLibrary.display, _glfwWin.window, True,
                          ButtonPressMask | ButtonReleaseMask |
                          PointerMotionMask, GrabModeAsync, GrabModeAsync,
                          _glfwWin.window, None, CurrentTime ) ==
            GrabSuccess )
        {
            _glfwWin.PointerGrabbed = GL_TRUE;
        }

        // Try to get window inside viewport (for virtual displays) by
        // moving the mouse cursor to the upper left corner (and then to
        // the center) - this works for XFree86
        XWarpPointer( _glfwLibrary.display, None, _glfwWin.window, 0,0,0,0, 0,0 );
        XWarpPointer( _glfwLibrary.display, None, _glfwWin.window, 0,0,0,0,
                      _glfwWin.Width/2, _glfwWin.Height/2 );
    }

    _glfwPlatformSetWindowTitle( "GLFW Window" );

    // Connect the context to the window
    glXMakeCurrent( _glfwLibrary.display, _glfwWin.window, _glfwWin.context );

    // Start by clearing the front buffer to black (avoid ugly desktop
    // remains in our OpenGL window)
    glClear( GL_COLOR_BUFFER_BIT );
    glXSwapBuffers( _glfwLibrary.display, _glfwWin.window );

    InitGLXExtensions();

    return GL_TRUE;
}


//========================================================================
// Properly kill the window/video display
//========================================================================

void _glfwPlatformCloseWindow( void )
{
#if defined( _GLFW_HAS_XRANDR )
    XRRScreenConfiguration *sc;
    Window root;
#endif

    // Do we have a rendering context?
    if( _glfwWin.context )
    {
        // Release the context
        glXMakeCurrent( _glfwLibrary.display, None, NULL );

        // Delete the context
        glXDestroyContext( _glfwLibrary.display, _glfwWin.context );
        _glfwWin.context = NULL;
    }

    // Do we have a visual?
    if( _glfwWin.visual )
    {
        XFree( _glfwWin.visual );
        _glfwWin.visual = NULL;
    }

    // Ungrab pointer and/or keyboard?
    if( _glfwWin.KeyboardGrabbed )
    {
        XUngrabKeyboard( _glfwLibrary.display, CurrentTime );
        _glfwWin.KeyboardGrabbed = GL_FALSE;
    }
    if( _glfwWin.PointerGrabbed )
    {
        XUngrabPointer( _glfwLibrary.display, CurrentTime );
        _glfwWin.PointerGrabbed = GL_FALSE;
    }

    // Do we have a window?
    if( _glfwWin.window )
    {
        // Unmap the window
        XUnmapWindow( _glfwLibrary.display, _glfwWin.window );

        // Destroy the window
        XDestroyWindow( _glfwLibrary.display, _glfwWin.window );
        _glfwWin.window = (Window) 0;
    }

    if( _glfwWin.colormap )
    {
        XFreeColormap( _glfwLibrary.display, _glfwWin.colormap );
        _glfwWin.colormap = (Colormap) 0;
    }

    // Did we change the fullscreen resolution?
    if( _glfwWin.FS.ModeChanged )
    {
#if defined( _GLFW_HAS_XRANDR )
        if( _glfwLibrary.XRandR.Available )
        {
            root = RootWindow( _glfwLibrary.display, _glfwWin.screen );
            sc = XRRGetScreenInfo( _glfwLibrary.display, root );

            XRRSetScreenConfig( _glfwLibrary.display,
                                sc,
                                root,
                                _glfwWin.FS.OldSizeID,
                                _glfwWin.FS.OldRotation,
                                CurrentTime );

            XRRFreeScreenConfigInfo( sc );
        }
#elif defined( _GLFW_HAS_XF86VIDMODE )
        if( _glfwLibrary.XF86VidMode.Available )
        {
            // Unlock mode switch
            XF86VidModeLockModeSwitch( _glfwLibrary.display,
                                       _glfwWin.screen,
                                       0 );

            // Change the video mode back to the old mode
            XF86VidModeSwitchToMode( _glfwLibrary.display,
                                     _glfwWin.screen,
                                     &_glfwWin.FS.OldMode );
        }
#endif
        _glfwWin.FS.ModeChanged = GL_FALSE;
    }

    // Did we change the screen saver setting?
    if( _glfwWin.Saver.Changed )
    {
        // Restore old screen saver settings
        XSetScreenSaver( _glfwLibrary.display,
                         _glfwWin.Saver.Timeout,
                         _glfwWin.Saver.Interval,
                         _glfwWin.Saver.Blanking,
                         _glfwWin.Saver.Exposure );
        _glfwWin.Saver.Changed = GL_FALSE;
    }

    //XSync( _glfwLibrary.display, True );
}


//========================================================================
// _glfwPlatformSetWindowTitle() - Set the window title.
//========================================================================

void _glfwPlatformSetWindowTitle( const char *title )
{
    // Set window & icon title
    XStoreName( _glfwLibrary.display, _glfwWin.window, title );
    XSetIconName( _glfwLibrary.display, _glfwWin.window, title );
}


//========================================================================
// _glfwPlatformSetWindowSize() - Set the window size.
//========================================================================

void _glfwPlatformSetWindowSize( int width, int height )
{
    int     mode = 0, rate, sizechanged = GL_FALSE;
    GLint   drawbuffer;
    GLfloat clearcolor[4];
    XSizeHints *sizehints;

    rate = _glfwWin.RefreshRate;

    // If we are in fullscreen mode, get some info about the current mode
    if( _glfwWin.Fullscreen )
    {
        // Get closest match for target video mode
        mode = _glfwGetClosestVideoMode( _glfwWin.screen, &width, &height, &rate );
    }

    if( _glfwWin.WindowNoResize )
    {
        sizehints = XAllocSizeHints();
        sizehints->flags = 0;

        sizehints->min_width  = sizehints->max_width  = width;
        sizehints->min_height = sizehints->max_height = height;

        XSetWMNormalHints( _glfwLibrary.display, _glfwWin.window, sizehints );
        XFree( sizehints );
    }

    // Change window size before changing fullscreen mode?
    if( _glfwWin.Fullscreen && (width > _glfwWin.Width) )
    {
        XResizeWindow( _glfwLibrary.display, _glfwWin.window, width, height );
        sizechanged = GL_TRUE;
    }

    // Change fullscreen video mode?
    if( _glfwWin.Fullscreen )
    {
        // Change video mode (keeping current rate)
        _glfwSetVideoModeMODE( _glfwWin.screen, mode, _glfwWin.RefreshRate );

        // Clear the front buffer to black (avoid ugly desktop remains in
        // our OpenGL window)
        glGetIntegerv( GL_DRAW_BUFFER, &drawbuffer );
        glGetFloatv( GL_COLOR_CLEAR_VALUE, clearcolor );
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT );
        if( drawbuffer == GL_BACK )
        {
            glXSwapBuffers( _glfwLibrary.display, _glfwWin.window );
        }
        glClearColor( clearcolor[0], clearcolor[1], clearcolor[2],
                      clearcolor[3] );
    }

    // Set window size (if not already changed)
    if( !sizechanged )
    {
        XResizeWindow( _glfwLibrary.display, _glfwWin.window, width, height );
    }
}


//========================================================================
// _glfwPlatformSetWindowPos() - Set the window position.
//========================================================================

void _glfwPlatformSetWindowPos( int x, int y )
{
    // Set window position
    XMoveWindow( _glfwLibrary.display, _glfwWin.window, x, y );
}


//========================================================================
// _glfwPlatformIconfyWindow() - Window iconification
//========================================================================

void _glfwPlatformIconifyWindow( void )
{
    // We can't do this for override redirect windows
    if( _glfwWin.OverrideRedirect )
    {
        return;
    }

    // In fullscreen mode, we need to restore the desktop video mode
    if( _glfwWin.Fullscreen )
    {
#if defined( _GLFW_HAS_XRANDR )
    // TODO: The code.
#elif defined( _GLFW_HAS_XF86VIDMODE )
        if( _glfwLibrary.XF86VidMode.Available )
        {
            // Unlock mode switch
            XF86VidModeLockModeSwitch( _glfwLibrary.display,
                                       _glfwWin.screen,
                                       0 );

            // Change the video mode back to the old mode
            XF86VidModeSwitchToMode( _glfwLibrary.display,
                _glfwWin.screen, &_glfwWin.FS.OldMode );
        }
#endif
        _glfwWin.FS.ModeChanged = GL_FALSE;
    }

    // Show mouse pointer
    if( _glfwWin.PointerHidden )
    {
        XUndefineCursor( _glfwLibrary.display, _glfwWin.window );
        _glfwWin.PointerHidden = GL_FALSE;
    }

    // Un-grab mouse pointer
    if( _glfwWin.PointerGrabbed )
    {
        XUngrabPointer( _glfwLibrary.display, CurrentTime );
        _glfwWin.PointerGrabbed = GL_FALSE;
    }

    // Iconify window
    XIconifyWindow( _glfwLibrary.display, _glfwWin.window,
                    _glfwWin.screen );

    // Window is now iconified
    _glfwWin.Iconified = GL_TRUE;
}


//========================================================================
// Window un-iconification
//========================================================================

void _glfwPlatformRestoreWindow( void )
{
    // We can't do this for override redirect windows
    if( _glfwWin.OverrideRedirect )
    {
        return;
    }

    // In fullscreen mode, change back video mode to user selected mode
    if( _glfwWin.Fullscreen )
    {
        _glfwSetVideoMode( _glfwWin.screen,
                       &_glfwWin.Width, &_glfwWin.Height, &_glfwWin.RefreshRate );
    }

    // Un-iconify window
    XMapWindow( _glfwLibrary.display, _glfwWin.window );

    // In fullscreen mode...
    if( _glfwWin.Fullscreen )
    {
        // Make sure window is in upper left corner
        XMoveWindow( _glfwLibrary.display, _glfwWin.window, 0, 0 );

        // Get input focus
        XSetInputFocus( _glfwLibrary.display, _glfwWin.window, RevertToParent,
                        CurrentTime );
    }

    // Lock mouse, if necessary
    if( _glfwWin.MouseLock )
    {
        // Hide cursor
        if( !_glfwWin.PointerHidden )
        {
            XDefineCursor( _glfwLibrary.display, _glfwWin.window,
                           CreateNULLCursor( _glfwLibrary.display,
                                             _glfwWin.window ) );

            _glfwWin.PointerHidden = GL_TRUE;
        }

        // Grab cursor
        if( !_glfwWin.PointerGrabbed )
        {
            if( XGrabPointer( _glfwLibrary.display, _glfwWin.window, True,
                              ButtonPressMask | ButtonReleaseMask |
                              PointerMotionMask, GrabModeAsync,
                              GrabModeAsync, _glfwWin.window, None,
                              CurrentTime ) == GrabSuccess )
            {
                _glfwWin.PointerGrabbed = GL_TRUE;
            }
        }
    }

    // Window is no longer iconified
    _glfwWin.Iconified = GL_FALSE;
}


//========================================================================
// _glfwPlatformSwapBuffers() - Swap buffers (double-buffering) and poll
// any new events.
//========================================================================

void _glfwPlatformSwapBuffers( void )
{
    // Update display-buffer
    glXSwapBuffers( _glfwLibrary.display, _glfwWin.window );
}


//========================================================================
// _glfwPlatformSwapInterval() - Set double buffering swap interval
//========================================================================

void _glfwPlatformSwapInterval( int interval )
{
    if( _glfwWin.SwapInterval )
    {
        _glfwWin.SwapInterval( interval );
    }
}


//========================================================================
// Read back framebuffer parameters from the context
//========================================================================

void _glfwPlatformRefreshWindowParams( void )
{
    int dummy, sample_buffers;
    GLXFBConfig *fbconfig;
#if defined( _GLFW_HAS_XRANDR )
    XRRScreenConfiguration *sc;
#elif defined( _GLFW_HAS_XF86VIDMODE )
    XF86VidModeModeLine modeline;
    int dotclock;
    float pixels_per_second, pixels_per_frame;
#endif
    int attribs[] = { GLX_FBCONFIG_ID, _glfwWin.fbconfigID, None };

    fbconfig = glXChooseFBConfig( _glfwLibrary.display,
                                  _glfwWin.screen,
                                  attribs,
                                  &dummy );
    if( fbconfig == NULL )
    {
        return;
    }

    // There is no clear definition of an "accelerated" context on X11/GLX, and
    // true sounds better than false, so we hardcode true here
    _glfwWin.Accelerated = GL_TRUE;

    // "Standard" window parameters
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_RED_SIZE,
                          &_glfwWin.RedBits );
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_GREEN_SIZE,
                          &_glfwWin.GreenBits );
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_BLUE_SIZE,
                          &_glfwWin.BlueBits );

    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_ALPHA_SIZE,
                          &_glfwWin.AlphaBits );
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_DEPTH_SIZE,
                          &_glfwWin.DepthBits );
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_STENCIL_SIZE,
                          &_glfwWin.StencilBits );

    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_ACCUM_RED_SIZE,
                          &_glfwWin.AccumRedBits );
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_ACCUM_GREEN_SIZE,
                          &_glfwWin.AccumGreenBits );
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_ACCUM_BLUE_SIZE,
                          &_glfwWin.AccumBlueBits );
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_ACCUM_ALPHA_SIZE,
                          &_glfwWin.AccumAlphaBits );
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_AUX_BUFFERS,
                          &_glfwWin.AuxBuffers );

    // Get stereo rendering setting
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_STEREO,
                          &_glfwWin.Stereo );
    _glfwWin.Stereo = _glfwWin.Stereo ? 1 : 0;

    // Get multisample buffer samples
    glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_SAMPLE_BUFFERS,
                          &sample_buffers );
    if( sample_buffers == 0 )
    {
        _glfwWin.Samples = 0;
    }
    else
    {
        glXGetFBConfigAttrib( _glfwLibrary.display, *fbconfig, GLX_SAMPLES,
                              &_glfwWin.Samples );
    }

    // Default to refresh rate unknown (=0 according to GLFW spec)
    _glfwWin.RefreshRate = 0;

    // Retrieve refresh rate if possible
#if defined( _GLFW_HAS_XRANDR )
    if( _glfwLibrary.XRandR.Available )
    {
        sc = XRRGetScreenInfo( _glfwLibrary.display,
                               RootWindow( _glfwLibrary.display, _glfwWin.screen ) );
        _glfwWin.RefreshRate = XRRConfigCurrentRate( sc );
        XRRFreeScreenConfigInfo( sc );
    }
#elif defined( _GLFW_HAS_XF86VIDMODE )
    if( _glfwLibrary.XF86VidMode.Available )
    {
        // Use the XF86VidMode extension to get current video mode
        XF86VidModeGetModeLine( _glfwLibrary.display, _glfwWin.screen,
                                &dotclock, &modeline );
        pixels_per_second = 1000.0f * (float) dotclock;
        pixels_per_frame  = (float) modeline.htotal * modeline.vtotal;
        _glfwWin.RefreshRate = (int)(pixels_per_second/pixels_per_frame+0.5);
    }
#endif
}


//========================================================================
// _glfwPlatformPollEvents() - Poll for new window and input events
//========================================================================

void _glfwPlatformPollEvents( void )
{
    int winclosed = GL_FALSE;

    // Flag that the cursor has not moved
    _glfwInput.MouseMoved = GL_FALSE;

    // Clear MapNotify and FocusIn counts
    _glfwWin.MapNotifyCount = 0;
    _glfwWin.FocusInCount = 0;

    // Use XSync to synchronise events to the X display.
    // I don't know if this can have a serious performance impact. My
    // benchmarks with a GeForce card under Linux shows no difference with
    // or without XSync, but when the GL window is rendered over a slow
    // network I have noticed bad event syncronisation problems when XSync
    // is not used, so I decided to use it.
    //XSync( _glfwLibrary.display, False );

    // Empty the window event queue
    while( XPending( _glfwLibrary.display ) )
    {
        if( GetNextEvent() )
        {
            winclosed = GL_TRUE;
        }
    }

    // Did we get mouse movement in locked cursor mode?
    if( _glfwInput.MouseMoved && _glfwWin.MouseLock )
    {
        int maxx, minx, maxy, miny;

        // Calculate movement threshold
        minx = _glfwWin.Width / 4;
        maxx = (_glfwWin.Width * 3) / 4;
        miny = _glfwWin.Height / 4;
        maxy = (_glfwWin.Height * 3) / 4;

        // Did the mouse cursor move beyond our movement threshold
        if(_glfwInput.CursorPosX < minx || _glfwInput.CursorPosX > maxx ||
           _glfwInput.CursorPosY < miny || _glfwInput.CursorPosY > maxy)
        {
            // Move the mouse pointer back to the window center so that it
            // does not wander off...
            _glfwPlatformSetMouseCursorPos( _glfwWin.Width/2,
                                            _glfwWin.Height/2 );
            //XSync( _glfwLibrary.display, False );
        }
    }

    // Was the window (un)iconified?
    if( _glfwWin.MapNotifyCount < 0 && !_glfwWin.Iconified )
    {
        // Show mouse pointer
        if( _glfwWin.PointerHidden )
        {
            XUndefineCursor( _glfwLibrary.display, _glfwWin.window );
            _glfwWin.PointerHidden = GL_FALSE;
        }

        // Un-grab mouse pointer
        if( _glfwWin.PointerGrabbed )
        {
            XUngrabPointer( _glfwLibrary.display, CurrentTime );
            _glfwWin.PointerGrabbed = GL_FALSE;
        }

        _glfwWin.Iconified = GL_TRUE;
    }
    else if( _glfwWin.MapNotifyCount > 0 && _glfwWin.Iconified )
    {
        // Restore fullscreen mode properties
        if( _glfwWin.Fullscreen )
        {
            // Change back video mode to user selected mode
            _glfwSetVideoMode( _glfwWin.screen, &_glfwWin.Width,
                               &_glfwWin.Height, &_glfwWin.RefreshRate );
            // Disable window manager decorations
            enableDecorations();

            // Make sure window is in upper left corner
            XMoveWindow( _glfwLibrary.display, _glfwWin.window, 0, 0 );

            // Get input focus
            XSetInputFocus( _glfwLibrary.display, _glfwWin.window,
                            RevertToParent, CurrentTime );
        }

        // Hide cursor if necessary
        if( _glfwWin.MouseLock && !_glfwWin.PointerHidden )
        {
            if( !_glfwWin.PointerHidden )
            {
                XDefineCursor( _glfwLibrary.display, _glfwWin.window,
                               CreateNULLCursor( _glfwLibrary.display,
                                                 _glfwWin.window ) );

                _glfwWin.PointerHidden = GL_TRUE;
            }
        }

        // Grab cursor if necessary
        if( (_glfwWin.MouseLock || _glfwWin.Fullscreen) &&
            !_glfwWin.PointerGrabbed )
        {
            if( XGrabPointer( _glfwLibrary.display, _glfwWin.window, True,
                    ButtonPressMask | ButtonReleaseMask |
                    PointerMotionMask, GrabModeAsync,
                    GrabModeAsync, _glfwWin.window, None,
                    CurrentTime ) == GrabSuccess )
            {
                _glfwWin.PointerGrabbed = GL_TRUE;
            }
        }

        _glfwWin.Iconified = GL_FALSE;
    }

    // Did the window get/lose focus
    if( _glfwWin.FocusInCount > 0 && !_glfwWin.Active )
    {
        // If we are in fullscreen mode, restore window
        if( _glfwWin.Fullscreen && _glfwWin.Iconified )
        {
            _glfwPlatformRestoreWindow();
        }

        // Window is now active
        _glfwWin.Active = GL_TRUE;
    }
    else if( _glfwWin.FocusInCount < 0 && _glfwWin.Active )
    {
        // If we are in fullscreen mode, iconfify window
        if( _glfwWin.Fullscreen )
        {
            _glfwPlatformIconifyWindow();
        }

        // Window is not active
        _glfwWin.Active = GL_FALSE;
        _glfwInputDeactivation();
    }

    // Was there a window close request?
    if( winclosed && _glfwWin.WindowCloseCallback )
    {
        // Check if the program wants us to close the window
        winclosed = _glfwWin.WindowCloseCallback();
    }
    if( winclosed )
    {
        glfwCloseWindow();
    }
}


//========================================================================
// _glfwPlatformWaitEvents() - Wait for new window and input events
//========================================================================

void _glfwPlatformWaitEvents( void )
{
    XEvent event;

    // Wait for new events (blocking)
    XNextEvent( _glfwLibrary.display, &event );
    XPutBackEvent( _glfwLibrary.display, &event );

    // Poll events from queue
    _glfwPlatformPollEvents();
}


//========================================================================
// _glfwPlatformHideMouseCursor() - Hide mouse cursor (lock it)
//========================================================================

void _glfwPlatformHideMouseCursor( void )
{
    // Hide cursor
    if( !_glfwWin.PointerHidden )
    {
        XDefineCursor( _glfwLibrary.display, _glfwWin.window,
                       CreateNULLCursor( _glfwLibrary.display,
                                         _glfwWin.window ) );

        _glfwWin.PointerHidden = GL_TRUE;
    }

    // Grab cursor to user window
    if( !_glfwWin.PointerGrabbed )
    {
        if( XGrabPointer( _glfwLibrary.display, _glfwWin.window, True,
                          ButtonPressMask | ButtonReleaseMask |
                          PointerMotionMask, GrabModeAsync, GrabModeAsync,
                          _glfwWin.window, None, CurrentTime ) ==
            GrabSuccess )
        {
            _glfwWin.PointerGrabbed = GL_TRUE;
        }
    }
}


//========================================================================
// _glfwPlatformShowMouseCursor() - Show mouse cursor (unlock it)
//========================================================================

void _glfwPlatformShowMouseCursor( void )
{
    // Un-grab cursor (only in windowed mode: in fullscreen mode we still
    // want the mouse grabbed in order to confine the cursor to the window
    // area)
    if( _glfwWin.PointerGrabbed && !_glfwWin.Fullscreen )
    {
        XUngrabPointer( _glfwLibrary.display, CurrentTime );
        _glfwWin.PointerGrabbed = GL_FALSE;
    }

    // Show cursor
    if( _glfwWin.PointerHidden )
    {
        XUndefineCursor( _glfwLibrary.display, _glfwWin.window );
        _glfwWin.PointerHidden = GL_FALSE;
    }
}


//========================================================================
// _glfwPlatformSetMouseCursorPos() - Set physical mouse cursor position
//========================================================================

void _glfwPlatformSetMouseCursorPos( int x, int y )
{
    // Change cursor position
    _glfwInput.CursorPosX = x;
    _glfwInput.CursorPosY = y;
    XWarpPointer( _glfwLibrary.display, None, _glfwWin.window, 0,0,0,0, x, y );
}

