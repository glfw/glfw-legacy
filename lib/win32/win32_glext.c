//========================================================================
// GLFW - An OpenGL framework
// File:     win32_glext.c
// Platform: Windows
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


//========================================================================
// We use the WGL_EXT_extensions_string if it is available, or
// WGL_ARB_extensions_string if it is available.
//========================================================================

typedef const char *(APIENTRY * WGLGETEXTENSIONSSTRINGEXT_T)( void );
typedef const char *(APIENTRY * WGLGETEXTENSIONSSTRINGARB_T)( HDC hdc );



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformExtensionSupported() - Check if an OpenGL extension is
// available at runtime (Windows version checks for WGL extensions)
//========================================================================

int _glfwPlatformExtensionSupported( const char *extension )
{
    const GLubyte *extensions;
    WGLGETEXTENSIONSSTRINGEXT_T _wglGetExtensionsStringEXT;
    WGLGETEXTENSIONSSTRINGARB_T _wglGetExtensionsStringARB;

    // Try wglGetExtensionsStringEXT
    _wglGetExtensionsStringEXT = (WGLGETEXTENSIONSSTRINGEXT_T)
        wglGetProcAddress( "wglGetExtensionsStringEXT" );
    if( _wglGetExtensionsStringEXT != NULL )
    {
        extensions = (GLubyte *) _wglGetExtensionsStringEXT();
        if( extensions != NULL )
        {
            if( _glfwStringInExtensionString( extension, extensions ) )
            {
                return GL_TRUE;
            }
        }
    }

    // Try wglGetExtensionsStringARB
    _wglGetExtensionsStringARB = (WGLGETEXTENSIONSSTRINGARB_T)
        wglGetProcAddress( "wglGetExtensionsStringARB" );
    if( _wglGetExtensionsStringARB != NULL )
    {
        extensions = (GLubyte *) _wglGetExtensionsStringARB(_glfwWin.DC);
        if( extensions != NULL )
        {
            if( _glfwStringInExtensionString( extension, extensions ) )
            {
                return GL_TRUE;
            }
        }
    }

    return GL_FALSE;
}


//========================================================================
// _glfwPlatformGetProcAddress() - Get the function pointer to an OpenGL
// function
//========================================================================

void * _glfwPlatformGetProcAddress( const char *procname )
{
    return (void *) wglGetProcAddress( procname );
}
