//========================================================================
// GLFW - An OpenGL framework
// File:        win32_window.c
// Platform:    Windows
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
// $Id: win32_window.c,v 1.17 2005-01-20 22:30:39 marcus256 Exp $
//========================================================================

#include "internal.h"



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************


//========================================================================
// _glfwMinMaxAnimations() - Enable/disable minimize/restore animations
//========================================================================

static int _glfwMinMaxAnimations( int enable )
{
    ANIMATIONINFO AI;
    int old_enable;

    // Get old animation setting
    AI.cbSize = sizeof( ANIMATIONINFO );
    SystemParametersInfo( SPI_GETANIMATION, AI.cbSize, &AI, 0 );
    old_enable = AI.iMinAnimate;

    // If requested, change setting
    if( old_enable != enable )
    {
        AI.iMinAnimate = enable;
        SystemParametersInfo( SPI_SETANIMATION, AI.cbSize, &AI,
                              SPIF_SENDCHANGE );
    }

    return old_enable;
}


//========================================================================
// _glfwSetForegroundWindow() - Function for bringing a window into focus
// and placing it on top of the window z stack. Due to some nastiness
// with how Win98/ME/2k/XP handles SetForegroundWindow, we have to go
// through some really bizarre measures to achieve this (thanks again, MS,
// for making life so much easier)!
//========================================================================

static void _glfwSetForegroundWindow( HWND hWnd )
{
    int try_count = 0;
    int old_animate;

    // Try the standard approach first...
    BringWindowToTop( hWnd );
    SetForegroundWindow( hWnd );

    // If it worked, return now
    if( hWnd == GetForegroundWindow() )
    {
        // Try to modify the system settings (since this is the foreground
        // process, we are allowed to do this)
        SystemParametersInfo( SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0,
                              SPIF_SENDCHANGE );
        return;
    }

    // For other Windows versions than 95 & NT4.0, the standard approach
    // may not work, so if we failed we have to "trick" Windows into
    // making our window the foureground window: Iconify and restore
    // again. It is ugly, but it seems to work (we turn off those annoying
    // zoom animations to make it look a bit better at least).

    // Turn off minimize/restore animations
    old_animate = _glfwMinMaxAnimations( 0 );

    // We try this a few times, just to be on the safe side of things...
    do
    {
        // Iconify & restore
        ShowWindow( hWnd, SW_HIDE );
        ShowWindow( hWnd, SW_SHOWMINIMIZED );
        ShowWindow( hWnd, SW_SHOWNORMAL );

        // Try to get focus
        BringWindowToTop( hWnd );
        SetForegroundWindow( hWnd );

        // We do not want to keep going on forever, so we keep track of
        // how many times we tried
        try_count ++;
    }
    while( hWnd != GetForegroundWindow() && try_count <= 3 );

    // Restore the system minimize/restore animation setting
    (void) _glfwMinMaxAnimations( old_animate );

    // Try to modify the system settings (since this is now hopefully the
    // foreground process, we are probably allowed to do this)
    SystemParametersInfo( SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0,
                          SPIF_SENDCHANGE );
}


//========================================================================
// _glfwTranslateKey() - Translates a windows key to an internal coding
//========================================================================

static int _glfwTranslateKey( DWORD wParam, DWORD lParam )
{
    MSG next_msg;
    DWORD msg_time;
    DWORD scan_code;

    // Check which key was pressed or released
    switch( wParam )
    {
        // The SHIFT keys require special handling
        case VK_SHIFT:
            // Compare scan code for this key with that of VK_RSHIFT in
            // order to determine which shift key was pressed (left or
            // right)
            scan_code = MapVirtualKey( VK_RSHIFT, 0 );
            if( ((lParam & 0x01ff0000) >> 16) == scan_code )
            {
                return GLFW_KEY_RSHIFT;
            }
            return GLFW_KEY_LSHIFT;

        // The CTRL keys require special handling
        case VK_CONTROL:
            // Is this an extended key (i.e. right key)?
            if( lParam & 0x01000000 )
            {
                return GLFW_KEY_RCTRL;
            }
            // Here is a trick: "Alt Gr" sends LCTRL, then RALT. We only
            // want the RALT message, so we try to see if the next message
            // is a RALT message. In that case, this is a false LCTRL!
            msg_time = GetMessageTime();
            if( PeekMessage( &next_msg, NULL, 0, 0, PM_NOREMOVE ) )
            {
                if( next_msg.message == WM_KEYDOWN ||
                    next_msg.message == WM_SYSKEYDOWN )
                {
                    if( next_msg.wParam == VK_MENU &&
                        (next_msg.lParam & 0x01000000) &&
                        next_msg.time == msg_time )
                    {
                        // Next message is a RALT down message, which
                        // means that this is NOT a proper LCTRL message!
                        return GLFW_KEY_UNKNOWN;
                    }
                }
            }
            return GLFW_KEY_LCTRL;

        // The ALT keys require special handling
        case VK_MENU:
            // Is this an extended key (i.e. right key)?
            if( lParam & 0x01000000 )
            {
                return GLFW_KEY_RALT;
            }
            return GLFW_KEY_LALT;

        // The ENTER keys require special handling
        case VK_RETURN:
            // Is this an extended key (i.e. right key)?
            if( lParam & 0x01000000 )
            {
                return GLFW_KEY_KP_ENTER;
            }
            return GLFW_KEY_ENTER;

        // Special keys (non character keys)
        case VK_ESCAPE:        return GLFW_KEY_ESC;
        case VK_TAB:           return GLFW_KEY_TAB;
        case VK_BACK:          return GLFW_KEY_BACKSPACE;
        case VK_HOME:          return GLFW_KEY_HOME;
        case VK_END:           return GLFW_KEY_END;
        case VK_PRIOR:         return GLFW_KEY_PAGEUP;
        case VK_NEXT:          return GLFW_KEY_PAGEDOWN;
        case VK_INSERT:        return GLFW_KEY_INSERT;
        case VK_DELETE:        return GLFW_KEY_DEL;
        case VK_LEFT:          return GLFW_KEY_LEFT;
        case VK_UP:            return GLFW_KEY_UP;
        case VK_RIGHT:         return GLFW_KEY_RIGHT;
        case VK_DOWN:          return GLFW_KEY_DOWN;
        case VK_F1:            return GLFW_KEY_F1;
        case VK_F2:            return GLFW_KEY_F2;
        case VK_F3:            return GLFW_KEY_F3;
        case VK_F4:            return GLFW_KEY_F4;
        case VK_F5:            return GLFW_KEY_F5;
        case VK_F6:            return GLFW_KEY_F6;
        case VK_F7:            return GLFW_KEY_F7;
        case VK_F8:            return GLFW_KEY_F8;
        case VK_F9:            return GLFW_KEY_F9;
        case VK_F10:           return GLFW_KEY_F10;
        case VK_F11:           return GLFW_KEY_F11;
        case VK_F12:           return GLFW_KEY_F12;
        case VK_F13:           return GLFW_KEY_F13;
        case VK_F14:           return GLFW_KEY_F14;
        case VK_F15:           return GLFW_KEY_F15;
        case VK_F16:           return GLFW_KEY_F16;
        case VK_F17:           return GLFW_KEY_F17;
        case VK_F18:           return GLFW_KEY_F18;
        case VK_F19:           return GLFW_KEY_F19;
        case VK_F20:           return GLFW_KEY_F20;
        case VK_F21:           return GLFW_KEY_F21;
        case VK_F22:           return GLFW_KEY_F22;
        case VK_F23:           return GLFW_KEY_F23;
        case VK_F24:           return GLFW_KEY_F24;
        case VK_SPACE:         return GLFW_KEY_SPACE;

        // Numeric keypad
        case VK_NUMPAD0:       return GLFW_KEY_KP_0;
        case VK_NUMPAD1:       return GLFW_KEY_KP_1;
        case VK_NUMPAD2:       return GLFW_KEY_KP_2;
        case VK_NUMPAD3:       return GLFW_KEY_KP_3;
        case VK_NUMPAD4:       return GLFW_KEY_KP_4;
        case VK_NUMPAD5:       return GLFW_KEY_KP_5;
        case VK_NUMPAD6:       return GLFW_KEY_KP_6;
        case VK_NUMPAD7:       return GLFW_KEY_KP_7;
        case VK_NUMPAD8:       return GLFW_KEY_KP_8;
        case VK_NUMPAD9:       return GLFW_KEY_KP_9;
        case VK_DIVIDE:        return GLFW_KEY_KP_DIVIDE;
        case VK_MULTIPLY:      return GLFW_KEY_KP_MULTIPLY;
        case VK_SUBTRACT:      return GLFW_KEY_KP_SUBTRACT;
        case VK_ADD:           return GLFW_KEY_KP_ADD;
        case VK_DECIMAL:       return GLFW_KEY_KP_DECIMAL;

        // The rest (should be printable keys)
        default:
            // Convert to printable character (ISO-8859-1 or Unicode)
            wParam = MapVirtualKey( wParam, 2 ) & 0x0000FFFF;

            // Make sure that the character is uppercase
            if( _glfwSys.HasUnicode )
            {
                wParam = (DWORD) CharUpperW( (LPWSTR) wParam );
            }
            else
            {
                wParam = (DWORD) CharUpperA( (LPSTR) wParam );
            }

            // Valid ISO-8859-1 character?
            if( (wParam >=  32 && wParam <= 126) ||
                (wParam >= 160 && wParam <= 255) )
            {
                return (int) wParam;
            }
            return GLFW_KEY_UNKNOWN;
    }
}


//========================================================================
// _glfwTranslateChar() - Translates a windows key to Unicode
//========================================================================

static void _glfwTranslateChar( DWORD wParam, DWORD lParam, int action )
{
    BYTE  keyboard_state[ 256 ];
    UCHAR char_buf[ 10 ];
    WCHAR unicode_buf[ 10 ];
    UINT  scan_code;
    int   i, num_chars, unicode;

    // Get keyboard state
    GetKeyboardState( keyboard_state );

    // Derive scan code from lParam and action
    scan_code = (lParam & 0x01ff0000) >> 16;
    if( action == GLFW_RELEASE )
    {
        scan_code |= 0x8000000;
    }

    // Do we have Unicode support?
    if( _glfwSys.HasUnicode )
    {
        // Convert to Unicode
        num_chars = ToUnicode(
            wParam,          // virtual-key code
            scan_code,       // scan code
            keyboard_state,  // key-state array
            unicode_buf,     // buffer for translated key
            10,              // size of translated key buffer
            0                // active-menu flag
        );
        unicode = 1;
    }
    else
    {
        // Convert to ISO-8859-1
        num_chars = ToAscii(
            wParam,            // virtual-key code
            scan_code,         // scan code
            keyboard_state,    // key-state array
            (LPWORD) char_buf, // buffer for translated key
            0                  // active-menu flag
        );
        unicode = 0;
    }

    // Report characters
    for( i = 0; i < num_chars; i ++ )
    {
        // Get next character from buffer
        if( unicode )
        {
            _glfwInputChar( (int) unicode_buf[ i ], action );
        }
        else
        {
            _glfwInputChar( (int) char_buf[ i ], action );
        }
    }
}


//========================================================================
// _glfwWindowCallback() - Window callback function (handles window
// events)
//========================================================================

LRESULT CALLBACK _glfwWindowCallback( HWND hWnd, UINT uMsg,
     WPARAM wParam, LPARAM lParam )
{
    int WheelDelta, Iconified;

    // Handle certain window messages
    switch( uMsg )
    {
        // Window activate message? (iconification?)
        case WM_ACTIVATE:
            _glfwWin.Active = LOWORD(wParam) != WA_INACTIVE ? GL_TRUE :
                                                              GL_FALSE;
            Iconified = HIWORD(wParam) ? GL_TRUE : GL_FALSE;

            // Were we deactivated/iconified?
            if( (!_glfwWin.Active || Iconified) && !_glfwWin.Iconified )
            {
                // If we are in fullscreen mode we need to iconify
                if( _glfwWin.Fullscreen )
                {
                    // Do we need to manually iconify?
                    if( !Iconified )
                    {
                        // Minimize window
                        CloseWindow( _glfwWin.Wnd );

                        // The window is now iconified
                        Iconified = GL_TRUE;
                    }

                    // Change display settings to the desktop resolution
                    ChangeDisplaySettings( NULL, CDS_FULLSCREEN );
                }

                // Unlock mouse
                if( !_glfwWin.OldMouseLockValid )
                {
                    _glfwWin.OldMouseLock = _glfwWin.MouseLock;
                    _glfwWin.OldMouseLockValid = GL_TRUE;
                    glfwEnable( GLFW_MOUSE_CURSOR );
                }
            }
            else if( _glfwWin.Active || !Iconified )
            {
                // If we are in fullscreen mode we need to maximize
                if( _glfwWin.Fullscreen && _glfwWin.Iconified )
                {
                    // Change display settings to the user selected mode
                    _glfwSetVideoModeMODE( _glfwWin.ModeID );

                    // Do we need to manually restore window?
                    if( Iconified )
                    {
                        // Restore window
                        OpenIcon( _glfwWin.Wnd );

                        // The window is no longer iconified
                        Iconified = GL_FALSE;

                        // Activate window
                        ShowWindow( hWnd, SW_SHOW );
                        _glfwSetForegroundWindow( _glfwWin.Wnd );
                        SetFocus( _glfwWin.Wnd );
                    }
                }

                // Lock mouse, if necessary
                if( _glfwWin.OldMouseLockValid && _glfwWin.OldMouseLock )
                {
                    glfwDisable( GLFW_MOUSE_CURSOR );
                }
                _glfwWin.OldMouseLockValid = GL_FALSE;
            }

            _glfwWin.Iconified = Iconified;
            return 0;

        // Intercept system commands (forbid certain actions/events)
        case WM_SYSCOMMAND:
            switch( wParam )
            {
                // Screensaver trying to start or monitor trying to enter
                // powersave?
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                    if( _glfwWin.Fullscreen )
                    {
                        return 0;
                    }
                    else
                    {
                        break;
                    }

                // User trying to access application menu using ALT?
                case SC_KEYMENU:
                    return 0;
            }
            break;

        // Did we receive a close message?
        case WM_CLOSE:
            PostQuitMessage( 0 );
            return 0;

        // Is a key being pressed?
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            // Translate and report key press
            _glfwInputKey( _glfwTranslateKey( wParam, lParam ),
                           GLFW_PRESS );

            // Translate and report character input
            if( _glfwWin.CharCallback )
            {
                _glfwTranslateChar( wParam, lParam, GLFW_PRESS );
            }
            return 0;

        // Is a key being released?
        case WM_KEYUP:
        case WM_SYSKEYUP:
            // Special trick: release both shift keys on SHIFT up event
            if( wParam == VK_SHIFT )
            {
                _glfwInputKey( GLFW_KEY_LSHIFT, GLFW_RELEASE );
                _glfwInputKey( GLFW_KEY_RSHIFT, GLFW_RELEASE );
            }
            else
            {
                // Translate and report key release
                _glfwInputKey( _glfwTranslateKey( wParam, lParam ),
                               GLFW_RELEASE );
            }

            // Translate and report character input
            if( _glfwWin.CharCallback )
            {
                _glfwTranslateChar( wParam, lParam, GLFW_RELEASE );
            }
            return 0;

        // Were any of the mouse-buttons pressed?
        case WM_LBUTTONDOWN:
            SetCapture(hWnd);
            _glfwInputMouseClick( GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS );
            return 0;
        case WM_RBUTTONDOWN:
            SetCapture(hWnd);
            _glfwInputMouseClick( GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS );
            return 0;
        case WM_MBUTTONDOWN:
            SetCapture(hWnd);
            _glfwInputMouseClick( GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS );
            return 0;
        case WM_XBUTTONDOWN:
            if( HIWORD(wParam) == XBUTTON1 )
            {
                SetCapture(hWnd);
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_4, GLFW_PRESS );
            }
            else if( HIWORD(wParam) == XBUTTON2 )
            {
                SetCapture(hWnd);
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_5, GLFW_PRESS );
            }
            return 1;

        // Were any of the mouse-buttons released?
        case WM_LBUTTONUP:
            ReleaseCapture();
            _glfwInputMouseClick( GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE );
            return 0;
        case WM_RBUTTONUP:
            ReleaseCapture();
            _glfwInputMouseClick( GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE );
            return 0;
        case WM_MBUTTONUP:
            ReleaseCapture();
            _glfwInputMouseClick( GLFW_MOUSE_BUTTON_MIDDLE, GLFW_RELEASE );
            return 0;
        case WM_XBUTTONUP:
            if( HIWORD(wParam) == XBUTTON1 )
            {
                ReleaseCapture();
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_4, GLFW_RELEASE );
            }
            else if( HIWORD(wParam) == XBUTTON2 )
            {
                ReleaseCapture();
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_5, GLFW_RELEASE );
            }
            return 1;

        // Did the mouse move?
        case WM_MOUSEMOVE:
            {
                int NewMouseX, NewMouseY;

                // Get signed (!) mouse position
                NewMouseX = (int)((short)LOWORD(lParam));
                NewMouseY = (int)((short)HIWORD(lParam));

                if( NewMouseX != _glfwInput.OldMouseX ||
                    NewMouseY != _glfwInput.OldMouseY )
                {
                    if( _glfwWin.MouseLock )
                    {
                        _glfwInput.MousePosX += NewMouseX -
                                                _glfwInput.OldMouseX;
                        _glfwInput.MousePosY += NewMouseY -
                                                _glfwInput.OldMouseY;
                    }
                    else
                    {
                        _glfwInput.MousePosX = NewMouseX;
                        _glfwInput.MousePosY = NewMouseY;
                    }
                    _glfwInput.OldMouseX = NewMouseX;
                    _glfwInput.OldMouseY = NewMouseY;
                    _glfwInput.MouseMoved = GL_TRUE;
    
                    // Call user callback function
                    if( _glfwWin.MousePosCallback )
                    {
                        _glfwWin.MousePosCallback( _glfwInput.MousePosX,
                                                   _glfwInput.MousePosY );
                    }
                }
            }
            return 0;

        // Mouse wheel action?
        case WM_MOUSEWHEEL:
            // WM_MOUSEWHEEL is not supported under Windows 95
            if( _glfwSys.WinVer != _GLFW_WIN_95 )
            {
                WheelDelta = (((int)wParam) >> 16) / WHEEL_DELTA;
                _glfwInput.WheelPos += WheelDelta;
                if( _glfwWin.MouseWheelCallback )
                {
                    _glfwWin.MouseWheelCallback( _glfwInput.WheelPos );
                }
                return 0;
            }
            break;

        // Resize the window?
        case WM_SIZE:
            _glfwWin.Width  = LOWORD(lParam);
            _glfwWin.Height = HIWORD(lParam);
            if( _glfwWin.WindowSizeCallback )
            {
                _glfwWin.WindowSizeCallback( LOWORD(lParam),
                                             HIWORD(lParam) );
            }
            return 0;

        // Was the window contents damaged?
        case WM_PAINT:
            // Call user callback function
            if( _glfwWin.WindowRefreshCallback )
            {
                _glfwWin.WindowRefreshCallback();
            }
            break;

        default:
            break;
    }

    // Pass all unhandled messages to DefWindowProc
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


//========================================================================
// _glfwGetFullWindowSize() - Translate client window size to full window
// size (including window borders)
//========================================================================

static void _glfwGetFullWindowSize( int w, int h, int *w2, int *h2 )
{
    RECT rect;

    // Create a window rectangle
    rect.left   = (long)0;
    rect.right  = (long)w-1;
    rect.top    = (long)0;
    rect.bottom = (long)h-1;

    // Adjust according to window styles
    AdjustWindowRectEx( &rect, _glfwWin.dwStyle, FALSE,
                        _glfwWin.dwExStyle );

    // Calculate width and height of full window
    *w2 = rect.right-rect.left+1;
    *h2 = rect.bottom-rect.top+1;
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
    GLuint      PixelFormat;
    WNDCLASS    wc;
    DWORD       dwStyle, dwExStyle;
    int         full_width, full_height;
    PIXELFORMATDESCRIPTOR pfd;
    RECT        wa;

    // Clear platform specific GLFW window state
    _glfwWin.DC                = NULL;
    _glfwWin.RC                = NULL;
    _glfwWin.Wnd               = NULL;
    _glfwWin.Instance          = NULL;
    _glfwWin.OldMouseLockValid = GL_FALSE;

    // Remember desired refresh rate for this window (used only in
    // fullscreen mode)
    _glfwWin.DesiredRefreshRate = refreshrate;

    // Grab an instance for our window
    _glfwWin.Instance = GetModuleHandle( NULL );

    // Set window class parameters
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Redraw on...
    wc.lpfnWndProc   = (WNDPROC)_glfwWindowCallback;  // Message handler
    wc.cbClsExtra    = 0;                             // No extra class data
    wc.cbWndExtra    = 0;                             // No extra window data
    wc.hInstance     = _glfwWin.Instance;             // Set instance
    wc.hIcon         = LoadIcon( NULL, IDI_WINLOGO ); // Load default icon
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW ); // Load arrow pointer
    wc.hbrBackground = NULL;                          // No background
    wc.lpszMenuName  = NULL;                          // No menu
    wc.lpszClassName = "GLFW";                        // Set class name

    // Register the window class
    if( !RegisterClass( &wc ) )
    {
        _glfwWin.Instance = NULL;
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Do we want full-screen mode?
    if( _glfwWin.Fullscreen )
    {
        _glfwSetVideoMode( &_glfwWin.Width, &_glfwWin.Height,
                           redbits, greenbits, bluebits,
                           refreshrate );
    }

    // Set common window styles
    dwStyle   = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
    dwExStyle = WS_EX_APPWINDOW;

    // Set window style, depending on fullscreen mode
    if( _glfwWin.Fullscreen )
    {
        dwStyle   |= WS_POPUP;
    }
    else
    {
        dwStyle   |= WS_OVERLAPPEDWINDOW;
        dwExStyle |= WS_EX_WINDOWEDGE;
    }

    // Remember window styles (used by _glfwGetFullWindowSize)
    _glfwWin.dwStyle   = dwStyle;
    _glfwWin.dwExStyle = dwExStyle;

    // Set window size to true requested size (adjust for window borders)
    _glfwGetFullWindowSize( _glfwWin.Width, _glfwWin.Height, &full_width,
                            &full_height );

    // Adjust window position to working area (e.g. if the task bar is at
    // the top of the display)...
    SystemParametersInfo( SPI_GETWORKAREA, 0, &wa, 0 );

    // Create window
    _glfwWin.Wnd = CreateWindowEx(
               dwExStyle,                 // Extended style
               "GLFW",                    // Class name
               "GLFW Window",             // Window title
               dwStyle,                   // Defined window style
               wa.left, wa.top,           // Window position
               full_width,                // Decorated window width
               full_height,               // Decorated window height
               NULL,                      // No parent window
               NULL,                      // No menu
               _glfwWin.Instance,         // Instance
               NULL );                    // Nothing to WM_CREATE
    if( !_glfwWin.Wnd )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Get a device context
    _glfwWin.DC = GetDC( _glfwWin.Wnd );
    if( !_glfwWin.DC )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Set required pixel format
    pfd.nSize           = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion        = 1;
    pfd.dwFlags         = PFD_DRAW_TO_WINDOW | // Draw to window
                          PFD_SUPPORT_OPENGL | // Support OpenGL
                          PFD_DOUBLEBUFFER;    // Double buffered window
    pfd.iPixelType      = PFD_TYPE_RGBA;       // Request an RGBA format
    pfd.cColorBits      = (BYTE) (redbits+
                                  greenbits+
                                  bluebits);   // Color bits (ex. alpha)
    pfd.cRedBits        = (BYTE) redbits;      // Red bits
    pfd.cRedShift       = 0;                   // Red shift ignored
    pfd.cGreenBits      = (BYTE) greenbits;    // Green bits
    pfd.cGreenShift     = 0;                   // Green shift ignored
    pfd.cBlueBits       = (BYTE) bluebits;     // Blue bits
    pfd.cBlueShift      = 0;                   // Blue shift ignored
    pfd.cAlphaBits      = (BYTE) alphabits;    // Alpha bits
    pfd.cAlphaShift     = 0;                   // Alpha shift ignored
    pfd.cAccumBits      = (BYTE) (accumredbits+
                                  accumgreenbits+
                                  accumbluebits+
                                  accumalphabits); // Accum. bits
    pfd.cAccumRedBits   = (BYTE) accumredbits;   // Accum. red bits
    pfd.cAccumGreenBits = (BYTE) accumgreenbits; // Accum. green bits
    pfd.cAccumBlueBits  = (BYTE) accumbluebits;  // Accum. blue bits
    pfd.cAccumAlphaBits = (BYTE) accumalphabits; // Accum. alpha bits
    pfd.cDepthBits      = (BYTE) depthbits;    // Depth buffer bits
    pfd.cStencilBits    = (BYTE) stencilbits;  // Stencil buffer bits
    pfd.cAuxBuffers     = (BYTE) auxbuffers;   // No. of aux buffers
    pfd.iLayerType      = PFD_MAIN_PLANE;      // Drawing layer: main
    pfd.bReserved       = 0;                   // (reserved)
    pfd.dwLayerMask     = 0;                   // Ignored
    pfd.dwVisibleMask   = 0;                   // "
    pfd.dwDamageMask    = 0;                   // "
    if( depthbits <= 0 )
    {
        // We do not need a depth buffer
        pfd.dwFlags |= PFD_DEPTH_DONTCARE;
    }
    if( stereo )
    {
        // Request a stereo mode
        pfd.dwFlags |= PFD_STEREO;
    }

    // Find a matching pixel format
    if( !(PixelFormat = _glfw_ChoosePixelFormat( _glfwWin.DC, &pfd )) )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Get actual pixel format description
    if( !_glfw_DescribePixelFormat( _glfwWin.DC, PixelFormat,
            sizeof(PIXELFORMATDESCRIPTOR), &pfd) )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // "stereo" is a strict requirement
    if( stereo && !(pfd.dwFlags & PFD_STEREO) )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Set the pixel-format
    if( !_glfw_SetPixelFormat( _glfwWin.DC, PixelFormat, &pfd ) )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Get a rendering context
    _glfwWin.RC = wglCreateContext(_glfwWin.DC);
    if( !_glfwWin.RC )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Activate the OpenGL rendering context
    if( !wglMakeCurrent( _glfwWin.DC, _glfwWin.RC ) )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Make sure that our window ends up on top of things
    if( _glfwWin.Fullscreen )
    {
        // Place the window above all topmost windows
        SetWindowPos( _glfwWin.Wnd, HWND_TOPMOST, 0,0,0,0,
                      SWP_NOMOVE | SWP_NOSIZE );
    }
    _glfwSetForegroundWindow( _glfwWin.Wnd );
    SetFocus( _glfwWin.Wnd );

    // Start by clearing the front buffer to black (avoid ugly desktop
    // remains in our OpenGL window)
    glClear( GL_COLOR_BUFFER_BIT );
    _glfw_SwapBuffers( _glfwWin.DC );

    // Initialize OpenGL extension: WGL_EXT_swap_control
    _glfwWin.SwapInterval = (WGLSWAPINTERVALEXT_T)
                            wglGetProcAddress("wglSwapIntervalEXT");

    return GL_TRUE;
}


//========================================================================
// _glfwPlatformCloseWindow() - Properly kill the window / video display
//========================================================================

void _glfwPlatformCloseWindow( void )
{
    // Do we have a rendering context?
    if( _glfwWin.RC )
    {
        // Release the DC and RC contexts
        wglMakeCurrent( NULL, NULL );

        // Delete the rendering context
        wglDeleteContext( _glfwWin.RC );
        _glfwWin.RC = NULL;
    }

    // Do we have a device context?
    if( _glfwWin.DC )
    {
        // Release the device context
        ReleaseDC( _glfwWin.Wnd, _glfwWin.DC );
        _glfwWin.DC = NULL;
    }

    // Do we have a window?
    if( _glfwWin.Wnd )
    {
        // Destroy the window
        if( _glfwSys.WinVer <= _GLFW_WIN_NT4 )
        {
            // Note: Hiding the window first fixes an annoying W98/NT4
            // remaining icon bug for fullscreen displays
            ShowWindow( _glfwWin.Wnd, SW_HIDE );
        }
        DestroyWindow( _glfwWin.Wnd );
        _glfwWin.Wnd = NULL;
    }

    // Do we have an instance?
    if( _glfwWin.Instance )
    {
        // Unregister class
        UnregisterClass( "GLFW", _glfwWin.Instance );
        _glfwWin.Instance = NULL;
    }

    // Are we in fullscreen mode?
    if( _glfwWin.Fullscreen )
    {
        // Switch back to desktop resolution
        ChangeDisplaySettings( NULL, CDS_FULLSCREEN );
    }

}


//========================================================================
// _glfwPlatformSetWindowTitle() - Set the window title
//========================================================================

void _glfwPlatformSetWindowTitle( const char *title )
{
    // Set window title
    (void) SetWindowText( _glfwWin.Wnd, title );
}


//========================================================================
// _glfwPlatformSetWindowSize() - Set the window size.
//========================================================================

void _glfwPlatformSetWindowSize( int width, int height )
{
    int     bpp, mode = 0, refresh;
    int     sizechanged = GL_FALSE;
    GLint   drawbuffer;
    GLfloat clearcolor[4];

    // If we are in fullscreen mode, get some info about the current mode
    if( _glfwWin.Fullscreen )
    {
        DEVMODE dm;
        int success;

        // Get current BPP settings
        dm.dmSize = sizeof( DEVMODE );
        success = EnumDisplaySettings( NULL, _glfwWin.ModeID, &dm );
        if( success )
        {
            // Get bpp
            bpp = dm.dmBitsPerPel;

            // Get closest match for target video mode
            refresh = _glfwWin.DesiredRefreshRate;
            mode = _glfwGetClosestVideoModeBPP( &width, &height, &bpp,
                                                &refresh );
        }
        else
        {
            mode = _glfwWin.ModeID;
        }
    }
    else
    {
        // If we are in windowed mode, adjust the window size to
        // compensate for window decorations
        _glfwGetFullWindowSize( width, height, &width, &height );
    }

    // Change window size before changing fullscreen mode?
    if( _glfwWin.Fullscreen && (width > _glfwWin.Width) )
    {
        SetWindowPos( _glfwWin.Wnd, HWND_TOP, 0, 0, width, height,
                      SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER );
        sizechanged = GL_TRUE;
    }

    // Change fullscreen video mode?
    if( _glfwWin.Fullscreen && mode != _glfwWin.ModeID )
    {
        // Change video mode
        _glfwSetVideoModeMODE( mode );

        // Clear the front buffer to black (avoid ugly desktop remains in
        // our OpenGL window)
        glGetIntegerv( GL_DRAW_BUFFER, &drawbuffer );
        glGetFloatv( GL_COLOR_CLEAR_VALUE, clearcolor );
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT );
        if( drawbuffer == GL_BACK )
        {
            _glfw_SwapBuffers( _glfwWin.DC );
        }
        glClearColor( clearcolor[0], clearcolor[1], clearcolor[2],
                      clearcolor[3] );
    }

    // Set window size (if not already changed)
    if( !sizechanged )
    {
        SetWindowPos( _glfwWin.Wnd, HWND_TOP, 0, 0, width, height,
                      SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER );
    }
}


//========================================================================
// _glfwPlatformSetWindowPos() - Set the window position
//========================================================================

void _glfwPlatformSetWindowPos( int x, int y )
{
    // Set window position
    (void) SetWindowPos( _glfwWin.Wnd, HWND_TOP, x, y, 0, 0,
                         SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER );
}


//========================================================================
// _glfwPlatformIconfyWindow() - Window iconification
//========================================================================

void _glfwPlatformIconifyWindow( void )
{
    // Iconify window
    CloseWindow( _glfwWin.Wnd );

    // Window is now iconified
    _glfwWin.Iconified = GL_TRUE;

    // If we are in fullscreen mode we need to change video modes
    if( _glfwWin.Fullscreen )
    {
        // Change display settings to the desktop resolution
        ChangeDisplaySettings( NULL, CDS_FULLSCREEN );
    }

    // Unlock mouse
    if( !_glfwWin.OldMouseLockValid )
    {
        _glfwWin.OldMouseLock = _glfwWin.MouseLock;
        _glfwWin.OldMouseLockValid = GL_TRUE;
        glfwEnable( GLFW_MOUSE_CURSOR );
    }
}


//========================================================================
// _glfwPlatformRestoreWindow() - Window un-iconification
//========================================================================

void _glfwPlatformRestoreWindow( void )
{
    // If we are in fullscreen mode we need to change video modes
    if( _glfwWin.Fullscreen )
    {
        // Change display settings to the user selected mode
        _glfwSetVideoModeMODE( _glfwWin.ModeID );
    }

    // Un-iconify window
    OpenIcon( _glfwWin.Wnd );

    // Make sure that our window ends up on top of things
    ShowWindow( _glfwWin.Wnd, SW_SHOW );
    _glfwSetForegroundWindow( _glfwWin.Wnd );
    SetFocus( _glfwWin.Wnd );

    // Window is no longer iconified
    _glfwWin.Iconified = GL_FALSE;

    // Lock mouse, if necessary
    if( _glfwWin.OldMouseLockValid && _glfwWin.OldMouseLock )
    {
        glfwDisable( GLFW_MOUSE_CURSOR );
    }
    _glfwWin.OldMouseLockValid = GL_FALSE;
}


//========================================================================
// _glfwPlatformSwapBuffers() - Swap buffers (double-buffering)
//========================================================================

void _glfwPlatformSwapBuffers( void )
{
    _glfw_SwapBuffers( _glfwWin.DC );
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
// _glfwPlatformRefreshWindowParams()
//========================================================================

void _glfwPlatformRefreshWindowParams( void )
{
    PIXELFORMATDESCRIPTOR pfd;
    DEVMODE dm;
    int     iPixelFormat, success, mode;

    // Obtain a detailed description of current pixel format
    iPixelFormat = _glfw_GetPixelFormat( _glfwWin.DC );
    _glfw_DescribePixelFormat( _glfwWin.DC, iPixelFormat,
                               sizeof(PIXELFORMATDESCRIPTOR), &pfd );

    // Is current OpenGL context accelerated?
    _glfwWin.Accelerated = (pfd.dwFlags & PFD_GENERIC_ACCELERATED) ||
                           !(pfd.dwFlags & PFD_GENERIC_FORMAT) ? 1 : 0;

    // "Standard" window parameters
    _glfwWin.RedBits        = pfd.cRedBits;
    _glfwWin.GreenBits      = pfd.cGreenBits;
    _glfwWin.BlueBits       = pfd.cBlueBits;
    _glfwWin.AlphaBits      = pfd.cAlphaBits;
    _glfwWin.DepthBits      = pfd.cDepthBits;
    _glfwWin.StencilBits    = pfd.cStencilBits;
    _glfwWin.AccumRedBits   = pfd.cAccumRedBits;
    _glfwWin.AccumGreenBits = pfd.cAccumGreenBits;
    _glfwWin.AccumBlueBits  = pfd.cAccumBlueBits;
    _glfwWin.AccumAlphaBits = pfd.cAccumAlphaBits;
    _glfwWin.AuxBuffers     = pfd.cAuxBuffers;
    _glfwWin.Stereo         = pfd.dwFlags & PFD_STEREO ? 1 : 0;

    // Get refresh rate
    mode = _glfwWin.Fullscreen ? _glfwWin.ModeID : ENUM_CURRENT_SETTINGS;
    dm.dmSize = sizeof( DEVMODE );
    success = EnumDisplaySettings( NULL, mode, &dm );
    if( success )
    {
        _glfwWin.RefreshRate = dm.dmDisplayFrequency;
        if( _glfwWin.RefreshRate <= 1 )
        {
            _glfwWin.RefreshRate = 0;
        }
    }
    else
    {
        _glfwWin.RefreshRate = 0;
    }
}


//========================================================================
// _glfwPlatformPollEvents() - Poll for new window and input events
//========================================================================

void _glfwPlatformPollEvents( void )
{
    MSG msg;
    int winclosed = GL_FALSE;

    // Flag: mouse was not moved (will be changed by _glfwGetNextEvent if
    // there was a mouse move event)
    _glfwInput.MouseMoved = GL_FALSE;
    if( _glfwWin.MouseLock )
    {
        _glfwInput.OldMouseX = _glfwWin.Width/2;
        _glfwInput.OldMouseY = _glfwWin.Height/2;
    }
    else
    {
        _glfwInput.OldMouseX = _glfwInput.MousePosX;
        _glfwInput.OldMouseY = _glfwInput.MousePosY;
    }

    // Check for new window messages
    while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
    {
        switch( msg.message )
        {
            // QUIT-message (from close window)?
            case WM_QUIT:
                winclosed = GL_TRUE;
                break;

            // Ok, send it to the window message handler
            default:
                DispatchMessage( &msg );
                break;
        }
    }

    // LSHIFT/RSHIFT fixup (keys tend to "stick" without this fix)
    // This is the only async event handling in GLFW, but it solves some
    // nasty problems.
    // Caveat: Does not work under Win 9x/ME.
    if( _glfwSys.WinVer >= _GLFW_WIN_NT4 )
    {
        int lshift_down, rshift_down;

        // Get current state of left and right shift keys
        lshift_down = (GetAsyncKeyState( VK_LSHIFT ) >> 15) & 1;
        rshift_down = (GetAsyncKeyState( VK_RSHIFT ) >> 15) & 1;

        // See if this differs from our belief of what has happened
        // (we only have to check for lost key up events)
        if( !lshift_down && _glfwInput.Key[ GLFW_KEY_LSHIFT ] == 1 )
        {
            _glfwInputKey( GLFW_KEY_LSHIFT, GLFW_RELEASE );
        }
        if( !rshift_down && _glfwInput.Key[ GLFW_KEY_RSHIFT ] == 1 )
        {
            _glfwInputKey( GLFW_KEY_RSHIFT, GLFW_RELEASE );
        }
    }

    // Did we have mouse movement in locked cursor mode?
    if( _glfwInput.MouseMoved && _glfwWin.MouseLock )
    {
        _glfwPlatformSetMouseCursorPos( _glfwWin.Width>>1,
                                        _glfwWin.Height>>1 );
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
    // Wait for new events
    WaitMessage();

    // Poll new events
    _glfwPlatformPollEvents();
}


//========================================================================
// _glfwPlatformHideMouseCursor() - Hide mouse cursor (lock it)
//========================================================================

void _glfwPlatformHideMouseCursor( void )
{
    // Hide cursor
    ShowCursor( FALSE );

    // Capture cursor to user window
    SetCapture( _glfwWin.Wnd );
}


//========================================================================
// _glfwPlatformShowMouseCursor() - Show mouse cursor (unlock it)
//========================================================================

void _glfwPlatformShowMouseCursor( void )
{
    // Un-capture cursor
    ReleaseCapture();

    // Show cursor
    ShowCursor( TRUE );
}


//========================================================================
// _glfwPlatformSetMouseCursorPos() - Set physical mouse cursor position
//========================================================================

void _glfwPlatformSetMouseCursorPos( int x, int y )
{
    POINT pos;

    // Convert client coordinates to screen coordinates
    pos.x = x;
    pos.y = y;
    ClientToScreen( _glfwWin.Wnd, &pos );

    // Change cursor position
    SetCursorPos( pos.x, pos.y );
}
