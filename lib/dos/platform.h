//========================================================================
// GLFW - An OpenGL framework
// File:        platform.h
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
// $Id: platform.h,v 1.1 2003-11-26 20:45:03 marcus256 Exp $
//========================================================================

#ifndef _platform_h_
#define _platform_h_


// This is the DOS version of GLFW
#define _GLFW_DOS


// Include files


// GLFW+GL+GLU defines
#include "../../include/GL/glfw.h"


// Stack size for each thread (in bytes)
#define _GLFW_TASK_STACK_SIZE 50000


//========================================================================
// Global variables (GLFW internals)
//========================================================================


//------------------------------------------------------------------------
// Window structure
//------------------------------------------------------------------------
typedef struct _GLFWwin_struct _GLFWwin;

struct _GLFWwin_struct {

// ========= PLATFORM INDEPENDENT MANDATORY PART =========================

    // User callback functions
    GLFWwindowsizefun  WindowSizeCallback;
    GLFWmousebuttonfun MouseButtonCallback;
    GLFWmouseposfun    MousePosCallback;
    GLFWmousewheelfun  MouseWheelCallback;
    GLFWkeyfun         KeyCallback;
    GLFWcharfun        CharCallback;

    // User selected window settings
    int       Fullscreen;      // Fullscreen flag
    int       MouseLock;       // Mouse-lock flag
    int       AutoPollEvents;  // Auto polling flag
    int       SysKeysDisabled; // System keys disabled flag

    // Window status & parameters
    int       Opened;          // Flag telling if window is opened or not
    int       Active;          // Application active flag
    int       Iconified;       // Window iconified flag
    int       Width, Height;   // Window width and heigth
    int       Accelerated;     // GL_TRUE if window is HW accelerated
    int       RedBits;
    int       GreenBits;
    int       BlueBits;
    int       AlphaBits;
    int       DepthBits;
    int       StencilBits;
    int       AccumRedBits;
    int       AccumGreenBits;
    int       AccumBlueBits;
    int       AccumAlphaBits;
    int       AuxBuffers;
    int       Stereo;
    int       RefreshRate;     // Vertical monitor refresh rate

    // Extensions
    int       Has_GL_SGIS_generate_mipmap;


// ========= PLATFORM SPECIFIC PART ======================================

    // Platform specific window resources

    // Platform specific extensions

    // Various platform specific internal variables

};

GLFWGLOBAL _GLFWwin _glfwWin;


//------------------------------------------------------------------------
// User input status (most of this should go in _GLFWwin)
//------------------------------------------------------------------------
GLFWGLOBAL struct {

// ========= PLATFORM INDEPENDENT MANDATORY PART =========================

    // Mouse status
    int  MousePosX, MousePosY;
    int  WheelPos;
    char MouseButton[ GLFW_MOUSE_BUTTON_LAST+1 ];

    // Keyboard status
    char Key[ GLFW_KEY_LAST+1 ];
    int  LastChar;

    // User selected settings
    int  StickyKeys;
    int  StickyMouseButtons;
    int  KeyRepeat;


// ========= PLATFORM SPECIFIC PART ======================================

    // Platform specific internal variables

} _glfwInput;


//------------------------------------------------------------------------
// Timer status
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    double             Period;
    unsigned long long t0;
} _glfwTimer;


//------------------------------------------------------------------------
// Thread record (one for each thread)
//------------------------------------------------------------------------
typedef struct _GLFWthread_struct _GLFWthread;

struct _GLFWthread_struct {

// ========= PLATFORM INDEPENDENT MANDATORY PART =========================

    // Pointer to previous and next threads in linked list
    _GLFWthread    *Previous, *Next;

    // GLFW user side thread information
    GLFWthread     ID;

// ========= PLATFORM SPECIFIC PART ======================================

    // System side thread information
    GLFWthreadfun  Function;
    void           *Arg;
};


//------------------------------------------------------------------------
// General thread information
//------------------------------------------------------------------------
GLFWGLOBAL struct {

// ========= PLATFORM INDEPENDENT MANDATORY PART =========================

    // Next thread ID to use (increments for every created thread)
    GLFWthread       NextID;

    // First thread in linked list (always the main thread)
    _GLFWthread      First;

// ========= PLATFORM SPECIFIC PART ======================================

    // Critical section lock

} _glfwThrd;


//------------------------------------------------------------------------
// Joystick information & state
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    int dummy;
} _glfwJoy;


//========================================================================
// Macros for encapsulating critical code sections (i.e. making parts
// of GLFW thread safe)
//========================================================================

// Thread list management
#define ENTER_THREAD_CRITICAL_SECTION \
        ;
#define LEAVE_THREAD_CRITICAL_SECTION \
        ;


//========================================================================
// Prototypes for platform specific internal functions
//========================================================================

// Time
int  _glfwInitTimer( void );
void _glfwTerminateTimer( void );

// Fullscreen

// Joystick
void _glfwInitJoysticks( void );
void _glfwTerminateJoysticks( void );

// Threads
int _glfwInitThreads( void );
void _glfwTerminateThreads( void );

#endif // _platform_h_
