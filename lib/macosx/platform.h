//========================================================================
// GLFW - An OpenGL framework
// File:     platform.h
// Platform: Mac OS X
// Version:  2.4
// Date:     2002.12.13
// Author:   Marcus Geelnard (marcus.geelnard@home.se)
// WWW:      http://hem.passagen.se/opengl/glfw/
//------------------------------------------------------------------------
// Copyright (c) 2002 Marcus Geelnard
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

#ifndef _platform_h_
#define _platform_h_


// This is the Mac OS X version of GLFW
#define _GLFW_MACOSX


// Include files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include "../../include/GL/glfw.h"

// Hack to allow inclusion of this file in both C and ObjC Source
#if defined(__OBJC__)
#include <Cocoa/Cocoa.h>
#else
typedef void* CFBundleRef;
typedef void* NSOpenGLContext;
typedef void* NSWindow;
typedef void* NSDictionary;
typedef void* NSAutoreleasePool;
#endif


//========================================================================
// Defines
//========================================================================

#define _GLFW_WINDOW_STYLE (NSTitledWindowMask |         \
                            NSMiniaturizableWindowMask | \
                            NSResizableWindowMask |      \
                            NSClosableWindowMask )


//========================================================================
// Global variables (GLFW internals)
//========================================================================

#if defined( _init_c_ )
#define GLFWGLOBAL
#else
#define GLFWGLOBAL extern
#endif


// Flag indicating if glfw has been initialised
#if defined( _init_c_ )
GLFWGLOBAL int _glfwInitialized = GL_FALSE;
#else
GLFWGLOBAL int _glfwInitialized;
#endif

// Autorelease pool
GLFWGLOBAL NSAutoreleasePool *_glfwPool;


//------------------------------------------------------------------------
// Window structure
//------------------------------------------------------------------------
typedef struct _GLFWwin_struct _GLFWwin;

struct _GLFWwin_struct {
    // Window states
    int         Opened;          // True if window is opened
    int         Iconified;       // True if window is iconified
    int         Active;          // True if window is active (in focus)

    // User callback functions
    GLFWwindowsizefun  WindowSizeCallback;
    GLFWmousebuttonfun MouseButtonCallback;
    GLFWmouseposfun    MousePosCallback;
    GLFWmousewheelfun  MouseWheelCallback;
    GLFWkeyfun         KeyCallback;

    // User selected window settings
    int         Fullscreen;      // Fullscreen flag
    int         MouseLock;       // Mouse-lock flag

    // Window resources
    NSOpenGLContext *Context;
    NSWindow        *Window;
    NSDictionary    *DesktopMode;

    // Window status
    int         Width, Height;   // Window width and heigth

    // Extensions
    int         Has_GL_SGIS_generate_mipmap;

    // Window Parameters
    int         Accelerated;
    int         ColorBits;
    int         AlphaBits;
    int         DepthBits;
    int         StencilBits;

    // Various internal variables
    // ...
};

GLFWGLOBAL _GLFWwin _glfwWin;


//------------------------------------------------------------------------
// User input status (some of this should go in _GLFWwin)
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    // Mouse status
    int  MousePosX, MousePosY;
    int  WheelPos;
    char MouseButton[ GLFW_MOUSE_BUTTON_LAST+1 ];

    // Keyboard status
    char Key[ GLFW_KEY_LAST+1 ];

    // User selected settings
    int  StickyKeys;
    int  StickyMouseButtons;
    int  KeyRepeat;

    // Internal variables
    // ...
} _glfwInput;


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


//------------------------------------------------------------------------
// Timer status
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    double       Resolution;
    long long    t0;
} _glfwTimer;


//------------------------------------------------------------------------
// Thread information
//------------------------------------------------------------------------
typedef struct _GLFWthread_struct _GLFWthread;

// Thread record (one for each thread)
struct _GLFWthread_struct {
    // GLFW user side thread information
    GLFWthread    ID;
    GLFWthreadfun Function;

    // System side thread information
    pthread_t     PosixID;

    // Pointer to previous and next threads in linked list
    _GLFWthread   *Previous, *Next;
};

// General thread information
GLFWGLOBAL struct {
    // Critical section lock
    pthread_mutex_t  CriticalSection;

    // Next thread ID to use (increments for every created thread)
    GLFWthread       NextID;

    // First thread in linked list (always the main thread)
    _GLFWthread      First;
} _glfwThrd;


//------------------------------------------------------------------------
// Mac OS X bundles
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    CFBundleRef OpenGL;  // Handle for OpenGL
} _glfwBundles;


//========================================================================
// Macros for encapsulating critical code sections (i.e. making parts
// of GLFW thread safe)
//========================================================================

// Thread list management
#define ENTER_THREAD_CRITICAL_SECTION \
        pthread_mutex_lock( &_glfwThrd.CriticalSection );
#define LEAVE_THREAD_CRITICAL_SECTION \
        pthread_mutex_unlock( &_glfwThrd.CriticalSection );


//========================================================================
// Prototypes for GLFW internal functions
//========================================================================

// Time
void _glfwInitTimer( void );

// Fullscreen support
// ...

// Image/texture I/O support
int _glfwReadTGA( FILE *f, GLFWimage *img, int flags );


#endif // _platform_h_
