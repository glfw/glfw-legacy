//========================================================================
// GLFW - An OpenGL framework
// File:        x11_joystick.c
// Platform:    X11 (Unix)
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
// $Id: x11_joystick.c,v 1.2 2003-02-02 21:24:56 marcus256 Exp $
//========================================================================

#include "internal.h"


//========================================================================
// Note: Only Linux joystick input is supported at the moment. Other
// systems will behave as if there are no joysticks connected.
//========================================================================

#ifdef linux
#define _GLFW_USE_LINUX_JOYSTICKS
#endif // linux



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

#ifdef _GLFW_USE_LINUX_JOYSTICKS

//------------------------------------------------------------------------
// Here are the Linux joystick driver v1.x interface definitions that we
// use (we do not want to rely on <linux/joystick.h>):
//------------------------------------------------------------------------

#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

// Joystick event types
#define JS_EVENT_BUTTON     0x01    /* button pressed/released */
#define JS_EVENT_AXIS       0x02    /* joystick moved */
#define JS_EVENT_INIT       0x80    /* initial state of device */

// Joystick event structure
struct js_event {
    unsigned int  time;    /* (u32) event timestamp in milliseconds */
    signed short  value;   /* (s16) value */
    unsigned char type;    /* (u8)  event type */
    unsigned char number;  /* (u8)  axis/button number */
};

// Joystick IOCTL commands
#define JSIOCGVERSION  _IOR('j', 0x01, int)   /* get driver version (u32) */
#define JSIOCGAXES     _IOR('j', 0x11, char)  /* get number of axes (u8) */
#define JSIOCGBUTTONS  _IOR('j', 0x12, char)  /* get number of buttons (u8) */

#endif // _GLFW_USE_LINUX_JOYSTICKS


//========================================================================
// _glfwInitJoysticks() - Initialize joystick interface
//========================================================================

void _glfwInitJoysticks( void )
{
#ifdef _GLFW_USE_LINUX_JOYSTICKS
    int  n, fd;
    char joy_dev_name[ 16 ];
    int  driver_version = 0x000800;
    char ret_data;
#endif // _GLFW_USE_LINUX_JOYSTICKS
    int  i;

    // Start by saying that there are no sticks
    for( i = 0; i <= GLFW_JOYSTICK_LAST; i ++ )
    {
        _glfwJoy[ i ].Present = GL_FALSE;
    }

#ifdef _GLFW_USE_LINUX_JOYSTICKS

    // Try to open joysticks (nonblocking)
    for( i = 0; i <= GLFW_JOYSTICK_LAST; i ++ )
    {
        sprintf( joy_dev_name, "/dev/js%d", i );
        fd = open( joy_dev_name, O_NONBLOCK );
        if( fd != -1 )
        {
            // Remember fd
            _glfwJoy[ i ].fd = fd;

            // Check that the joystick driver version is 1.0+
            ioctl( fd, JSIOCGVERSION, &driver_version );
            if( driver_version < 0x010000 )
            {
                // It's an old 0.x interface (we don't support it)
                close( fd );
                return;
            }

            // Get number of joystick axes
            ioctl( fd, JSIOCGAXES, &ret_data );
            _glfwJoy[ i ].NumAxes = (int) ret_data;

            // Get number of joystick buttons
            ioctl( fd, JSIOCGBUTTONS, &ret_data );
            _glfwJoy[ i ].NumButtons = (int) ret_data;

            // Allocate memory for joystick state, and clear it
            _glfwJoy[ i ].Axis = (float *) malloc( sizeof(float) *
                                 _glfwJoy[ i ].NumAxes );
            if( _glfwJoy[ i ].Axis == NULL )
            {
                return;
            }
            _glfwJoy[ i ].Button = (char *) malloc( sizeof(char) *
                                    _glfwJoy[ i ].NumButtons );
            if( _glfwJoy[ i ].Button == NULL )
            {
                free( _glfwJoy[ i ].Axis );
                return;
            }
            for( n = 0; n < _glfwJoy[ i ].NumAxes; n ++ )
            {
                _glfwJoy[ i ].Axis[ n ] = 0.0f;
            }
            for( n = 0; n < _glfwJoy[ i ].NumButtons; n ++ )
            {
                _glfwJoy[ i ].Button[ n ] = GLFW_RELEASE;
            }

            // The joystick is supported and connected
            _glfwJoy[ i ].Present = GL_TRUE;
        }
    }

#endif // _GLFW_USE_LINUX_JOYSTICKS

}


//========================================================================
// _glfwTerminateJoysticks() - Close all opened joystick handles
//========================================================================

void _glfwTerminateJoysticks( void )
{

#ifdef _GLFW_USE_LINUX_JOYSTICKS

    int i;

    // Close any opened joysticks
    for( i = 0; i <= GLFW_JOYSTICK_LAST; i ++ )
    {
        if( _glfwJoy[ i ].Present )
        {
            close( _glfwJoy[ i ].fd );
            free( _glfwJoy[ i ].Axis );
            free( _glfwJoy[ i ].Button );
            _glfwJoy[ i ].Present = GL_FALSE;
        }
    }

#endif // _GLFW_USE_LINUX_JOYSTICKS

}


//========================================================================
// _glfwPollJoystickEvents() - Empty joystick event queue
//========================================================================

static void _glfwPollJoystickEvents( void )
{

#ifdef _GLFW_USE_LINUX_JOYSTICKS

    struct js_event e;
    int    i;

    // Get joystick events for all GLFW joysticks
    for( i = 0; i <= GLFW_JOYSTICK_LAST; i ++ )
    {
        // Is the stick present?
        if( _glfwJoy[ i ].Present )
        {
            // Read all queued events (non-blocking)
            while( read(_glfwJoy[i].fd, &e, sizeof(struct js_event)) > 0 )
            {
                // We don't care if it's an init event or not
                e.type &= ~JS_EVENT_INIT;

                // Check event type
                switch( e.type )
                {
                case JS_EVENT_AXIS:
                    _glfwJoy[ i ].Axis[ e.number ] = (float) e.value /
                                                             32767.0f;
                    // We need to change the sign for the Y axes, so that
                    // positive = up/forward, according to the GLFW spec.
                    if( e.number & 1 )
                    {
                        _glfwJoy[ i ].Axis[ e.number ] =
                            -_glfwJoy[ i ].Axis[ e.number ];
                    }
                    break;

                case JS_EVENT_BUTTON:
                    _glfwJoy[ i ].Button[ e.number ] =
                        e.value ? GLFW_PRESS : GLFW_RELEASE;
                    break;

                default:
                    break;
                }
            }
        }
    }

#endif // _GLFW_USE_LINUX_JOYSTICKS

}


//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetJoystickParam() - Determine joystick capabilities
//========================================================================

int _glfwPlatformGetJoystickParam( int joy, int param )
{
    // Is joystick present?
    if( !_glfwJoy[ joy ].Present )
    {
        return 0;
    }

    switch( param )
    {
    case GLFW_PRESENT:
        return GL_TRUE;

    case GLFW_AXES:
        return _glfwJoy[ joy ].NumAxes;

    case GLFW_BUTTONS:
        return _glfwJoy[ joy ].NumButtons;

    default:
        break;
    }

    return 0;
}


//========================================================================
// _glfwPlatformGetJoystickPos() - Get joystick axis positions
//========================================================================

int _glfwPlatformGetJoystickPos( int joy, float *pos, int numaxes )
{
    int       i;

    // Is joystick present?
    if( !_glfwJoy[ joy ].Present )
    {
        return 0;
    }

    // Update joystick state
    _glfwPollJoystickEvents();

    // Does the joystick support less axes than requested?
    if( _glfwJoy[ joy ].NumAxes < numaxes )
    {
        numaxes = _glfwJoy[ joy ].NumAxes;
    }

    // Copy axis positions from internal state
    for( i = 0; i < numaxes; i ++ )
    {
        pos[ i ] = _glfwJoy[ joy ].Axis[ i ];
    }

    return numaxes;
}


//========================================================================
// _glfwPlatformGetJoystickButtons() - Get joystick button states
//========================================================================

int _glfwPlatformGetJoystickButtons( int joy, unsigned char *buttons,
    int numbuttons )
{
    int       i;

    // Is joystick present?
    if( !_glfwJoy[ joy ].Present )
    {
        return 0;
    }

    // Update joystick state
    _glfwPollJoystickEvents();

    // Does the joystick support less buttons than requested?
    if( _glfwJoy[ joy ].NumButtons < numbuttons )
    {
        numbuttons = _glfwJoy[ joy ].NumButtons;
    }

    // Copy button states from internal state
    for( i = 0; i < numbuttons; i ++ )
    {
        buttons[ i ] = _glfwJoy[ joy ].Button[ i ];
    }

    return numbuttons;
}
