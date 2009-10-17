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

//========================================================================
// Delegate for window related notifications
// (but also used as an application delegate)
//========================================================================

@interface GLFWWindowDelegate : NSObject
@end

@implementation GLFWWindowDelegate

- (BOOL)windowShouldClose:(id)window
{
    if( _glfwWin.WindowCloseCallback )
    {
        if( !_glfwWin.WindowCloseCallback() )
        {
            return NO;
        }
    }

    // This is horribly ugly, but it works
    glfwCloseWindow();
    return NO;
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

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    if( _glfwWin.WindowCloseCallback )
    {
        if( !_glfwWin.WindowCloseCallback() )
        {
            return NSTerminateCancel;
        }
    }

    // This is horribly ugly, but it works
    glfwCloseWindow();
    return NSTerminateCancel;
}

@end

// todo need to find mappings for F13-F15, volume down/up/mute, and eject.
static const unsigned int MAC_TO_GLFW_KEYCODE_MAPPING[128] =
{
    /* 00 */ 'A',
    /* 01 */ 'S',
    /* 02 */ 'D',
    /* 03 */ 'F',
    /* 04 */ 'H',
    /* 05 */ 'G',
    /* 06 */ 'Z',
    /* 07 */ 'X',
    /* 08 */ 'C',
    /* 09 */ 'V',
    /* 0a */ -1,
    /* 0b */ 'B',
    /* 0c */ 'Q',
    /* 0d */ 'W',
    /* 0e */ 'E',
    /* 0f */ 'R',
    /* 10 */ 'Y',
    /* 11 */ 'T',
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
    /* 1f */ 'O',
    /* 20 */ 'U',
    /* 21 */ '[',
    /* 22 */ 'I',
    /* 23 */ 'P',
    /* 24 */ GLFW_KEY_ENTER,
    /* 25 */ 'L',
    /* 26 */ 'J',
    /* 27 */ '\'',
    /* 28 */ 'K',
    /* 29 */ ';',
    /* 2a */ '\\',
    /* 2b */ ',',
    /* 2c */ '/',
    /* 2d */ 'N',
    /* 2e */ 'M',
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

//========================================================================
// Converts a Mac OS X keycode to a GLFW keycode
//========================================================================

static int _glfwFromMacKeyCode( unsigned int macKeyCode )
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

//========================================================================
// Content view class for the GLFW window
//========================================================================

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
    if( _glfwWin.mouseLock )
    {
        CGMouseDelta x, y;

        CGGetLastMouseDelta( &x, &y );

        _glfwInput.MousePosX += x;
        _glfwInput.MousePosY -= y;
    }
    else
    {
        NSPoint p = [event locationInWindow];

        // Cocoa coordinate system has origin at lower left
        _glfwInput.MousePosX = p.x;
        _glfwInput.MousePosY = [[_glfwWin.window contentView] bounds].size.height - p.y;
    }

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
    _glfwWin.pixelFormat = nil;
    _glfwWin.window = nil;
    _glfwWin.context = nil;
    _glfwWin.delegate = nil;

    _glfwWin.delegate = [[GLFWWindowDelegate alloc] init];
    if( _glfwWin.delegate == nil )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    [NSApp setDelegate:_glfwWin.delegate];

    // Mac OS X needs non-zero color size, so set resonable values
    if( redbits + greenbits + bluebits < 15 )
    {
        redbits = greenbits = bluebits = 8;
    }

    // Ignored hints:
    // OpenGLMajor, OpenGLMinor, OpenGLForward:
    //     pending Mac OS X support for OpenGL 3.x
    // OpenGLDebug
    //     pending it meaning anything on Mac OS X

    // Don't use accumulation buffer support; it's not accelerated
    // Aux buffers probably aren't accelerated either

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
        styleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;

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
    [_glfwWin.window setDelegate:_glfwWin.delegate];
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

    _glfwWin.pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    if( _glfwWin.pixelFormat == nil )
    {
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    _glfwWin.context = [[NSOpenGLContext alloc] initWithFormat:_glfwWin.pixelFormat
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
}

//========================================================================
// Properly kill the window / video display
//========================================================================

void _glfwPlatformCloseWindow( void )
{
    [_glfwWin.window orderOut:nil];

    if( _glfwWin.Fullscreen )
    {
        [[_glfwWin.window contentView] exitFullScreenModeWithOptions:nil];
        CGDisplaySwitchToMode( CGMainDisplayID(),
                               (CFDictionaryRef)_glfwLibrary.DesktopMode );
        CGReleaseAllDisplays();
    }

    [_glfwWin.pixelFormat release];
    _glfwWin.pixelFormat = nil;

    [NSOpenGLContext clearCurrentContext];
    [_glfwWin.context release];
    _glfwWin.context = nil;

    [_glfwWin.window setDelegate:nil];
    [NSApp setDelegate:nil];
    [_glfwWin.delegate release];
    _glfwWin.delegate = nil;

    [_glfwWin.window close];
    _glfwWin.window = nil;

    // TODO: Probably more cleanup
}

//========================================================================
// Set the window title
//========================================================================

void _glfwPlatformSetWindowTitle( const char *title )
{
    [_glfwWin.window setTitle:[NSString stringWithCString:title
                     encoding:NSISOLatin1StringEncoding]];
}

//========================================================================
// Set the window size
//========================================================================

void _glfwPlatformSetWindowSize( int width, int height )
{
    [_glfwWin.window setContentSize:NSMakeSize(width, height)];
}

//========================================================================
// Set the window position
//========================================================================

void _glfwPlatformSetWindowPos( int x, int y )
{
    NSRect contentRect = [_glfwWin.window contentRectForFrameRect:[_glfwWin.window frame]];

    // We assume here that the client code wants to position the window within the
    // screen the window currently occupies
    NSRect screenRect = [[_glfwWin.window screen] visibleFrame];
    contentRect.origin = NSMakePoint(screenRect.origin.x + x,
                                     screenRect.origin.y + screenRect.size.height -
                                         y - contentRect.size.height);

    [_glfwWin.window setFrame:[_glfwWin.window frameRectForContentRect:contentRect]
                      display:YES];
}

//========================================================================
// Iconify the window
//========================================================================

void _glfwPlatformIconifyWindow( void )
{
    [_glfwWin.window miniaturize:nil];
}

//========================================================================
// Restore (un-iconify) the window
//========================================================================

void _glfwPlatformRestoreWindow( void )
{
    [_glfwWin.window deminiaturize:nil];
}

//========================================================================
// Swap buffers
//========================================================================

void _glfwPlatformSwapBuffers( void )
{
    // ARP appears to be unnecessary, but this is future-proof
    [_glfwWin.context flushBuffer];
}

//========================================================================
// Set double buffering swap interval
//========================================================================

void _glfwPlatformSwapInterval( int interval )
{
    GLint sync = interval;
    [_glfwWin.context setValues:&sync forParameter:NSOpenGLCPSwapInterval];
}

//========================================================================
// Write back window parameters into GLFW window structure
//========================================================================

void _glfwPlatformRefreshWindowParams( void )
{
    GLint value;

    // Since GLFW 2.x doesn't understand screens, we use virtual screen zero

    [_glfwWin.pixelFormat getValues:&value
                       forAttribute:NSOpenGLPFAAccelerated
                   forVirtualScreen:0];
    _glfwWin.Accelerated = value;

    [_glfwWin.pixelFormat getValues:&value
                       forAttribute:NSOpenGLPFAAlphaSize
                   forVirtualScreen:0];
    _glfwWin.AlphaBits = value;

    // It seems that the color size includes the size of the alpha channel
    [_glfwWin.pixelFormat getValues:&value
                       forAttribute:NSOpenGLPFAColorSize
                   forVirtualScreen:0];
    value -= _glfwWin.AlphaBits;
    _glfwWin.RedBits = value / 3;
    _glfwWin.GreenBits = value / 3;
    _glfwWin.BlueBits = value / 3;

    [_glfwWin.pixelFormat getValues:&value
                       forAttribute:NSOpenGLPFADepthSize
                   forVirtualScreen:0];
    _glfwWin.DepthBits = value;

    [_glfwWin.pixelFormat getValues:&value
                       forAttribute:NSOpenGLPFAStencilSize
                   forVirtualScreen:0];
    _glfwWin.StencilBits = value;

    [_glfwWin.pixelFormat getValues:&value
                       forAttribute:NSOpenGLPFAAccumSize
                   forVirtualScreen:0];
    _glfwWin.AccumRedBits = value / 3;
    _glfwWin.AccumGreenBits = value / 3;
    _glfwWin.AccumBlueBits = value / 3;

    // TODO: Figure out what to set this value to
    _glfwWin.AccumAlphaBits = 0;

    [_glfwWin.pixelFormat getValues:&value
                       forAttribute:NSOpenGLPFAAuxBuffers
                   forVirtualScreen:0];
    _glfwWin.AuxBuffers = value;

    [_glfwWin.pixelFormat getValues:&value
                       forAttribute:NSOpenGLPFAStereo
                   forVirtualScreen:0];
    _glfwWin.Stereo = value;

    [_glfwWin.pixelFormat getValues:&value
                       forAttribute:NSOpenGLPFASamples
                   forVirtualScreen:0];
    _glfwWin.Samples = value;

    // These are forced to false as long as Mac OS X lacks support for OpenGL 3+
    _glfwWin.GLForward = GL_FALSE;
    _glfwWin.GLDebug = GL_FALSE;
}

//========================================================================
// Poll for new window and input events
//========================================================================

void _glfwPlatformPollEvents( void )
{
    NSEvent *event;

    do
    {
        event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                   untilDate:[NSDate distantPast]
                                      inMode:NSDefaultRunLoopMode
                                     dequeue:YES];

        if (event)
        {
            [NSApp sendEvent:event];
        }
    }
    while (event);

    [_glfwLibrary.AutoreleasePool drain];
    _glfwLibrary.AutoreleasePool = [[NSAutoreleasePool alloc] init];
}

//========================================================================
// Wait for new window and input events
//========================================================================

void _glfwPlatformWaitEvents( void )
{
    // I wanted to pass NO to dequeue:, and rely on PollEvents to
    // dequeue and send.  For reasons not at all clear to me, passing
    // NO to dequeue: causes this method never to return.
    NSEvent *event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                        untilDate:[NSDate distantFuture]
                                           inMode:NSDefaultRunLoopMode
                                          dequeue:YES];
    [NSApp sendEvent:event];

    _glfwPlatformPollEvents();
}

//========================================================================
// Hide mouse cursor (lock it)
//========================================================================

void _glfwPlatformHideMouseCursor( void )
{
    [NSCursor hide];
    CGAssociateMouseAndMouseCursorPosition( false );
}

//========================================================================
// Show mouse cursor (unlock it)
//========================================================================

void _glfwPlatformShowMouseCursor( void )
{
    [NSCursor unhide];
    CGAssociateMouseAndMouseCursorPosition( true );
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

