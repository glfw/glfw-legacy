//========================================================================
// GLFW - An OpenGL framework
// File:        enable.c
// Platform:    Any
// API version: 2.4
// Author:      Marcus Geelnard (marcus.geelnard at home.se)
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2004 Marcus Geelnard
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
// $Id: enable.c,v 1.4 2004-02-14 20:50:10 marcus256 Exp $
//========================================================================

#include "internal.h"


//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwEnableMouseCursor() - Enable (show) mouse cursor
// _glfwDisableMouseCursor() - Disable (hide) mouse cursor
//========================================================================

static void _glfwEnableMouseCursor( void )
{
    if( !_glfwWin.Opened || !_glfwWin.MouseLock )
    {
        return;
    }

    // Show mouse cursor
    _glfwPlatformShowMouseCursor();

    // From now on the mouse is unlocked
    _glfwWin.MouseLock = GL_FALSE;
}

static void _glfwDisableMouseCursor( void )
{
    if( !_glfwWin.Opened || _glfwWin.MouseLock )
    {
        return;
    }

    // Hide mouse cursor
    _glfwPlatformHideMouseCursor();

    // Move cursor to the middle of the window
    _glfwPlatformSetMouseCursorPos( _glfwWin.Width>>1,
                                    _glfwWin.Height>>1 );

    // From now on the mouse is locked
    _glfwWin.MouseLock = GL_TRUE;
}


//========================================================================
// _glfwEnableStickyKeys() - Enable sticky keys
// _glfwDisableStickyKeys() - Disable sticky keys
//========================================================================

static void _glfwEnableStickyKeys( void )
{
    _glfwInput.StickyKeys = 1;
}

static void _glfwDisableStickyKeys( void )
{
    int i;

    _glfwInput.StickyKeys = 0;

    // Release all sticky keys
    for( i = 0; i <= GLFW_KEY_LAST; i ++ )
    {
        if( _glfwInput.Key[ i ] == 2 )
        {
            _glfwInput.Key[ i ] = 0;
        }
    }
}


//========================================================================
// _glfwEnableStickyMouseButtons() - Enable sticky mouse buttons
// _glfwDisableStickyMouseButtons() - Disable sticky mouse buttons
//========================================================================

static void _glfwEnableStickyMouseButtons( void )
{
    _glfwInput.StickyMouseButtons = 1;
}

static void _glfwDisableStickyMouseButtons( void )
{
    int i;

    _glfwInput.StickyMouseButtons = 0;

    // Release all sticky mouse buttons
    for( i = 0; i <= GLFW_MOUSE_BUTTON_LAST; i ++ )
    {
        if( _glfwInput.MouseButton[ i ] == 2 )
        {
            _glfwInput.MouseButton[ i ] = 0;
        }
    }
}


//========================================================================
// _glfwEnableSystemKeys() - Enable system keys
// _glfwDisableSystemKeys() - Disable system keys
//========================================================================

static void _glfwEnableSystemKeys( void )
{
    if( !_glfwWin.SysKeysDisabled )
    {
        return;
    }

    _glfwPlatformEnableSystemKeys();

    // Indicate that system keys are no longer disabled
    _glfwWin.SysKeysDisabled = GL_FALSE;
}

static void _glfwDisableSystemKeys( void )
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
// _glfwEnableKeyRepeat() - Enable key repeat
// _glfwDisableKeyRepeat() - Disable key repeat
//========================================================================

static void _glfwEnableKeyRepeat( void )
{
    _glfwInput.KeyRepeat = 1;
}

static void _glfwDisableKeyRepeat( void )
{
    _glfwInput.KeyRepeat = 0;
}


//========================================================================
// _glfwEnableAutoPollEvents() - Enable automatic event polling
// _glfwDisableAutoPollEvents() - Disable automatic event polling
//========================================================================

static void _glfwEnableAutoPollEvents( void )
{
    _glfwWin.AutoPollEvents = 1;
}

static void _glfwDisableAutoPollEvents( void )
{
    _glfwWin.AutoPollEvents = 0;
}



//************************************************************************
//****                    GLFW user functions                         ****
//************************************************************************

//========================================================================
// glfwEnable() - Enable certain GLFW/window/system functions.
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
        _glfwEnableMouseCursor();
        break;
    case GLFW_STICKY_KEYS:
        _glfwEnableStickyKeys();
        break;
    case GLFW_STICKY_MOUSE_BUTTONS:
        _glfwEnableStickyMouseButtons();
        break;
    case GLFW_SYSTEM_KEYS:
        _glfwEnableSystemKeys();
        break;
    case GLFW_KEY_REPEAT:
        _glfwEnableKeyRepeat();
        break;
    case GLFW_AUTO_POLL_EVENTS:
        _glfwEnableAutoPollEvents();
        break;
    default:
        break;
    }
}


//========================================================================
// glfwDisable() - Disable certain GLFW/window/system functions.
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
        _glfwDisableMouseCursor();
        break;
    case GLFW_STICKY_KEYS:
        _glfwDisableStickyKeys();
        break;
    case GLFW_STICKY_MOUSE_BUTTONS:
        _glfwDisableStickyMouseButtons();
        break;
    case GLFW_SYSTEM_KEYS:
        _glfwDisableSystemKeys();
        break;
    case GLFW_KEY_REPEAT:
        _glfwDisableKeyRepeat();
        break;
    case GLFW_AUTO_POLL_EVENTS:
        _glfwDisableAutoPollEvents();
        break;
    default:
        break;
    }
}
