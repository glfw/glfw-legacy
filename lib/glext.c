//========================================================================
// GLFW - An OpenGL framework
// File:        glext.c
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
// $Id: glext.c,v 1.2 2003-02-02 22:39:18 marcus256 Exp $
//========================================================================

#include "internal.h"


//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwStringInExtensionString() - Check if a string can be found in an
// OpenGL extension string
//========================================================================

int _glfwStringInExtensionString( const char *string,
    const GLubyte *extensions )
{
    const GLubyte *start;
    GLubyte *where, *terminator;

    // It takes a bit of care to be fool-proof about parsing the
    // OpenGL extensions string. Don't be fooled by sub-strings,
    // etc.
    start = extensions;
    while( 1 )
    {
        where = (GLubyte *) strstr( (const char *) start, string );
        if( !where )
        {
            return GL_FALSE;
        }
        terminator = where + strlen( string );
        if( where == start || *(where - 1) == ' ' )
        {
            if( *terminator == ' ' || *terminator == '\0' )
            {
                break;
            }
        }
        start = terminator;
    }

    return GL_TRUE;
}



//************************************************************************
//****                    GLFW user functions                         ****
//************************************************************************

//========================================================================
// glfwExtensionSupported() - Check if an OpenGL extension is available
// at runtime
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwExtensionSupported( const char *extension )
{
    const GLubyte *extensions;
    GLubyte       *where;

    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return GL_FALSE;
    }

    // Extension names should not have spaces
    where = (GLubyte *) strchr( extension, ' ' );
    if( where || *extension == '\0' )
    {
        return GL_FALSE;
    }

    // Check if extension is in the standard OpenGL extensions string
    extensions = (GLubyte *) glGetString( GL_EXTENSIONS );
    if( extensions != NULL )
    {
        if( _glfwStringInExtensionString( extension, extensions ) )
        {
            return GL_TRUE;
        }
    }

    // Additional platform specific extension checking (e.g. WGL)
    if( _glfwPlatformExtensionSupported( extension ) )
    {
        return GL_TRUE;
    }

    return GL_FALSE;
}


//========================================================================
// glfwGetProcAddress() - Get the function pointer to an OpenGL function.
// This function can be used to get access to extended OpenGL functions.
//========================================================================

GLFWAPI void * GLFWAPIENTRY glfwGetProcAddress( const char *procname )
{
    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return NULL;
    }

    return _glfwPlatformGetProcAddress( procname );
}


//========================================================================
// glfwGetGLVersion() - Get OpenGL version
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwGetGLVersion( int *major, int *minor,
    int *rev )
{
    GLuint _major, _minor = 0, _rev = 0;
    const GLubyte *version;
    GLubyte *ptr;

    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return;
    }

    // Get OpenGL version string
    version = glGetString( GL_VERSION );
    if( !version )
    {
        return;
    }

    // Parse string
    ptr = (GLubyte*) version;
    for( _major = 0; *ptr >= '0' && *ptr <= '9'; ptr ++ )
    {
        _major = 10*_major + (*ptr - '0');
    }
    if( *ptr == '.' )
    {
        ptr ++;
        for( _minor = 0; *ptr >= '0' && *ptr <= '9'; ptr ++ )
        {
            _minor = 10*_minor + (*ptr - '0');
        }
        if( *ptr == '.' )
        {
            ptr ++;
            for( _rev = 0; *ptr >= '0' && *ptr <= '9'; ptr ++ )
            {
                _rev = 10*_rev + (*ptr - '0');
            }
        }
    }

    // Return parsed values
    if( major != NULL )
    {
        *major = _major;
    }
    if( minor != NULL )
    {
        *minor = _minor;
    }
    if( rev != NULL )
    {
        *rev = _rev;
    }
}
