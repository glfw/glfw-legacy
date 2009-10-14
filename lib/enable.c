//========================================================================
// GLFW - An OpenGL framework
// File:        enable.c
// Platform:    Any
// API version: 2.7
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


//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// Enable (show) mouse cursor
//========================================================================

static void EnableMouseCursor( void )
{
    int CenterPosX, CenterPosY;

    if( !_glfwWin.Opened || !_glfwWin.MouseLock )
    {
        return;
    }

    // Show mouse cursor
    _glfwPlatformShowMouseCursor();

    CenterPosX = _glfwWin.Width / 2;
    CenterPosY = _glfwWin.Height / 2;

    if( CenterPosX != _glfwInput.MousePosX || CenterPosY != _glfwInput.MousePosY )
    {
        _glfwPlatformSetMouseCursorPos( CenterPosX, CenterPosY );

        _glfwInput.MousePosX = CenterPosX;
        _glfwInput.MousePosY = CenterPosY;

        if( _glfwWin.MousePosCallback )
        {
            _glfwWin.MousePosCallback( _glfwInput.MousePosX,
                        _glfwInput.MousePosY );
        }
    }

    // From now on the mouse is unlocked
    _glfwWin.MouseLock = GL_FALSE;
}

//========================================================================
// Disable (hide) mouse cursor
//========================================================================

static void DisableMouseCursor( void )
{
    if( !_glfwWin.Opened || _glfwWin.MouseLock )
    {
        return;
    }

    // Hide mouse cursor
    _glfwPlatformHideMouseCursor();

    // Move cursor to the middle of the window
    _glfwPlatformSetMouseCursorPos( _glfwWin.Width >> 1,
                                    _glfwWin.Height >> 1 );

    // From now on the mouse is locked
    _glfwWin.MouseLock = GL_TRUE;
}


//========================================================================
// Enable sticky keys
//========================================================================

static void EnableStickyKeys( void )
{
    _glfwInput.StickyKeys = 1;
}

//========================================================================
// Disable sticky keys
//========================================================================

static void DisableStickyKeys( void )
{
    int i;

    _glfwInput.StickyKeys = 0;

    // Release all sticky keys
    for( i = 0; i <= GLFW_KEY_LAST; i++ )
    {
        if( _glfwInput.Key[ i ] == 2 )
        {
            _glfwInput.Key[ i ] = 0;
        }
    }
}


//========================================================================
// Enable sticky mouse buttons
//========================================================================

static void EnableStickyMouseButtons( void )
{
    _glfwInput.StickyMouseButtons = 1;
}

//========================================================================
// Disable sticky mouse buttons
//========================================================================

static void DisableStickyMouseButtons( void )
{
    int i;

    _glfwInput.StickyMouseButtons = 0;

    // Release all sticky mouse buttons
    for( i = 0; i <= GLFW_MOUSE_BUTTON_LAST; i++ )
    {
        if( _glfwInput.MouseButton[ i ] == 2 )
        {
            _glfwInput.MouseButton[ i ] = 0;
        }
    }
}


//========================================================================
// Enable system keys
//========================================================================

static void EnableSystemKeys( void )
{
    if( !_glfwWin.SysKeysDisabled )
    {
        return;
    }

    _glfwPlatformEnableSystemKeys();

    // Indicate that system keys are no longer disabled
    _glfwWin.SysKeysDisabled = GL_FALSE;
}

//========================================================================
// Disable system keys
//========================================================================

static void DisableSystemKeys( void )
{
    if( _glfwWin.SysKeysDisabled )
    {
        return;
    }

    _glfwPlatformDisableSystemKeys();

    // Indicate that system keys are now disabled
    _glfwWin.SysKeysDisabled = GL_TRUE;
}


//========================================================================
// Enable key repeat
//========================================================================

static void EnableKeyRepeat( void )
{
    _glfwInput.KeyRepeat = 1;
}

//========================================================================
// Disable key repeat
//========================================================================

static void DisableKeyRepeat( void )
{
    _glfwInput.KeyRepeat = 0;
}


//========================================================================
// Enable automatic event polling
//========================================================================

static void EnableAutoPollEvents( void )
{
    _glfwWin.AutoPollEvents = 1;
}

//========================================================================
// Disable automatic event polling
//========================================================================

static void DisableAutoPollEvents( void )
{
    _glfwWin.AutoPollEvents = 0;
}



//************************************************************************
//****                    GLFW user functions                         ****
//************************************************************************

//========================================================================
// Enable certain GLFW/window/system functions.
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwEnable( int token )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return;
    }

    switch( token )
    {
        case GLFW_MOUSE_CURSOR:
            EnableMouseCursor();
            break;
        case GLFW_STICKY_KEYS:
            EnableStickyKeys();
            break;
        case GLFW_STICKY_MOUSE_BUTTONS:
            EnableStickyMouseButtons();
            break;
        case GLFW_SYSTEM_KEYS:
            EnableSystemKeys();
            break;
        case GLFW_KEY_REPEAT:
            EnableKeyRepeat();
            break;
        case GLFW_AUTO_POLL_EVENTS:
            EnableAutoPollEvents();
            break;
        default:
            break;
    }
}


//========================================================================
// Disable certain GLFW/window/system functions.
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwDisable( int token )
{
    // Is GLFW initialized?
    if( !_glfwInitialized )
    {
        return;
    }

    switch( token )
    {
        case GLFW_MOUSE_CURSOR:
            DisableMouseCursor();
            break;
        case GLFW_STICKY_KEYS:
            DisableStickyKeys();
            break;
        case GLFW_STICKY_MOUSE_BUTTONS:
            DisableStickyMouseButtons();
            break;
        case GLFW_SYSTEM_KEYS:
            DisableSystemKeys();
            break;
        case GLFW_KEY_REPEAT:
            DisableKeyRepeat();
            break;
        case GLFW_AUTO_POLL_EVENTS:
            DisableAutoPollEvents();
            break;
        default:
            break;
    }
}

