//========================================================================
// GLFW - An OpenGL framework
// File:     internal.h
// Platform: Any
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

#ifndef _internal_h_
#define _internal_h_

//========================================================================
// GLFWGLOBAL is a macro that places all global variables in the init.c
// module (all other modules reference global variables as 'extern')
//========================================================================

#if defined( _init_c_ )
#define GLFWGLOBAL
#else
#define GLFWGLOBAL extern
#endif


//========================================================================
// System independent include files
//========================================================================

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//------------------------------------------------------------------------
// Platform specific definitions goes in platform.h (which also includes
// glfw.h)
//------------------------------------------------------------------------

#include "platform.h"


//========================================================================
// System independent global variables (GLFW internals)
//========================================================================

// Flag indicating if GLFW has been initialized
#if defined( _init_c_ )
int _glfwInitialized = 0;
#else
extern int _glfwInitialized;
#endif


//------------------------------------------------------------------------
// Window hints (set by glfwOpenWindowHint)
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    int          RefreshRate;
    int          AccumRedBits;
    int          AccumGreenBits;
    int          AccumBlueBits;
    int          AccumAlphaBits;
    int          AuxBuffers;
    int          Stereo;
} _glfwWinHints;



//========================================================================
// Prototypes for platform specific implementation functions
//========================================================================

// Init/terminate
int _glfwPlatformInit( void );
int _glfwPlatformTerminate( void );

// Enable/Disable
void _glfwPlatformEnableSystemKeys( void );
void _glfwPlatformDisableSystemKeys( void );

// Fullscreen
int  _glfwPlatformGetVideoModes( GLFWvidmode *list, int maxcount );
void _glfwPlatformGetDesktopMode( GLFWvidmode *mode );

// OpenGL extensions
int    _glfwPlatformExtensionSupported( const char *extension );
void * _glfwPlatformGetProcAddress( const char *procname );

// Joystick
int _glfwPlatformGetJoystickParam( int joy, int param );
int _glfwPlatformGetJoystickPos( int joy, float *pos, int numaxes );
int _glfwPlatformGetJoystickButtons( int joy, unsigned char *buttons, int numbuttons );

// Threads
GLFWthread _glfwPlatformCreateThread( GLFWthreadfun fun, void *arg );
void       _glfwPlatformDestroyThread( GLFWthread ID );
int        _glfwPlatformWaitThread( GLFWthread ID, int waitmode );
GLFWthread _glfwPlatformGetThreadID( void );
GLFWmutex  _glfwPlatformCreateMutex( void );
void       _glfwPlatformDestroyMutex( GLFWmutex mutex );
void       _glfwPlatformLockMutex( GLFWmutex mutex );
void       _glfwPlatformUnlockMutex( GLFWmutex mutex );
GLFWcond   _glfwPlatformCreateCond( void );
void       _glfwPlatformDestroyCond( GLFWcond cond );
void       _glfwPlatformWaitCond( GLFWcond cond, GLFWmutex mutex, double timeout );
void       _glfwPlatformSignalCond( GLFWcond cond );
void       _glfwPlatformBroadcastCond( GLFWcond cond );
int        _glfwPlatformGetNumberOfProcessors( void );

// Time
double _glfwPlatformGetTime( void );
void   _glfwPlatformSetTime( double time );
void   _glfwPlatformSleep( double time );

// Window management
int  _glfwPlatformOpenWindow( int width, int height, int redbits, int greenbits, int bluebits, int alphabits, int depthbits, int stencilbits, int mode, int accumredbits, int accumgreenbits, int accumbluebits, int accumalphabits, int auxbuffers, int stereo, int refreshrate );
void _glfwPlatformCloseWindow( void );
void _glfwPlatformSetWindowTitle( const char *title );
void _glfwPlatformSetWindowSize( int width, int height );
void _glfwPlatformSetWindowPos( int x, int y );
void _glfwPlatformIconifyWindow( void );
void _glfwPlatformRestoreWindow( void );
void _glfwPlatformSwapBuffers( void );
void _glfwPlatformSwapInterval( int interval );
void _glfwPlatformRefreshWindowParams( void );
void _glfwPlatformPollEvents( void );
void _glfwPlatformHideMouseCursor( void );
void _glfwPlatformShowMouseCursor( void );
void _glfwPlatformSetMouseCursorPos( int x, int y );


//========================================================================
// Prototypes for platform independent internal functions
//========================================================================

// Input handling (window.c)
void _glfwClearInput( void );
void _glfwInputKey( int key, int action );
void _glfwInputChar( int character, int action );
void _glfwInputMouseClick( int button, int action );

// Threads (thread.c)
_GLFWthread * _glfwGetThreadPointer( int ID );
void _glfwAppendThread( _GLFWthread * t );
void _glfwRemoveThread( _GLFWthread * t );

// OpenGL extensions (glext.c)
int _glfwStringInExtensionString( const char *string, const GLubyte *extensions );

// Image/texture I/O support (tga.c)
int _glfwReadTGA( FILE *f, GLFWimage *img, int flags );


#endif // _internal_h_
