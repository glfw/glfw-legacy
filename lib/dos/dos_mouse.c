//========================================================================
// GLFW - An OpenGL framework
// File:        dos_mouse.c
// Platform:    DOS
// API version: 2.4
// Authors:     Daniel Borca (dborca at yahoo.com)
//              Marcus Geelnard (marcus.geelnard at home.se)
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
// $Id: dos_mouse.c,v 1.4 2003-12-08 23:26:54 marcus256 Exp $
//========================================================================

#include "internal.h"

//========================================================================
// Most of the code in this source file is based on the mouse driver in
// Daniel Borca's GLUT implementation for DOS/MESA.
//
//                   !!! MUCH CLEANUP TO BE DONE !!!
//
//========================================================================

//========================================================================
// Definitions
//========================================================================

#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define MID(x,y,z)   MAX((x), MIN((y), (z)))

#define MOUSE_STACK_SIZE 16384

#define CLEAR_MICKEYS() \
    do { \
        __asm __volatile ("movw $0xb, %%ax; int $0x33":::"%eax", "%ecx", "%edx"); \
        _glfwMouseDrv.ox = _glfwMouseDrv.oy = 0; \
    } while (0)


// Assembler function prototypes
extern void mouse_wrap( void );
extern int  *mouse_wrap_end;


//========================================================================
// Global variables
//========================================================================

static struct {
    volatile int x, y, z, b;
    volatile int old_x, old_y, old_z, old_b;
    long         mouse_callback;
    __dpmi_regs  Regs;

    int minx, maxx, miny, maxy, minz, maxz;
    int ox, oy;
    int emulat3;

} _glfwMouseDrv;

static int _glfwMouseInstalled = 0;



//************************************************************************
//**** Mouse Interrupt ***************************************************
//************************************************************************

//========================================================================
// _glfwMouseFunc() - Mouse interrupt callback function
//========================================================================

static void _glfwMouseFunc( int mouse_x, int mouse_y, int mouse_z,
    int mouse_b )
{
    _GLFWdosevent event;
    struct mousemove_event   *mousemove   = &event.MouseMove;
    struct mousewheel_event  *mousewheel  = &event.MouseWheel;
    struct mousebutton_event *mousebutton = &event.MouseButton;

    /* mouse moved? */
    if( (mouse_x != _glfwMouseDrv.old_x) ||
        (mouse_y != _glfwMouseDrv.old_y) )
    {
        mousemove->Type = _GLFW_DOS_MOUSE_MOVE_EVENT;
        mousemove->DeltaX = mouse_x - _glfwMouseDrv.old_x;
        mousemove->DeltaY = mouse_y - _glfwMouseDrv.old_y;
        _glfwPostDOSEvent( &event );

        _glfwMouseDrv.old_x = mouse_x;
        _glfwMouseDrv.old_y = mouse_y;
    }

    /* mouse wheel moved? */
    if( mouse_z != _glfwMouseDrv.old_z )
    {
        mousewheel->Type = _GLFW_DOS_MOUSE_WHEEL_EVENT;
        mousewheel->WheelDelta = mouse_z - _glfwMouseDrv.old_z;
        _glfwPostDOSEvent( &event );

        _glfwMouseDrv.old_z = mouse_z;
    }

    /* button state changed? */
    if( mouse_b != _glfwMouseDrv.old_b )
    {
        mousebutton->Type = _GLFW_DOS_MOUSE_BUTTON_EVENT;

        // Left mouse button changed?
        if( (_glfwMouseDrv.old_b & 1) && !(mouse_b & 1) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_LEFT;
            mousebutton->Action = GLFW_RELEASE;
            _glfwPostDOSEvent( &event );
        }
        else if( !(_glfwMouseDrv.old_b & 1) && (mouse_b & 1) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_LEFT;
            mousebutton->Action = GLFW_PRESS;
            _glfwPostDOSEvent( &event );
        }

        // Right mouse button changed?
        if( (_glfwMouseDrv.old_b & 2) && !(mouse_b & 2) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_RIGHT;
            mousebutton->Action = GLFW_RELEASE;
            _glfwPostDOSEvent( &event );
        }
        else if( !(_glfwMouseDrv.old_b & 2) && (mouse_b & 2) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_RIGHT;
            mousebutton->Action = GLFW_PRESS;
            _glfwPostDOSEvent( &event );
        }

        // Middle mouse button changed?
        if( (_glfwMouseDrv.old_b & 4) && !(mouse_b & 4) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_MIDDLE;
            mousebutton->Action = GLFW_RELEASE;
            _glfwPostDOSEvent( &event );
        }
        else if( !(_glfwMouseDrv.old_b & 4) && (mouse_b & 4) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_MIDDLE;
            mousebutton->Action = GLFW_PRESS;
            _glfwPostDOSEvent( &event );
        }

        _glfwMouseDrv.old_b = mouse_b;
    }
} ENDOFUNC(_glfwMouseFunc)


//========================================================================
// mouse() - Mouse interrupt handler
//========================================================================

static void mouse( __dpmi_regs *r )
{
    int newx, newy, dx, dy, dz;

    // Calculate mouse deltas
    newx = (signed short)r->x.si;
    newy = (signed short)r->x.di;
    dx = newx - _glfwMouseDrv.ox;
    dy = newy - _glfwMouseDrv.oy;
    dz = (signed char)r->h.bh;

    // Remember old mouse position
    _glfwMouseDrv.ox = newx;
    _glfwMouseDrv.oy = newy;

    // Store x,y,z & button state
    _glfwMouseDrv.x = MID( _glfwMouseDrv.minx, _glfwMouseDrv.x + dx,
                           _glfwMouseDrv.maxx);
    _glfwMouseDrv.y = MID( _glfwMouseDrv.miny, _glfwMouseDrv.y + dy,
                           _glfwMouseDrv.maxy);
    _glfwMouseDrv.z = MID( _glfwMouseDrv.minz, _glfwMouseDrv.z + dz,
                           _glfwMouseDrv.maxz);
    _glfwMouseDrv.b = r->h.bl;

    // Emulate 3rd mouse button?
    if( _glfwMouseDrv.emulat3 )
    {
        if( (_glfwMouseDrv.b & 3) == 3 )
        {
            _glfwMouseDrv.b = 4;
        }
    }

    // Create new mouse events
    _glfwMouseFunc( _glfwMouseDrv.x, _glfwMouseDrv.y, _glfwMouseDrv.z,
                    _glfwMouseDrv.b );
} ENDOFUNC(mouse)


//========================================================================
// _glfwInstallMouse() - Install mouse driver
//========================================================================

static int _glfwInstallMouse( void )
{
    int buttons;

    /* fail if already call-backed */
    if( _glfwMouseDrv.mouse_callback )
    {
        return 0;
    }

    /* reset mouse and get status */
    __asm("\n\
                xorl    %%eax, %%eax    \n\
                int     $0x33           \n\
                andl    %%ebx, %%eax    \n\
                movl    %%eax, %0       \n\
    ":"=g" (buttons)::"%eax", "%ebx");
    if( !buttons )
    {
        return 0;
    }

    /* lock wrapper */
    LOCKFUNC(mouse);
    LOCKFUNC(mouse_wrap);

    mouse_wrap_end[1] = __djgpp_ds_alias;

    /* grab a locked stack */
    mouse_wrap_end[0] = (int)malloc( MOUSE_STACK_SIZE );
    if( mouse_wrap_end[0] == NULL )
    {
        return 0;
    }
    LOCKBUFF( mouse_wrap_end[0], MOUSE_STACK_SIZE );

    /* try to hook a call-back */
    __asm("\n\
                pushl   %%ds            \n\
                pushl   %%es            \n\
                movw    $0x0303, %%ax   \n\
                pushl   %%ds            \n\
                pushl   %%cs            \n\
                popl    %%ds            \n\
                popl    %%es            \n\
                int     $0x31           \n\
                popl    %%es            \n\
                popl    %%ds            \n\
                jc      0f              \n\
                shll    $16, %%ecx      \n\
                movw    %%dx, %%cx      \n\
                movl    %%ecx, %0       \n\
        0:                              \n\
    ":"=g"(_glfwMouseDrv.mouse_callback)
    :"S" (mouse_wrap), "D"(&_glfwMouseDrv.Regs)
    :"%eax", "%ecx", "%edx");
    if( !_glfwMouseDrv.mouse_callback )
    {
        free( (void *)mouse_wrap_end[0] );
        return 0;
    }

    /* adjust stack */
    mouse_wrap_end[0] += MOUSE_STACK_SIZE;

    /* install the handler */
    _glfwMouseDrv.Regs.x.ax = 0x000c;
    _glfwMouseDrv.Regs.x.cx = 0x7f | 0x80;
    _glfwMouseDrv.Regs.x.dx = _glfwMouseDrv.mouse_callback & 0xffff;
    _glfwMouseDrv.Regs.x.es = _glfwMouseDrv.mouse_callback >> 16;
    __dpmi_int(0x33, &_glfwMouseDrv.Regs);

    CLEAR_MICKEYS();

    _glfwMouseDrv.emulat3 = buttons<3;

    return buttons;
}


//========================================================================
// _glfwRemoveMouse() - Uninstall mouse driver
//========================================================================

static void _glfwRemoveMouse( void )
{
    if( _glfwMouseDrv.mouse_callback )
    {
        __asm("\n\
                movl    %%edx, %%ecx    \n\
                shrl    $16, %%ecx      \n\
                movw    $0x0304, %%ax   \n\
                int     $0x31           \n\
                movw    $0x000c, %%ax   \n\
                xorl    %%ecx, %%ecx    \n\
                int     $0x33           \n\
        "::"d"(_glfwMouseDrv.mouse_callback):"%eax", "%ecx");

        _glfwMouseDrv.mouse_callback = 0;

        free( (void *)(mouse_wrap_end[0] - MOUSE_STACK_SIZE) );
    }
}


//========================================================================
// mouse_wrap()
//========================================================================

/* Hack alert:
 * `mouse_wrap_end' actually holds the
 * address of stack in a safe data selector.
 */
__asm("\n\
                .text                           \n\
                .p2align 5,,31                  \n\
                .global _mouse_wrap             \n\
_mouse_wrap:                                    \n\
                cld                             \n\
                lodsl                           \n\
                movl    %eax, %es:42(%edi)      \n\
                addw    $4, %es:46(%edi)        \n\
                pushl   %es                     \n\
                movl    %ss, %ebx               \n\
                movl    %esp, %esi              \n\
                lss     %cs:_mouse_wrap_end, %esp\n\
                pushl   %ss                     \n\
                pushl   %ss                     \n\
                popl    %es                     \n\
                popl    %ds                     \n\
                movl    ___djgpp_dos_sel, %fs   \n\
                pushl   %fs                     \n\
                popl    %gs                     \n\
                pushl   %edi                    \n\
                call    _mouse                  \n\
                popl    %edi                    \n\
                movl    %ebx, %ss               \n\
                movl    %esi, %esp              \n\
                popl    %es                     \n\
                iret                            \n\
                .global _mouse_wrap_end         \n\
_mouse_wrap_end:.long   0, 0");



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwInitMouse() - Initialize mouse driver
//========================================================================

int _glfwInitMouse( void )
{
    if( _glfwMouseInstalled )
    {
        return 0;
    }

    // Lock data and functions
    LOCKDATA( _glfwMouseDrv );
    LOCKFUNC( _glfwMouseFunc );

    // Setup initial mouse driver settings
    _glfwMouseDrv.minx       = 0;
    _glfwMouseDrv.maxx       = _glfwWin.Width-1;
    _glfwMouseDrv.miny       = 0;
    _glfwMouseDrv.maxy       = _glfwWin.Height-1;
    _glfwMouseDrv.minz       = 0;
    _glfwMouseDrv.maxz       = 255;
    _glfwMouseDrv.emulat3    = 0;

    // Install mouse driver
    _glfwMouseInstalled = _glfwInstallMouse();
    if( !_glfwMouseInstalled )
    {
        return 0;
    }

    return 1;
}


//========================================================================
// _glfwTerminateMouse() - Terminate mouse driver
//========================================================================

void _glfwTerminateMouse( void )
{
    if( _glfwMouseInstalled )
    {
        _glfwRemoveMouse();
        _glfwMouseInstalled = 0;
    }
}
