//========================================================================
// GLFW - An OpenGL framework
// File:     platform.h
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

#ifndef _platform_h_
#define _platform_h_

// First of all: a file that hides compiler specific stuff
#include "SDI_compiler.h"


// This is the AmigaOS version of GLFW
#define _GLFW_AMIGAOS

// Are we compiling for MorphOS?
#if defined(__MORPHOS__) || defined(MORPHOS)
  #define _GLFW_MORPHOS
#endif

// Mesa/OpenGL flavour (we only support StormMesa at the moment)
#if !defined(_GLFW_STORMMESA)
  #define _GLFW_STORMMESA
#endif

// If this is the file where we keep the library bases, don't let the
// AmigaOS include files declare them as external
#if defined( _init_c_ )
  #define __NOLIBBASE__
#endif


// Include files
#include <exec/exec.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/dosextens.h>
#include <intuition/intuition.h>
#include <graphics/displayinfo.h>
#include <graphics/rastport.h>
#include <devices/timer.h>
#include <devices/keymap.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/timer.h>
#include <proto/keymap.h>
#include <proto/utility.h>

#include <clib/alib_protos.h>


// Amiga Mesa/OpenGL implementation dependent include
#ifdef _GLFW_STORMMESA
  #include <GL/amigamesa.h>
#endif

// MorphOS support
#ifdef _GLFW_MORPHOS
  #include <emul/emulinterface.h>
#endif

// GLFW+GL+GLU defines
#include "//include/GL/glfw.h"


// Stack size for each thread (in bytes)
#define _GLFW_TASK_STACK_SIZE 50000


//========================================================================
// Global variables (GLFW internals)
//========================================================================

//------------------------------------------------------------------------
// Shared libraries
//------------------------------------------------------------------------

#if defined( _init_c_ )
struct GfxBase       * GfxBase;
struct IntuitionBase * IntuitionBase;
struct Library       * KeymapBase;
struct UtilityBase   * UtilityBase;
struct Device        * TimerBase;
#endif

GLFWGLOBAL struct MsgPort       * TimerMP;
GLFWGLOBAL struct timerequest   * TimerIO;


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
    struct Screen            *Screen;   // Screen handle
    struct Window            *Window;   // Window handle
    ULONG     ModeID;                   // ModeID
    APTR      PointerSprite;            // Memory for blank pointer sprite
    int       PointerHidden;            // Is pointer hidden?

    // Mesa/OpenGL flavour specific
#ifdef _GLFW_STORMMESA
    struct amigamesa_context *Context;  // GL context handle
#endif

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
    int  MouseMoved, OldMouseX, OldMouseY;

} _glfwInput;


//------------------------------------------------------------------------
// Timer status
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    double       Resolution;
    long long    t0;
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
    struct Process *AmiProc;
    struct Task    *AmiTask;

    // "Wait for" object. Can be a thread, condition variable or NULL.
    void           *WaitFor;
    int            WaitSig;

    // MorphOS support
#ifdef _GLFW_MORPHOS
    struct EmulLibEntry mosEmulLibEntry;
#endif
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
    struct SignalSemaphore CriticalSection;

    // Next condition variable ID (decrements for every created cond)
    unsigned int NextCondID;

} _glfwThrd;


//========================================================================
// Macros for encapsulating critical code sections (i.e. making parts
// of GLFW thread safe)
//========================================================================

// Thread list management
#define ENTER_THREAD_CRITICAL_SECTION \
        ObtainSemaphore( &_glfwThrd.CriticalSection );
#define LEAVE_THREAD_CRITICAL_SECTION \
        ReleaseSemaphore( &_glfwThrd.CriticalSection );


//========================================================================
// Prototypes for platform specific internal functions
//========================================================================

// Time
void _glfwInitTimer( void );

// Fullscreen
int _glfwOpenScreen( int *width, int *height, int *r, int *g, int *b,
    int refresh );
int _glfwGetClosestVideoMode( int *w, int *h, int *r, int *g, int *b,
                              int refresh );
void _glfwGetModeIDInfo( ULONG ModeID, int *w, int *h, int *r, int *g,
                         int *b, int *refresh );

#endif // _platform_h_
