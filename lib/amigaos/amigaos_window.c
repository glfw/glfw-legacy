//========================================================================
// GLFW - An OpenGL framework
// File:     amigaos_window.c
// Platform: AmigaOS (3.x)
// Version:  2.4
// Date:     2003.01.08
// Author:   Marcus Geelnard (marcus.geelnard@home.se)
// WWW:      http://hem.passagen.se/opengl/glfw/
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
// marcus.geelnard@home.se
//========================================================================

#include "internal.h"


//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwTranslateChar() - Translates an AmigaOS key to Unicode
//========================================================================

static int _glfwTranslateChar( struct IntuiMessage *msg )
{
    struct InputEvent event;
    unsigned char     buffer[ 4 ];
    int               character;

    // Create input event
    event.ie_Class        = IECLASS_RAWKEY;
    event.ie_Code         = msg->Code;
    event.ie_Qualifier    = msg->Qualifier;
    event.ie_EventAddress = msg->IAddress;

    // Map key event to text string
    if( MapRawKey( &event, buffer, 4, NULL ) > 0 )
    {
        // Valid Unicode character?
        character = (int) buffer[ 0 ];
        if( (character >= 32  && character <= 126) ||
            (character >= 160 && character <= 255) )
        {
            return character;
        }
    }

    return -1;
}


//========================================================================
// _glfwTranslateKey() - Translates an AmigaOS key to internal coding
//========================================================================

static int _glfwTranslateKey( struct IntuiMessage *msg )
{
    int key = msg->Code & 0x7F;
    ULONG old_qualifier;

    // Special (non printable) keys
    switch( key )
    {
        // Modifier keys
        case 0x60: return GLFW_KEY_LSHIFT;
        case 0x61: return GLFW_KEY_RSHIFT;
        case 0x62: return GLFW_KEY_LCTRL;  // ?
        case 0x63: return GLFW_KEY_RCTRL;  // ?
        case 0x64: return GLFW_KEY_LALT;
        case 0x65: return GLFW_KEY_RALT;

        // Function keys
        case 0x50: return GLFW_KEY_F1;
        case 0x51: return GLFW_KEY_F2;
        case 0x52: return GLFW_KEY_F3;
        case 0x53: return GLFW_KEY_F4;
        case 0x54: return GLFW_KEY_F5;
        case 0x55: return GLFW_KEY_F6;
        case 0x56: return GLFW_KEY_F7;
        case 0x57: return GLFW_KEY_F8;
        case 0x58: return GLFW_KEY_F9;
        case 0x59: return GLFW_KEY_F10;

        // Other control keys
        case 0x45: return GLFW_KEY_ESC;
        case 0x42: return GLFW_KEY_TAB;
        case 0x44: return GLFW_KEY_ENTER;
        case 0x46: return GLFW_KEY_DEL;
        case 0x41: return GLFW_KEY_BACKSPACE;
        case 0x66: return GLFW_KEY_INSERT; // ?
        case 0x4F: return GLFW_KEY_LEFT;
        case 0x4E: return GLFW_KEY_RIGHT;
        case 0x4C: return GLFW_KEY_UP;
        case 0x4D: return GLFW_KEY_DOWN;

        // Keypad keys
        case 0x0F: return GLFW_KEY_KP_0;
        case 0x1D: return GLFW_KEY_KP_1;
        case 0x1E: return GLFW_KEY_KP_2;
        case 0x1F: return GLFW_KEY_KP_3;
        case 0x2D: return GLFW_KEY_KP_4;
        case 0x2E: return GLFW_KEY_KP_5;
        case 0x2F: return GLFW_KEY_KP_6;
        case 0x3D: return GLFW_KEY_KP_7;
        case 0x3E: return GLFW_KEY_KP_8;
        case 0x3F: return GLFW_KEY_KP_9;
        case 0x43: return GLFW_KEY_KP_ENTER;
        case 0x5E: return GLFW_KEY_KP_ADD;
        case 0x4A: return GLFW_KEY_KP_SUBTRACT;
        case 0x5D: return GLFW_KEY_KP_MULTIPLY;
        case 0x5C: return GLFW_KEY_KP_DIVIDE;
        case 0x3C: return GLFW_KEY_KP_DECIMAL;

        default:   break;
    }

    // Printable keys (without modifiers!)
    old_qualifier  = msg->Qualifier;
    msg->Qualifier = 0;
    key            = _glfwTranslateChar( msg );
    msg->Qualifier = old_qualifier;
    if( key > 0 )
    {
        // Make sure it is upper case
        key = ToUpper( key );
    }

    return key;
}


//========================================================================
// _glfwProcessEvents() - Process all pending AmigaOS events
//========================================================================

static int _glfwProcessEvents( void )
{
    struct IntuiMessage message, *tmp_message = NULL;
    struct MsgPort      *msg_port;
    int                 win_closed = GL_FALSE, action;
    int                 x, y;

    // Examine pending messages
    msg_port = _glfwWin.Window->UserPort;
    while( tmp_message = (struct IntuiMessage *) GetMsg( msg_port ) )
    {
        // Copy contents of message structure
        message = *tmp_message;

        // Now reply to the message (we don't need it anymore)
        ReplyMsg( (struct Message *) tmp_message );

        // Handle different messages
        switch( message.Class )
        {

        // Was the window activated?
        case IDCMP_ACTIVEWINDOW:
            _glfwWin.Active = GL_TRUE;
            break;

        // Was the window deactivated?
        case IDCMP_INACTIVEWINDOW:
            _glfwWin.Active = GL_FALSE;
            break;

        // Did we get a keyboard press or release?
        case IDCMP_RAWKEY:
            action = (message.Code & 0x80) ? GLFW_RELEASE : GLFW_PRESS;
            message.Code &= 0x7F;
            _glfwInputKey( _glfwTranslateKey( &message ), action );
            _glfwInputChar( _glfwTranslateChar( &message ), action );
            break;

        // Was the mouse moved?
        case IDCMP_MOUSEMOVE:
            x = message.MouseX;
            y = message.MouseY;
            if( _glfwWin.PointerHidden )
            {
                // When pointer is hidden, we get delta moves
                x += _glfwInput.MousePosX;
                y += _glfwInput.MousePosY;
            }
            else if( x < 0 || x >= _glfwWin.Width ||
                     y < 0 || y >= _glfwWin.Height )
            {
                // Only report mouse moves that are INSIDE client area
                break;
            }
            if( x != _glfwInput.MousePosX || y != _glfwInput.MousePosY )
            {
                _glfwInput.MousePosX = x;
                _glfwInput.MousePosY = y;
                if( _glfwWin.MousePosCallback )
                {
                    _glfwWin.MousePosCallback( x, y );
                }
            }
            break;

        // Did we get a mouse button event?
        case IDCMP_MOUSEBUTTONS:
            switch( message.Code )
            {
            case SELECTUP:
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_LEFT,
                                      GLFW_RELEASE );
                break;
            case SELECTDOWN:
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_LEFT,
                                      GLFW_PRESS );
                break;
            case MENUUP:
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_RIGHT,
                                      GLFW_RELEASE );
                break;
            case MENUDOWN:
                _glfwInputMouseClick( GLFW_MOUSE_BUTTON_RIGHT,
                                      GLFW_PRESS );
                break;
            default:
                break;
            }
            break;

        // Was the window size changed?
        case IDCMP_NEWSIZE:
            _glfwWin.Width  = message.IDCMPWindow->GZZWidth;
            _glfwWin.Height = message.IDCMPWindow->GZZHeight;
            if( _glfwWin.WindowSizeCallback )
            {
                _glfwWin.WindowSizeCallback( _glfwWin.Width,
                                             _glfwWin.Height );
            }
            break;

        // Was the window closed?
        case IDCMP_CLOSEWINDOW:
            win_closed = GL_TRUE;
            break;

        default:
            break;
        }
    }

    // Return GL_TRUE if window was closed
    return( win_closed );
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
    struct TagItem tagList[ 25 ];
    int tagNR, accumbits;

    // Calculate sum of accumulator bits
    accumbits = accumredbits + accumgreenbits + accumbluebits +
                accumalphabits;

    // Clear window state
    _glfwWin.Screen        = NULL;
    _glfwWin.Window        = NULL;
    _glfwWin.Context       = NULL;
    _glfwWin.PointerHidden = 0;
    _glfwWin.PointerSprite = NULL;

    // Do we want fullscreen?
    if( _glfwWin.Fullscreen )
    {
        // Open a new Amiga screen
        if( !_glfwOpenScreen( &width, &height, &redbits, &greenbits,
                              &bluebits, refreshrate ) )
        {
            _glfwWin.Fullscreen = GL_FALSE;
        }
    }

    // Select window properties
    tagNR = 0;
    tagList[ tagNR   ].ti_Tag  = WA_Left;
    tagList[ tagNR++ ].ti_Data = 0;
    tagList[ tagNR   ].ti_Tag  = WA_Top;
    tagList[ tagNR++ ].ti_Data = 0;
    tagList[ tagNR   ].ti_Tag  = WA_IDCMP;
    tagList[ tagNR++ ].ti_Data = IDCMP_CLOSEWINDOW |
                                 IDCMP_NEWSIZE |
                                 IDCMP_ACTIVEWINDOW |
                                 IDCMP_INACTIVEWINDOW |
                                 IDCMP_RAWKEY |
                                 IDCMP_MOUSEMOVE |
                                 IDCMP_MOUSEBUTTONS;
    tagList[ tagNR   ].ti_Tag  = WA_ReportMouse;
    tagList[ tagNR++ ].ti_Data = TRUE;
    tagList[ tagNR   ].ti_Tag  = WA_RMBTrap;
    tagList[ tagNR++ ].ti_Data = TRUE;
    tagList[ tagNR   ].ti_Tag  = WA_NoCareRefresh;
    tagList[ tagNR++ ].ti_Data = TRUE;
    tagList[ tagNR   ].ti_Tag  = WA_Activate;
    tagList[ tagNR++ ].ti_Data = TRUE;
    tagList[ tagNR   ].ti_Tag  = WA_CloseGadget;
    tagList[ tagNR++ ].ti_Data = _glfwWin.Fullscreen ? FALSE : TRUE;
    tagList[ tagNR   ].ti_Tag  = WA_SizeGadget;
    tagList[ tagNR++ ].ti_Data = _glfwWin.Fullscreen ? FALSE : TRUE;
    tagList[ tagNR   ].ti_Tag  = WA_DepthGadget;
    tagList[ tagNR++ ].ti_Data = _glfwWin.Fullscreen ? FALSE : TRUE;
    tagList[ tagNR   ].ti_Tag  = WA_DragBar;
    tagList[ tagNR++ ].ti_Data = _glfwWin.Fullscreen ? FALSE : TRUE;
    tagList[ tagNR   ].ti_Tag  = WA_Borderless;
    tagList[ tagNR++ ].ti_Data = _glfwWin.Fullscreen ? TRUE : FALSE;
    tagList[ tagNR   ].ti_Tag  = WA_Backdrop;
    tagList[ tagNR++ ].ti_Data = _glfwWin.Fullscreen ? TRUE : FALSE;
    if( _glfwWin.Fullscreen )
    {
        tagList[ tagNR   ].ti_Tag  = WA_CustomScreen;
        tagList[ tagNR++ ].ti_Data = (ULONG) _glfwWin.Screen;
        tagList[ tagNR   ].ti_Tag  = WA_Width;
        tagList[ tagNR++ ].ti_Data = width;
        tagList[ tagNR   ].ti_Tag  = WA_Height;
        tagList[ tagNR++ ].ti_Data = height;
    }
    else
    {
        tagList[ tagNR   ].ti_Tag  = WA_GimmeZeroZero;
        tagList[ tagNR++ ].ti_Data = TRUE;
        tagList[ tagNR   ].ti_Tag  = WA_InnerWidth;
        tagList[ tagNR++ ].ti_Data = width;
        tagList[ tagNR   ].ti_Tag  = WA_InnerHeight;
        tagList[ tagNR++ ].ti_Data = height;
        tagList[ tagNR   ].ti_Tag  = WA_MinWidth;
        tagList[ tagNR++ ].ti_Data = 20;
        tagList[ tagNR   ].ti_Tag  = WA_MinHeight;
        tagList[ tagNR++ ].ti_Data = 20;
        tagList[ tagNR   ].ti_Tag  = WA_MaxWidth;
        tagList[ tagNR++ ].ti_Data = 9999;
        tagList[ tagNR   ].ti_Tag  = WA_MaxHeight;
        tagList[ tagNR++ ].ti_Data = 9999;
        tagList[ tagNR   ].ti_Tag  = WA_Title;
        tagList[ tagNR++ ].ti_Data = (ULONG) "GLFW Window";
        tagList[ tagNR   ].ti_Tag  = WA_ScreenTitle;
        tagList[ tagNR++ ].ti_Data = (ULONG) "GLFW Application";
    }
    tagList[ tagNR ].ti_Tag = TAG_DONE;

    // Open window
    _glfwWin.Window = OpenWindowTagList( NULL, tagList );
    if( !_glfwWin.Window )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Fullscreen/windowed post fixups
    if( _glfwWin.Fullscreen )
    {
        // Don't show screen title
        ShowTitle( _glfwWin.Screen, FALSE );

        // Remember window size
        _glfwWin.Width  = _glfwWin.Window->Width;
        _glfwWin.Height = _glfwWin.Window->Height;
    }
    else
    {
        // If we are not in fullscreen mode, get screen handle from window
        _glfwWin.Screen = _glfwWin.Window->WScreen;

        // Get ModeID for the current video mode
        _glfwWin.ModeID = GetVPModeID( &_glfwWin.Screen->ViewPort );

        // Remember window size
        _glfwWin.Width  = _glfwWin.Window->GZZWidth;
        _glfwWin.Height = _glfwWin.Window->GZZHeight;
    }

    // Put window on top
    WindowToFront( _glfwWin.Window );

    // Create OpenGL context
#ifdef _GLFW_STORMMESA
    tagNR = 0;
    tagList[ tagNR   ].ti_Tag  = AMA_Window;
    tagList[ tagNR++ ].ti_Data = (ULONG) _glfwWin.Window;
    tagList[ tagNR   ].ti_Tag  = AMA_RastPort;
    tagList[ tagNR++ ].ti_Data = (ULONG) _glfwWin.Window->RPort;
    tagList[ tagNR   ].ti_Tag  = AMA_Screen;
    tagList[ tagNR++ ].ti_Data = (ULONG) _glfwWin.Screen;
    tagList[ tagNR   ].ti_Tag  = AMA_Left;
    tagList[ tagNR++ ].ti_Data = 0;
    tagList[ tagNR   ].ti_Tag  = AMA_Bottom;
    tagList[ tagNR++ ].ti_Data = 0;
    tagList[ tagNR   ].ti_Tag  = AMA_Width;
    tagList[ tagNR++ ].ti_Data = _glfwWin.Width;
    tagList[ tagNR   ].ti_Tag  = AMA_Height;
    tagList[ tagNR++ ].ti_Data = _glfwWin.Height;
    tagList[ tagNR   ].ti_Tag  = AMA_DoubleBuf;
    tagList[ tagNR++ ].ti_Data = GL_TRUE;
    tagList[ tagNR   ].ti_Tag  = AMA_RGBMode;
    tagList[ tagNR++ ].ti_Data = GL_TRUE;
    tagList[ tagNR   ].ti_Tag  = AMA_AlphaFlag;
    tagList[ tagNR++ ].ti_Data = alphabits ? GL_TRUE : GL_FALSE;
    tagList[ tagNR   ].ti_Tag  = AMA_NoDepth;
    tagList[ tagNR++ ].ti_Data = depthbits ? GL_FALSE : GL_TRUE;
    tagList[ tagNR   ].ti_Tag  = AMA_NoStencil;
    tagList[ tagNR++ ].ti_Data = stencilbits ? GL_FALSE : GL_TRUE;
    tagList[ tagNR   ].ti_Tag  = AMA_NoAccum;
    tagList[ tagNR++ ].ti_Data = accumbits ? GL_FALSE : GL_TRUE;
    tagList[ tagNR   ].ti_Tag  = AMA_DirectRender;
    tagList[ tagNR++ ].ti_Data = GL_TRUE;
    tagList[ tagNR   ].ti_Tag  = AMA_DrawMode;
    tagList[ tagNR++ ].ti_Data = AMESA_AGA_C2P;
    tagList[ tagNR   ].ti_Tag  = TAG_DONE;
    _glfwWin.Context = AmigaMesaCreateContext( tagList );
#endif
    if( !_glfwWin.Context )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Make current
#ifdef _GLFW_STORMMESA
    AmigaMesaMakeCurrent( _glfwWin.Context, _glfwWin.Context->buffer );
#endif

    return GL_TRUE;
}


//========================================================================
// _glfwPlatformCloseWindow() - Properly kill the window/video display
//========================================================================

void _glfwPlatformCloseWindow( void )
{
    // Restore mouse pointer (if hidden)
    _glfwPlatformShowMouseCursor();

    // Destroy OpenGL context
    if( _glfwWin.Context )
    {
#ifdef _GLFW_STORMMESA
        AmigaMesaDestroyContext( _glfwWin.Context );
#endif
        _glfwWin.Context = NULL;
    }

    // Close window
    if( _glfwWin.Window )
    {
        CloseWindow( _glfwWin.Window );
        _glfwWin.Window = NULL;
    }

    // Close screen
    if( _glfwWin.Fullscreen && _glfwWin.Screen )
    {
        CloseScreen( _glfwWin.Screen );
    }
    _glfwWin.Screen = NULL;
}


//========================================================================
// _glfwPlatformSetWindowTitle() - Set the window title.
//========================================================================

void _glfwPlatformSetWindowTitle( const char *title )
{
    if( !_glfwWin.Fullscreen )
    {
        SetWindowTitles( _glfwWin.Window, (char*) title, (char*) title );
    }
}


//========================================================================
// _glfwPlatformSetWindowSize() - Set the window size.
//========================================================================

void _glfwPlatformSetWindowSize( int width, int height )
{
    if( !_glfwWin.Fullscreen )
    {
        SizeWindow( _glfwWin.Window, width-_glfwWin.Width,
                    height-_glfwWin.Height );
    }
}


//========================================================================
// _glfwPlatformSetWindowPos() - Set the window position.
//========================================================================

void _glfwPlatformSetWindowPos( int x, int y )
{
    if( !_glfwWin.Fullscreen )
    {
        ChangeWindowBox( _glfwWin.Window, x, y, _glfwWin.Window->Width,
                          _glfwWin.Window->Height );
    }
}


//========================================================================
// _glfwPlatformIconfyWindow() - Window iconification
//========================================================================

void _glfwPlatformIconifyWindow( void )
{
    if( _glfwWin.Fullscreen )
    {
        ScreenToBack( _glfwWin.Screen );
        WBenchToFront();
        _glfwWin.Iconified = GL_TRUE;
    }
}


//========================================================================
// _glfwPlatformRestoreWindow() - Window un-iconification
//========================================================================

void _glfwPlatformRestoreWindow( void )
{
    if( _glfwWin.Fullscreen )
    {
        ScreenToFront( _glfwWin.Screen );
    }
    WindowToFront( _glfwWin.Window );
    ActivateWindow( _glfwWin.Window );
    _glfwWin.Iconified = GL_FALSE;
}


//========================================================================
// _glfwPlatformSwapBuffers() - Swap buffers (double-buffering) and poll
// any new events.
//========================================================================

void _glfwPlatformSwapBuffers( void )
{
#ifdef _GLFW_STORMMESA
    AmigaMesaSwapBuffers( _glfwWin.Context );
#endif
}


//========================================================================
// _glfwPlatformSwapInterval() - Set double buffering swap interval
//========================================================================

void _glfwPlatformSwapInterval( int interval )
{
    // Not supported
}


//========================================================================
// _glfwPlatformRefreshWindowParams()
//========================================================================

void _glfwPlatformRefreshWindowParams( void )
{
    int r, g, b, refresh;

    // This function is not proerly implemented yet. Most of this
    // information should be gathered from the OpenGL/MESA implementation,
    // not the ModeID (except refresh rate)

    // Get ModeID information
    _glfwGetModeIDInfo( _glfwWin.ModeID, NULL, NULL, &r, &g, &b,
                        &refresh );

    // Fill out information
    _glfwWin.Accelerated    = GL_TRUE;
    _glfwWin.RedBits        = r;
    _glfwWin.GreenBits      = g;
    _glfwWin.BlueBits       = b;
    _glfwWin.AlphaBits      = 0;
    _glfwWin.DepthBits      = 16;
    _glfwWin.StencilBits    = 0;
    _glfwWin.AccumRedBits   = 0;
    _glfwWin.AccumGreenBits = 0;
    _glfwWin.AccumBlueBits  = 0;
    _glfwWin.AccumAlphaBits = 0;
    _glfwWin.AuxBuffers     = 0;
    _glfwWin.Stereo         = 0;
    _glfwWin.RefreshRate    = refresh;
}


//========================================================================
// _glfwPlatformPollEvents() - Poll for new window and input events
//========================================================================

void _glfwPlatformPollEvents( void )
{
    int winclosed;

    // Process all pending window events
    winclosed = GL_FALSE;
    if( _glfwProcessEvents() )
    {
        winclosed = GL_TRUE;
    }

    // Was there a window close request?
    if( winclosed )
    {
        glfwCloseWindow();
    }
}


/*
//========================================================================
// _glfwPlatformWaitEvents() - Wait for new window and input events
//========================================================================

void _glfwPlatformWaitEvents( void )
{
    // Wait for new window events
    Wait( 1L << _glfwWin.Window->UserPort->mp_SigBit );

    // Process events
    _glfwPlatformPollEvents();
}
*/


//========================================================================
// _glfwPlatformHideMouseCursor() - Hide mouse cursor (lock it)
//========================================================================

void _glfwPlatformHideMouseCursor( void )
{
    // We only allow this under fullscreen right now, since we can't rely
    // on the pointer position in windowed mode! Perhaps it's possible to
    // "steal" the mouse with input.device or something...?
    if( !_glfwWin.PointerHidden && _glfwWin.Fullscreen )
    {
        // Allocate chip memory for the blank mouse pointer
        _glfwWin.PointerSprite = AllocVec( 128, MEMF_CHIP | MEMF_CLEAR );
        if( _glfwWin.PointerSprite )
        {
            // Switch to blank/transparent pointer
            SetPointer( _glfwWin.Window, (UWORD *) _glfwWin.PointerSprite,
                        1, 1, 0, 0 );
            _glfwWin.PointerHidden = 1;

            // Switch to mouse delta movement
            Forbid();
            _glfwWin.Window->IDCMPFlags |= IDCMP_DELTAMOVE;
            Permit();
        }
    }
}


//========================================================================
// _glfwPlatformShowMouseCursor() - Show mouse cursor (unlock it)
//========================================================================

void _glfwPlatformShowMouseCursor( void )
{
    if( _glfwWin.PointerHidden )
    {
        // Switch to absolute mouse movement
        Forbid();
        _glfwWin.Window->IDCMPFlags &= (0xFFFFFFFF^IDCMP_DELTAMOVE);
        Permit();

        // Change back to normal pointer
        ClearPointer( _glfwWin.Window );
        if( _glfwWin.PointerSprite )
        {
            FreeVec( _glfwWin.PointerSprite );
            _glfwWin.PointerSprite = NULL;
        }
        _glfwWin.PointerHidden = 0;
    }
}


//========================================================================
// _glfwPlatformSetMouseCursorPos() - Set physical mouse cursor position
//========================================================================

void _glfwPlatformSetMouseCursorPos( int x, int y )
{
    // Not implemented yet (can it be done?!?!)
}
