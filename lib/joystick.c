//========================================================================
// GLFW - An OpenGL framework
// File:     joystick.c
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

#include "internal.h"


//************************************************************************
//****                    GLFW user functions                         ****
//************************************************************************

//========================================================================
// glfwGetJoystickParam() - Determine joystick capabilities
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwGetJoystickParam( int joy, int param )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return 0;
    }

    return _glfwPlatformGetJoystickParam( joy, param );
}


//========================================================================
// glfwGetJoystickPos() - Get joystick axis positions
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwGetJoystickPos( int joy, float *pos,
    int numaxes )
{
    int       i;

    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return 0;
    }

    // Clear positions
    for( i = 0; i < numaxes; i++ )
    {
        pos[ i ] = 0.0f;
    }

    return _glfwPlatformGetJoystickPos( joy, pos, numaxes );
}


//========================================================================
// glfwGetJoystickButtons() - Get joystick button states
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwGetJoystickButtons( int joy,
    unsigned char *buttons, int numbuttons )
{
    int       i;

    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return 0;
    }

    // Clear button states
    for( i = 0; i < numbuttons; i++ )
    {
        buttons[ i ] = GLFW_RELEASE;
    }

    return _glfwPlatformGetJoystickButtons( joy, buttons, numbuttons );
}
