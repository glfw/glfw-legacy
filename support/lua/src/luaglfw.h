//========================================================================
// GLFW - An OpenGL framework
// File:        luaglfw.h
// Platform:    Lua 5.0
// API version: 2.5
// Author:      Marcus Geelnard (marcus.geelnard at home.se)
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2005 Marcus Geelnard
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
// $Id: luaglfw.h,v 1.2 2005-03-14 21:09:49 marcus256 Exp $
//========================================================================

#ifndef __luaglfw_h_
#define __luaglfw_h_

#ifdef __cplusplus
extern "C" {
#endif


#include <lua.h>

int luaopen_glfw( lua_State * L );


#ifdef __cplusplus
}
#endif

#endif // __luaglfw_h_
