//========================================================================
// GLFW - An OpenGL framework
// File:        macosx_window.m
// Platform:    Mac OS X
// API Version: 2.7
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

@interface GLFWWindowDelegate : NSObject
@end

@implementation GLFWWindowDelegate

- (BOOL)windowShouldClose:(id)window
{
    if( _glfwWin.WindowCloseCallback )
    {
        if( !_glfwWin.WindowCloseCallback() )
            return NO;
    }

    return YES;
}

- (void)windowWillClose:(NSNotification *)notification
{
    glfwCloseWindow();
}

- (void)windowDidResize:(NSNotification *)notification
{
    [_glfwWin.context update];
    
    if( _glfwWin.WindowSizeCallback )
    {
        NSRect contentRect =
            [_glfwWin.window contentRectForFrameRect:[_glfwWin.window frame]];
        _glfwWin.WindowSizeCallback( contentRect.size.width,
                                     contentRect.size.height );
    }
}

@end

// todo need to find mappings for F13-F15, volume down/up/mute, and eject.
static const unsigned int MAC_TO_GLFW_KEYCODE_MAPPING[128] =
{
    /* 00 */ 'a',
    /* 01 */ 's',
    /* 02 */ 'd',
    /* 03 */ 'f',
    /* 04 */ 'h',
    /* 05 */ 'g',
    /* 06 */ 'z',
    /* 07 */ 'x',
    /* 08 */ 'c',
    /* 09 */ 'v',
    /* 0a */ -1,
    /* 0b */ 'b',
    /* 0c */ 'q',
    /* 0d */ 'w',
    /* 0e */ 'e',
    /* 0f */ 'r',
    /* 10 */ 'y',
    /* 11 */ 't',
    /* 12 */ '1',
    /* 13 */ '2',
    /* 14 */ '3',
    /* 15 */ '4',
    /* 16 */ '6',
    /* 17 */ '5',
    /* 18 */ GLFW_KEY_KP_EQUAL,
    /* 19 */ '9',
    /* 1a */ '7',
    /* 1b */ '-',
    /* 1c */ '8',
    /* 1d */ '0',
    /* 1e */ ']',
    /* 1f */ 'o',
    /* 20 */ 'u',
    /* 21 */ '[',
    /* 22 */ 'i',
    /* 23 */ 'p',
    /* 24 */ GLFW_KEY_ENTER,
    /* 25 */ 'l',
    /* 26 */ 'j',
    /* 27 */ '\'',
    /* 28 */ 'k',
    /* 29 */ ';',
    /* 2a */ '\\',
    /* 2b */ ',',
    /* 2c */ '/',
    /* 2d */ 'n',
    /* 2e */ 'm',
    /* 2f */ '.',
    /* 30 */ GLFW_KEY_TAB,
    /* 31 */ GLFW_KEY_SPACE,
    /* 32 */ '`',
    /* 33 */ GLFW_KEY_BACKSPACE,
    /* 34 */ -1,
    /* 35 */ GLFW_KEY_ESC,
    /* 36 */ -1, // GLFW_KEY_RCOMMAND
    /* 37 */ -1, // GLFW_KEY_LCOMMAND
    /* 38 */ GLFW_KEY_LSHIFT,
    /* 39 */ -1,
    /* 3a */ GLFW_KEY_LALT,
    /* 3b */ GLFW_KEY_LCTRL,
    /* 3c */ GLFW_KEY_RSHIFT,
    /* 3d */ GLFW_KEY_RALT,
    /* 3e */ GLFW_KEY_RCTRL,
    /* 3f */ -1,
    /* 40 */ -1,
    /* 41 */ GLFW_KEY_KP_DECIMAL,
    /* 42 */ -1,
    /* 43 */ GLFW_KEY_KP_MULTIPLY,
    /* 44 */ -1,
    /* 45 */ GLFW_KEY_KP_ADD,
    /* 46 */ -1,
    /* 47 */ -1,
    /* 48 */ -1,
    /* 49 */ -1,
    /* 4a */ -1,
    /* 4b */ GLFW_KEY_KP_DIVIDE,
    /* 4c */ GLFW_KEY_KP_ENTER,
    /* 4d */ -1,
    /* 4e */ GLFW_KEY_KP_SUBTRACT,
    /* 4f */ -1,
    /* 50 */ -1,
    /* 51 */ -1,
    /* 52 */ GLFW_KEY_KP_0,
    /* 53 */ GLFW_KEY_KP_1,
    /* 54 */ GLFW_KEY_KP_2,
    /* 55 */ GLFW_KEY_KP_3,
    /* 56 */ GLFW_KEY_KP_4,
    /* 57 */ GLFW_KEY_KP_5,
    /* 58 */ GLFW_KEY_KP_6,
    /* 59 */ GLFW_KEY_KP_7,
    /* 5a */ -1,
    /* 5b */ GLFW_KEY_KP_8,
    /* 5c */ GLFW_KEY_KP_9,
    /* 5d */ -1,
    /* 5e */ -1,
    /* 5f */ -1,
    /* 60 */ GLFW_KEY_F5,
    /* 61 */ GLFW_KEY_F6,
    /* 62 */ GLFW_KEY_F7,
    /* 63 */ GLFW_KEY_F3,
    /* 64 */ GLFW_KEY_F8,
    /* 65 */ -1,
    /* 66 */ -1,
    /* 67 */ -1,
    /* 68 */ -1,
    /* 69 */ GLFW_KEY_F13,
    /* 6a */ GLFW_KEY_F16,
    /* 6b */ -1,
    /* 6c */ -1,
    /* 6d */ -1,
    /* 6e */ -1,
    /* 6f */ -1,
    /* 70 */ -1,
    /* 71 */ -1,
    /* 72 */ GLFW_KEY_INSERT,
    /* 73 */ GLFW_KEY_HOME,
    /* 74 */ GLFW_KEY_PAGEUP,
    /* 75 */ GLFW_KEY_DEL,
    /* 76 */ GLFW_KEY_F4,
    /* 77 */ GLFW_KEY_END,
    /* 78 */ GLFW_KEY_F2,
    /* 79 */ GLFW_KEY_PAGEDOWN,
    /* 7a */ GLFW_KEY_F1,
    /* 7b */ GLFW_KEY_LEFT,
    /* 7c */ GLFW_KEY_RIGHT,
    /* 7d */ GLFW_KEY_DOWN,
    /* 7e */ GLFW_KEY_UP,
    /* 7f */ -1,
};

int _glfwFromMacKeyCode( unsigned int macKeyCode )
{
    if( macKeyCode >= 128 )
    {
        return -1;
    }
    
    // This treats keycodes as *positional*; that is, we'll return 'a'
    // for the key left of 's', even on an AZERTY keyboard.  The charInput
    // function should still get 'q' though.
    return MAC_TO_GLFW_KEYCODE_MAPPING[macKeyCode];
}

@interface GLFWContentView : NSView
@end

@implementation GLFWContentView

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)mouseDown:(NSEvent *)event
{
    _glfwInputMouseClick( GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS );
}

- (void)mouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void)mouseUp:(NSEvent *)event
{
    _glfwInputMouseClick( GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE );
}

- (void)mouseMoved:(NSEvent *)event
{
    NSPoint p = [event locationInWindow];

    // Cocoa coordinate system has origin at lower left
    _glfwInput.MousePosX = p.x;
    _glfwInput.MousePosY = [[_glfwWin.window contentView] bounds].size.height - p.y;

    if( _glfwWin.MousePosCallback )
    {
        _glfwWin.MousePosCallback( _glfwInput.MousePosX, _glfwInput.MousePosY );
    }
}

- (void)rightMouseDown:(NSEvent *)event
{
    _glfwInputMouseClick( GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS );
}

- (void)rightMouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void)rightMouseUp:(NSEvent *)event
{
    _glfwInputMouseClick( GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE );
}

- (void)otherMouseDown:(NSEvent *)event
{
    _glfwInputMouseClick( [event buttonNumber], GLFW_PRESS );
}

- (void)otherMouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void)otherMouseUp:(NSEvent *)event
{
    _glfwInputMouseClick( [event buttonNumber], GLFW_RELEASE );
}

- (void)keyDown:(NSEvent *)event
{
    int code = _glfwFromMacKeyCode( [event keyCode] );

    if( code != -1 )
    {
        _glfwInputKey( code, GLFW_PRESS );
        
        if( [event modifierFlags] & NSCommandKeyMask )
        {
            if( !_glfwWin.SysKeysDisabled )
            {
                [super keyDown:event];
            }
        }
        else
        {
            // This eventually calls through to -insertText:
            [self interpretKeyEvents:[NSArray arrayWithObject:event]];
        }
    }
}

- (void)flagsChanged:(NSEvent *)event
{
    unsigned int newModifierFlags = [event modifierFlags];
    int mode;

    if( newModifierFlags > _glfwWin.modifierFlags )
    {
        mode = GLFW_PRESS;
    }
    else
    {
        mode = GLFW_RELEASE;
    }

    _glfwWin.modifierFlags = newModifierFlags;
    _glfwInputKey( MAC_TO_GLFW_KEYCODE_MAPPING[[event keyCode]], mode );
}

- (void)keyUp:(NSEvent *)event
{
    int code = _glfwFromMacKeyCode( [event keyCode] );
    if( code != -1 )
    {
        _glfwInputKey( code, GLFW_RELEASE );
    }
}

- (void)insertText:(id)string
{
    unsigned int i, n = [string length];

    for( i = 0; i < n; i++ )
    {
        _glfwInputChar( [string characterAtIndex:i], GLFW_PRESS );

        // TODO: Immediately release it too?  What does the mode even mean?
    }
}

- (void)scrollWheel:(NSEvent *)event
{
    _glfwInput.WheelPosFloating += [event deltaY];
    _glfwInput.WheelPos = lrint(_glfwInput.WheelPosFloating);

    if( _glfwWin.MouseWheelCallback )
    {
        _glfwWin.MouseWheelCallback( _glfwInput.WheelPos );
    }
}

@end

//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// Here is where the window is created, and the OpenGL rendering context is
// created
//========================================================================

int _glfwPlatformOpenWindow( int width, int height,
                             int redbits, int greenbits, int bluebits,
                             int alphabits, int depthbits, int stencilbits,
                             int mode, _GLFWhints* hints )
{
    // Apparently 0 means "don't care" to GLFW, but Mac OS X sure cares...
    if( redbits + greenbits + bluebits < 15 )
    {
        redbits = greenbits = bluebits = 8;
    }
    
    // Ignored hints:
    // OpenGLMajor, OpenGLMinor, OpenGLForward:
    //     pending Mac OS X support for OpenGL 3.x
    // OpenGLDebug
    //     pending it meaning anything on Mac OS X
    
    // Don't use accumulation buffer support; it ain't accelerated
    // I've put in support for aux buffers but I don't think they're accelerated either...
    
    GLFW_IN_ARP({
        CFDictionaryRef fullscreenMode = NULL;
        if( mode == GLFW_FULLSCREEN )
        {
            fullscreenMode =
                // I think it's safe to pass 0 to the refresh rate for this function
                // rather than conditionalizing the code to call the version which
                // doesn't specify refresh...
                CGDisplayBestModeForParametersAndRefreshRateWithProperty(
                    CGMainDisplayID(),
                    redbits + greenbits + bluebits + alphabits,
                    width,
                    height,
                    hints->RefreshRate,
                    // Controversial, see macosx_fullscreen.m for discussion
                    kCGDisplayModeIsSafeForHardware,
                    NULL);
            width = [[(id)fullscreenMode objectForKey:(id)kCGDisplayWidth] intValue];
            height = [[(id)fullscreenMode objectForKey:(id)kCGDisplayHeight] intValue];
        }
        
        unsigned int styleMask = 0;
        if( mode == GLFW_WINDOW )
        {
            styleMask = NSTitledWindowMask
                      | NSClosableWindowMask
                      | NSMiniaturizableWindowMask;
            if( !hints->WindowNoResize )
            {
                styleMask |= NSResizableWindowMask;
            }
        }
        else
        {
            styleMask = NSBorderlessWindowMask;
        }
        
        _glfwWin.window = [[NSWindow alloc]
            initWithContentRect:NSMakeRect(0, 0, width, height)
                      styleMask:styleMask
                        backing:NSBackingStoreBuffered
                          defer:NO];
        [_glfwWin.window setContentView:[[GLFWContentView alloc] init]];
        [_glfwWin.window setDelegate:[[GLFWWindowDelegate alloc] init]];
        [_glfwWin.window setAcceptsMouseMovedEvents:YES];
        [_glfwWin.window center];
        
        if( mode == GLFW_FULLSCREEN )
        {
            CGCaptureAllDisplays();
            CGDisplaySwitchToMode( CGMainDisplayID(), fullscreenMode );
        }
        
        unsigned int attribute_count = 0;
#define ADD_ATTR(x) attributes[attribute_count++] = x
#define ADD_ATTR2(x, y) (void)({ ADD_ATTR(x); ADD_ATTR(y); })
#define MAX_ATTRS 24 // urgh
        NSOpenGLPixelFormatAttribute attributes[MAX_ATTRS];
        
        ADD_ATTR( NSOpenGLPFADoubleBuffer );
        if( mode == GLFW_FULLSCREEN )
        {
            ADD_ATTR( NSOpenGLPFAFullScreen );
            ADD_ATTR( NSOpenGLPFANoRecovery );
            ADD_ATTR2( NSOpenGLPFAScreenMask,
                       CGDisplayIDToOpenGLDisplayMask( CGMainDisplayID() ) );
        }
        ADD_ATTR2( NSOpenGLPFAColorSize, redbits + greenbits + bluebits );

        if( alphabits > 0)
        {
            ADD_ATTR2( NSOpenGLPFAAlphaSize, alphabits );
        }

        if( depthbits > 0)
        {
            ADD_ATTR2( NSOpenGLPFADepthSize, depthbits );
        }

        if( stencilbits > 0)
        {
            ADD_ATTR2( NSOpenGLPFAStencilSize, stencilbits );
        }

        int accumbits = hints->AccumRedBits + hints->AccumGreenBits +
                        hints->AccumBlueBits + hints->AccumAlphaBits;

        if( accumbits > 0)
        {
            ADD_ATTR2( NSOpenGLPFAAccumSize, accumbits );
        }

        if( hints->AuxBuffers > 0)
        {
            ADD_ATTR2( NSOpenGLPFAAuxBuffers, hints->AuxBuffers );
        }

        if( hints->Stereo)
        {
            ADD_ATTR( NSOpenGLPFAStereo );
        }

        if( hints->Samples > 0)
        {
            ADD_ATTR2( NSOpenGLPFASampleBuffers, 1 );
            ADD_ATTR2( NSOpenGLPFASamples, hints->Samples );
        }

        ADD_ATTR(0);
        
        _glfwWin.pixelFormat = [[NSOpenGLPixelFormat alloc]
            initWithAttributes:attributes];
        if( _glfwWin.pixelFormat == nil )
        {
            _glfwPlatformCloseWindow();
            return GL_FALSE;
        }

        _glfwWin.context = [[NSOpenGLContext alloc]
            initWithFormat:_glfwWin.pixelFormat
              shareContext:nil];
        if( _glfwWin.context == nil )
        {
            _glfwPlatformCloseWindow();
            return GL_FALSE;
        }
        
        [_glfwWin.window makeKeyAndOrderFront:nil];
        [_glfwWin.context setView:[_glfwWin.window contentView]];
        
        if( mode == GLFW_FULLSCREEN )
        {
            // TODO: Make this work on pre-Leopard systems
            [[_glfwWin.window contentView] enterFullScreenMode:[NSScreen mainScreen]
                                                   withOptions:nil];
        }
        
        [_glfwWin.context makeCurrentContext];

        return GL_TRUE;
    })
    
    DODGE_FINALLY_BUG
}

//========================================================================
// Properly kill the window / video display
//========================================================================

void _glfwPlatformCloseWindow( void )
{
    GLFW_IN_ARP({
        [_glfwWin.window orderOut:nil];
        
        if( _glfwWin.Fullscreen )
        {
            [[_glfwWin.window contentView] exitFullScreenModeWithOptions:nil];
            CGDisplaySwitchToMode( CGMainDisplayID(),
                                   (CFDictionaryRef)_glfwLibrary.DesktopMode );
            CGReleaseAllDisplays();
        }

        // TODO: Probably more cleanup
    })
}

//========================================================================
// Set the window title
//========================================================================

void _glfwPlatformSetWindowTitle( const char *title )
{
    GLFW_IN_ARP({
        [_glfwWin.window setTitle:[NSString stringWithCString:title encoding:NSISOLatin1StringEncoding]];
    })
}

//========================================================================
// Set the window size
//========================================================================

void _glfwPlatformSetWindowSize( int width, int height )
{
    GLFW_IN_ARP({
        NSSize size = { width, height };
        [_glfwWin.window setContentSize:size];
    })
}

//========================================================================
// Set the window position
//========================================================================

void _glfwPlatformSetWindowPos( int x, int y )
{
    GLFW_IN_ARP({
        NSRect contentRect = [_glfwWin.window contentRectForFrameRect:[_glfwWin.window frame]];

        // We here assume that the client code wants to position the window within the
        // screen the window currently occupies
        NSRect screenRect = [[_glfwWin.window screen] visibleFrame];
        contentRect.origin = NSMakePoint(screenRect.origin.x + x,
                                         screenRect.origin.y + screenRect.size.height - y - contentRect.size.height);

        [_glfwWin.window setFrame:[_glfwWin.window frameRectForContentRect:contentRect] display:YES];
    })
}

//========================================================================
// Iconify the window
//========================================================================

void _glfwPlatformIconifyWindow( void )
{
    GLFW_IN_ARP({
        [_glfwWin.window miniaturize:nil];
    })
}

//========================================================================
// Restore (un-iconify) the window
//========================================================================

void _glfwPlatformRestoreWindow( void )
{
    GLFW_IN_ARP({
        [_glfwWin.window deminiaturize:nil];
    })
}

//========================================================================
// Swap buffers
//========================================================================

void _glfwPlatformSwapBuffers( void )
{
    // ARP appears to be unnecessary, but this is future-proof
    GLFW_IN_ARP({
        [_glfwWin.context flushBuffer];
    })
}

//========================================================================
// Set double buffering swap interval
//========================================================================

void _glfwPlatformSwapInterval( int interval )
{
    GLFW_IN_ARP({
        GLint sync = interval;
        [_glfwWin.context setValues:&sync forParameter:NSOpenGLCPSwapInterval];
    })
}

//========================================================================
// Write back window parameters into GLFW window structure
//========================================================================

void _glfwPlatformRefreshWindowParams( void )
{
    // TODO: Implement this.
}

//========================================================================
// Poll for new window and input events
//========================================================================

void _glfwPlatformPollEvents( void )
{
    NSEvent *event = nil;

    do
    {
        GLFW_IN_ARP({
            NSEvent *event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                                untilDate:[NSDate distantPast]
                                                   inMode:NSDefaultRunLoopMode
                                                  dequeue:YES];
            if (event)
            {
                [NSApp sendEvent:event];
            }
        })
    }
    while (event);
}

//========================================================================
// Wait for new window and input events
//========================================================================

void _glfwPlatformWaitEvents( void )
{
    GLFW_IN_ARP({
        // I wanted to pass NO to dequeue:, and rely on PollEvents to
        // dequeue and send.  For reasons not at all clear to me, passing
        // NO to dequeue: causes this method never to return.
        NSEvent *event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                            untilDate:[NSDate distantFuture]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        [NSApp sendEvent:event];
    })
    _glfwPlatformPollEvents();
}

//========================================================================
// Hide mouse cursor (lock it)
//========================================================================

void _glfwPlatformHideMouseCursor( void )
{
    GLFW_IN_ARP({
        [NSCursor hide];
    });
}

//========================================================================
// Show mouse cursor (unlock it)
//========================================================================

void _glfwPlatformShowMouseCursor( void )
{
    GLFW_IN_ARP({
        [NSCursor unhide];
    });
}

//========================================================================
// Set physical mouse cursor position
//========================================================================

void _glfwPlatformSetMouseCursorPos( int x, int y )
{
    // The library seems to assume that after calling this the mouse won't move,
    // but obviously it will, and escape the app's window, and activate other apps,
    // and other badness in pain.  I think the API's just silly, but maybe I'm
    // misunderstanding it...
    
    // Also, (x, y) are window coords...
    
    // Also, it doesn't seem possible to write this robustly without
    // calculating the maximum y coordinate of all screens, since Cocoa's
    // "global coordinates" are upside down from CG's...
    
    // Without this (once per app run, but it's convenient to do it here)
    // events will be suppressed for a default of 0.25 seconds after we
    // move the cursor.
    CGSetLocalEventsSuppressionInterval( 0.0 );
    
    NSPoint localPoint = NSMakePoint( x, y );
    NSPoint globalPoint = [_glfwWin.window convertBaseToScreen:localPoint];
    CGPoint mainScreenOrigin = CGDisplayBounds( CGMainDisplayID() ).origin;
    double mainScreenHeight = CGDisplayBounds( CGMainDisplayID() ).size.height;
    CGPoint targetPoint = CGPointMake( globalPoint.x - mainScreenOrigin.x,
                                       mainScreenHeight - globalPoint.y - mainScreenOrigin.y );
    CGDisplayMoveCursorToPoint( CGMainDisplayID(), targetPoint );
}

