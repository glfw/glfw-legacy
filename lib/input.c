//========================================================================
// GLFW - An OpenGL framework
// File:        input.c
// Platform:    Any
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
// $Id: input.c,v 1.2 2003-02-02 22:39:18 marcus256 Exp $
//========================================================================

#include "internal.h"


//========================================================================
// glfwGetKey()
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwGetKey( int key )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return GLFW_RELEASE;
    }

    // Is it a valid key?
    if( key < 0 || key > GLFW_KEY_LAST )
    {
        return GLFW_RELEASE;
    }

    if( _glfwInput.Key[ key ] == 2 )
    {
        // Sticky mode: release key now
        _glfwInput.Key[ key ] = 0;
        return GLFW_PRESS;
    }

    return (int) _glfwInput.Key[ key ];
}


//========================================================================
// glfwGetMouseButton()
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwGetMouseButton( int button )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return GLFW_RELEASE;
    }

    // Is it a valid mouse button?
    if( button < 0 || button > GLFW_MOUSE_BUTTON_LAST )
    {
        return GLFW_RELEASE;
    }

    if( _glfwInput.MouseButton[ button ] == 2 )
    {
        // Sticky mode: release mouse button now
        _glfwInput.MouseButton[ button ] = 0;
        return GLFW_PRESS;
    }

    return (int) _glfwInput.MouseButton[ button ];
}


//========================================================================
// glfwGetMousePos()
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwGetMousePos( int *xpos, int *ypos )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return;
    }

    // Return mouse position
    if( xpos != NULL )
    {
        *xpos = _glfwInput.MousePosX;
    }
    if( ypos != NULL )
    {
        *ypos = _glfwInput.MousePosY;
    }
}


//========================================================================
// glfwSetMousePos()
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwSetMousePos( int xpos, int ypos )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return;
    }

    // Don't do anything if the mouse position did not change
    if( xpos == _glfwInput.MousePosX && ypos == _glfwInput.MousePosY )
    {
        return;
    }

    // Set GLFW mouse position
    _glfwInput.MousePosX = xpos;
    _glfwInput.MousePosY = ypos;

    // If we have a locked mouse, do not change cursor position
    if( _glfwWin.MouseLock )
    {
        return;
    }

    // Update physical cursor position
    _glfwPlatformSetMouseCursorPos( xpos, ypos );
}


//========================================================================
// glfwGetMouseWheel()
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwGetMouseWheel( void )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return 0;
    }

    // Return mouse wheel position
    return _glfwInput.WheelPos;
}


//========================================================================
// glfwSetMouseWheel()
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwSetMouseWheel( int pos )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return;
    }

    // Set mouse wheel position
    _glfwInput.WheelPos = pos;
}


//========================================================================
// glfwSetKeyCallback() - Set callback function for keyboard input
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwSetKeyCallback( GLFWkeyfun cbfun )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return;
    }

    // Set callback function
    _glfwWin.KeyCallback = cbfun;
}


//========================================================================
// glfwSetCharCallback() - Set callback function for character input
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwSetCharCallback( GLFWcharfun cbfun )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return;
    }

    // Set callback function
    _glfwWin.CharCallback = cbfun;
}


//========================================================================
// glfwSetMouseButtonCallback() - Set callback function for mouse clicks
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwSetMouseButtonCallback( GLFWmousebuttonfun cbfun )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return;
    }

    // Set callback function
    _glfwWin.MouseButtonCallback = cbfun;
}


//========================================================================
// glfwSetMousePosCallback() - Set callback function for mouse moves
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwSetMousePosCallback( GLFWmouseposfun cbfun )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return;
    }

    // Set callback function
    _glfwWin.MousePosCallback = cbfun;

    // Call the callback function to let the application know the current
    // mouse position
    if( cbfun )
    {
        cbfun( _glfwInput.MousePosX, _glfwInput.MousePosY );
    }
}


//========================================================================
// glfwSetMouseWheelCallback() - Set callback function for mouse wheel
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwSetMouseWheelCallback( GLFWmousewheelfun cbfun )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return;
    }

    // Set callback function
    _glfwWin.MouseWheelCallback = cbfun;

    // Call the callback function to let the application know the current
    // mouse wheel position
    if( cbfun )
    {
        cbfun( _glfwInput.WheelPos );
    }
}
