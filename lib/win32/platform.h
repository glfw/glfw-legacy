//========================================================================
// GLFW - An OpenGL framework
// File:        platform.h
// Platform:    Windows
// API version: 2.4
// Author:      Marcus Geelnard (marcus.geelnard at home.se)
// WWW:         http://glfw.sourceforge.net
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
// marcus.geelnard at home.se
//------------------------------------------------------------------------
// $Id: platform.h,v 1.4 2003-10-29 21:50:07 marcus256 Exp $
//========================================================================

#ifndef _platform_h_
#define _platform_h_


// This is the Windows version of GLFW
#define _GLFW_WIN32


// Include files
#include <windows.h>
#include <mmsystem.h>
#include "../../include/GL/glfw.h"


//========================================================================
// Hack: Define things that some <windows.h>'s do not define
//========================================================================

// Some old versions of w32api (used by MinGW and Cygwin) define
// WH_KEYBOARD_LL without typedef:ing KBDLLHOOKSTRUCT (!)
#if defined(__MINGW32__) || defined(__CYGWIN__)
#include <w32api.h>
#if defined(WH_KEYBOARD_LL) && (__W32API_MAJOR_VERSION == 1) && (__W32API_MINOR_VERSION <= 2)
#undef WH_KEYBOARD_LL
#endif
#endif

//------------------------------------------------------------------------
// ** NOTE **  If this gives you compiler errors and you are using MinGW
// (or Dev-C++), update to w32api version 1.3 or later:
// http://sourceforge.net/project/showfiles.php?group_id=2435
//------------------------------------------------------------------------
#ifndef WH_KEYBOARD_LL
#define WH_KEYBOARD_LL 13
typedef struct tagKBDLLHOOKSTRUCT {
  DWORD   vkCode;
  DWORD   scanCode;
  DWORD   flags;
  DWORD   time;
  DWORD   dwExtraInfo;
} KBDLLHOOKSTRUCT, FAR *LPKBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;
#endif // WH_KEYBOARD_LL

#ifndef LLKHF_ALTDOWN
#define LLKHF_ALTDOWN  0x00000020
#endif

#ifndef SPI_SETSCREENSAVERRUNNING
#define SPI_SETSCREENSAVERRUNNING 97
#endif
#ifndef SPI_GETANIMATION
#define SPI_GETANIMATION 72
#endif
#ifndef SPI_SETANIMATION
#define SPI_SETANIMATION 73
#endif
#ifndef SPI_GETFOREGROUNDLOCKTIMEOUT
#define SPI_GETFOREGROUNDLOCKTIMEOUT 0x2000
#endif
#ifndef SPI_SETFOREGROUNDLOCKTIMEOUT
#define SPI_SETFOREGROUNDLOCKTIMEOUT 0x2001
#endif

#ifndef CDS_FULLSCREEN
#define CDS_FULLSCREEN 4
#endif

#ifndef PFD_GENERIC_ACCELERATED
#define PFD_GENERIC_ACCELERATED 0x00001000
#endif
#ifndef PFD_DEPTH_DONTCARE
#define PFD_DEPTH_DONTCARE 0x20000000
#endif

#ifndef ENUM_CURRENT_SETTINGS
#define ENUM_CURRENT_SETTINGS -1
#endif
#ifndef ENUM_REGISTRY_SETTINGS
#define ENUM_REGISTRY_SETTINGS -2
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

// wglSwapIntervalEXT typedef (Win32 buffer-swap interval control)
typedef int (APIENTRY * WGLSWAPINTERVALEXT_T) (int interval);



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

    // Extensions (context specific)
    int       Has_GL_SGIS_generate_mipmap;


// ========= PLATFORM SPECIFIC PART ======================================

    // Platform specific window resources
    HDC       DC;              // Private GDI device context
    HGLRC     RC;              // Permanent rendering context
    HWND      Wnd;             // Window handle
    HINSTANCE Instance;        // Instance of the application
    int       ModeID;          // Mode ID for fullscreen mode
    HHOOK     KeyboardHook;    // Keyboard hook handle
    DWORD     dwStyle;         // Window styles used for window creation
    DWORD     dwExStyle;       // --"--

    // Platform specific extensions (context specific)
    WGLSWAPINTERVALEXT_T SwapInterval;

    // Various platform specific internal variables
    int       OldMouseLock;    // Old mouse-lock flag (used for remembering
                               // mouse-lock state when iconifying)
    int       OldMouseLockValid;
    int       DesiredRefreshRate; // Desired vertical monitor refresh rate

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
    int          HasPerformanceCounter;
    int          HasRDTSC;
    double       Resolution;
    unsigned int t0_32;
    __int64      t0_64;
} _glfwTimer;


//------------------------------------------------------------------------
// Thread record (one for each thread)
//------------------------------------------------------------------------
typedef struct _GLFWthread_struct _GLFWthread;

struct _GLFWthread_struct {

// ========= PLATFORM INDEPENDENT MANDATORY PART =========================

    // Pointer to previous and next threads in linked list
    _GLFWthread   *Previous, *Next;

    // GLFW user side thread information
    GLFWthread    ID;
    GLFWthreadfun Function;

// ========= PLATFORM SPECIFIC PART ======================================

    // System side thread information
    HANDLE        Handle;
    DWORD         WinID;

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
    CRITICAL_SECTION CriticalSection;

} _glfwThrd;



//------------------------------------------------------------------------
// System information
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    int     WinVer;
    int     HasUnicode;
    DWORD   ForegroundLockTimeout;
} _glfwSys;


//========================================================================
// DLLs that are loaded at glfwInit()
//========================================================================

// gdi32.dll function pointer typedefs
#ifndef _GLFW_NO_DLOAD_GDI32
typedef int  (WINAPI * CHOOSEPIXELFORMAT_T) (HDC,CONST PIXELFORMATDESCRIPTOR*);
typedef int  (WINAPI * DESCRIBEPIXELFORMAT_T) (HDC,int,UINT,LPPIXELFORMATDESCRIPTOR);
typedef int  (WINAPI * GETPIXELFORMAT_T) (HDC);
typedef BOOL (WINAPI * SETPIXELFORMAT_T) (HDC,int,const PIXELFORMATDESCRIPTOR*);
typedef BOOL (WINAPI * SWAPBUFFERS_T) (HDC);
#endif // _GLFW_NO_DLOAD_GDI32

// winmm.dll function pointer typedefs
#ifndef _GLFW_NO_DLOAD_WINMM
typedef MMRESULT (WINAPI * JOYGETDEVCAPSA_T) (UINT,LPJOYCAPSA,UINT);
typedef MMRESULT (WINAPI * JOYGETPOS_T) (UINT,LPJOYINFO);
typedef MMRESULT (WINAPI * JOYGETPOSEX_T) (UINT,LPJOYINFOEX);
typedef DWORD (WINAPI * TIMEGETTIME_T) (void);
#endif // _GLFW_NO_DLOAD_WINMM

// Library handles and function pointers
GLFWGLOBAL struct {
#ifndef _GLFW_NO_DLOAD_GDI32
    // gdi32.dll
    HINSTANCE             gdi32;
    CHOOSEPIXELFORMAT_T   ChoosePixelFormat;
    DESCRIBEPIXELFORMAT_T DescribePixelFormat;
    GETPIXELFORMAT_T      GetPixelFormat;
    SETPIXELFORMAT_T      SetPixelFormat;
    SWAPBUFFERS_T         SwapBuffers;
#endif // _GLFW_NO_DLOAD_GDI32

    // winmm.dll
#ifndef _GLFW_NO_DLOAD_WINMM
    HINSTANCE             winmm;
    JOYGETDEVCAPSA_T      joyGetDevCapsA;
    JOYGETPOS_T           joyGetPos;
    JOYGETPOSEX_T         joyGetPosEx;
    TIMEGETTIME_T         timeGetTime;
#endif // _GLFW_NO_DLOAD_WINMM
} _glfwLibs;

// gdi32.dll shortcuts
#ifndef _GLFW_NO_DLOAD_GDI32
#define _glfw_ChoosePixelFormat   _glfwLibs.ChoosePixelFormat
#define _glfw_DescribePixelFormat _glfwLibs.DescribePixelFormat
#define _glfw_GetPixelFormat      _glfwLibs.GetPixelFormat
#define _glfw_SetPixelFormat      _glfwLibs.SetPixelFormat
#define _glfw_SwapBuffers         _glfwLibs.SwapBuffers
#else
#define _glfw_ChoosePixelFormat   ChoosePixelFormat
#define _glfw_DescribePixelFormat DescribePixelFormat
#define _glfw_GetPixelFormat      GetPixelFormat
#define _glfw_SetPixelFormat      SetPixelFormat
#define _glfw_SwapBuffers         SwapBuffers
#endif // _GLFW_NO_DLOAD_GDI32

// winmm.dll shortcuts
#ifndef _GLFW_NO_DLOAD_WINMM
#define _glfw_joyGetDevCaps _glfwLibs.joyGetDevCapsA
#define _glfw_joyGetPos     _glfwLibs.joyGetPos
#define _glfw_joyGetPosEx   _glfwLibs.joyGetPosEx
#define _glfw_timeGetTime   _glfwLibs.timeGetTime
#else
#define _glfw_joyGetDevCaps joyGetDevCapsA
#define _glfw_joyGetPos     joyGetPos
#define _glfw_joyGetPosEx   joyGetPosEx
#define _glfw_timeGetTime   timeGetTime
#endif // _GLFW_NO_DLOAD_WINMM


//========================================================================
// Macros for encapsulating critical code sections (i.e. making parts
// of GLFW thread safe)
//========================================================================

// Thread list management
#define ENTER_THREAD_CRITICAL_SECTION \
        EnterCriticalSection( &_glfwThrd.CriticalSection );
#define LEAVE_THREAD_CRITICAL_SECTION \
        LeaveCriticalSection( &_glfwThrd.CriticalSection );


//========================================================================
// Various Windows version constants
//========================================================================

#define _GLFW_WIN_UNKNOWN    0x0000  // Earlier than 95 or NT4
#define _GLFW_WIN_95         0x0001
#define _GLFW_WIN_98         0x0002
#define _GLFW_WIN_ME         0x0003
#define _GLFW_WIN_UNKNOWN_9x 0x0004  // Later than ME
#define _GLFW_WIN_NT4        0x0101
#define _GLFW_WIN_2K         0x0102
#define _GLFW_WIN_XP         0x0103
#define _GLFW_WIN_NET_SERVER 0x0104
#define _GLFW_WIN_UNKNOWN_NT 0x0105  // Later than .NET Server


//========================================================================
// Prototypes for platform specific internal functions
//========================================================================

// Time
void _glfwInitTimer( void );

// Fullscreen support
int _glfwGetClosestVideoModeBPP( int *w, int *h, int *bpp, int *refresh );
int _glfwGetClosestVideoMode( int *w, int *h, int *r, int *g, int *b, int *refresh );
void _glfwSetVideoModeMODE( int mode );
void _glfwSetVideoMode( int *w, int *h, int r, int g, int b, int refresh );


#endif // _platform_h_
