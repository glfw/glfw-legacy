//========================================================================
// GLFW - An OpenGL framework
// File:        image.c
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
// $Id: image.c,v 1.2 2003-02-02 22:39:18 marcus256 Exp $
//========================================================================

//========================================================================
// Description:
//
// This module acts as an interface for different image file formats (the
// image file format is detected automatically).
//
// By default the loaded image is rescaled (using bilinear interpolation)
// to the next higher 2^N x 2^M resolution, unless it has a valid
// 2^N x 2^M resolution. The interpolation is quite slow, even if the
// routine has been optimized for speed (a 200x200 RGB image is scaled to
// 256x256 in ~30 ms on a P3-500).
//
// Paletted images are converted to RGB/RGBA images.
//
// A convenience function is also included (glfwLoadTexture2D), which
// loads a texture image from a file directly to OpenGL texture memory,
// with an option to generate all mipmap levels. GL_SGIS_generate_mipmap
// is used whenever available, which should give an optimal mipmap
// generation speed (possibly performed in hardware). A software fallback
// method is included when GL_SGIS_generate_mipmap is not supported (it
// generates all mipmaps of a 256x256 RGB texture in ~3 ms on a P3-500).
//
//========================================================================


#include "internal.h"


// We want to support automatic mipmap generation
#ifndef GL_SGIS_generate_mipmap
#define GL_GENERATE_MIPMAP_SGIS       0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS  0x8192
#define GL_SGIS_generate_mipmap    1
#endif // GL_SGIS_generate_mipmap


//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwUpsampleImage() - Upsample image, from size w1 x h1 to w2 x h2
//========================================================================

static void _glfwUpsampleImage( unsigned char *src, unsigned char *dst,
    int w1, int h1, int w2, int h2, int bpp )
{
    int m, n, k, x, y, col8;
    float dx, dy, xstep, ystep, col, col1, col2;
    unsigned char *src1, *src2, *src3, *src4;

    // Calculate scaling factor
    xstep = (float)(w1-1) / (float)(w2-1);
    ystep = (float)(h1-1) / (float)(h2-1);

    // Copy source data to destination data with bilinear interpolation
    // Note: The rather strange look of this routine is a direct result of
    // my attempts at optimizing it. Improvements are welcome!
    dy = 0.0f;
    y = 0;
    for( n = 0; n < h2; n ++ )
    {
        dx = 0.0f;
        src1 = &src[ y*w1*bpp ];
        src3 = y < (h1-1) ? src1 + w1*bpp : src1;
        src2 = src1 + bpp;
        src4 = src3 + bpp;
        x = 0;
        for( m = 0; m < w2; m ++ )
        {
            for( k = 0; k < bpp; k ++ )
            {
                col1 = *src1 ++;
                col2 = *src2 ++;
                col = col1 + (col2 - col1) * dx;
                col1 = *src3 ++;
                col2 = *src4 ++;
                col2 = col1 + (col2 - col1) * dx;
                col += (col2 - col) * dy;
                col8 = (int) (col + 0.5);
                if( col8 >= 256 ) col8 = 255;
                *dst++ = (unsigned char) col8;
            }
            dx += xstep;
            if( dx >= 1.0f )
            {
                x ++;
                dx -= 1.0f;
                if( x >= (w1-1) )
                {
                    src2 = src1;
                    src4 = src3;
                }
            }
            else
            {
                src1 -= bpp;
                src2 -= bpp;
                src3 -= bpp;
                src4 -= bpp;
            }
        }
        dy += ystep;
        if( dy >= 1.0f )
        {
            y ++;
            dy -= 1.0f;
        }
    }
}


//========================================================================
// _glfwHalveImage() - Build the next mip-map level
//========================================================================

static int _glfwHalveImage( GLubyte *src, int *width, int *height,
    int components )
{
    int     halfwidth, halfheight, m, n, k, idx1, idx2;
    GLubyte *dst;

    // Last level?
    if( *width <= 1 && *height <= 1 )
    {
        return GL_FALSE;
    }

    // Calculate new width and height (handle 1D case)
    halfwidth  = *width > 1 ? *width / 2 : 1;
    halfheight = *height > 1 ? *height / 2 : 1;

    // Downsample image with a simple box-filter
    dst = src;
    if( *width == 1 || *height == 1 )
    {
        // 1D case
        for( m = 0; m < halfwidth+halfheight-1; m ++ )
        {
            for( k = 0; k < components; k ++ )
            {
                *dst ++ = (GLubyte) (((int)*src +
                                      (int)src[components] + 1) >> 1);
                src ++;
            }
            src += components;
        }
    }
    else
    {
        // 2D case
        idx1 = *width*components;
        idx2 = (*width+1)*components;
        for( m = 0; m < halfheight; m ++ )
        {
            for( n = 0; n < halfwidth; n ++ )
            {
                for( k = 0; k < components; k ++ )
                {
                    *dst ++ = (GLubyte) (((int)*src +
                                          (int)src[components] +
                                          (int)src[idx1] +
                                          (int)src[idx2] + 2) >> 2);
                    src ++;
                }
                src += components;
            }
            src += components * (*width);
        }
    }

    // Return new width and height
    *width = halfwidth;
    *height = halfheight;

    return GL_TRUE;
}


//************************************************************************
//****                    GLFW user functions                         ****
//************************************************************************

//========================================================================
// glfwReadImage() - Read an image from a named file
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwReadImage( const char *name, GLFWimage *img,
    int flags )
{
    int     success, width, height, log2, newsize;
    FILE    *f;
    unsigned char *data;

    // Start with an empty image descriptor
    img->Width         = 0;
    img->Height        = 0;
    img->BytesPerPixel = 0;
    img->Data          = NULL;

    // Open file
    f = fopen( name, "rb" );
    if( f == NULL )
    {
        return GL_FALSE;
    }

    // We only support TGA files at the moment
    success = _glfwReadTGA( f, img, flags );
    if( !success )
    {
        fclose( f );
        return GL_FALSE;
    }

    // Close file
    fclose( f );

    // Should we rescale the image to closest 2^N x 2^M resolution?
    if( !(flags & GLFW_NO_RESCALE_BIT) )
    {
        // Calculate next larger 2^N width
        for( log2 = 0, width = img->Width; width > 1; width >>= 1, log2 ++ );
        width  = (int) 1 << log2;
        if( width < img->Width )
        {
            width <<= 1;
        }

        // Calculate next larger 2^M height
        for( log2 = 0, height = img->Height; height > 1; height >>= 1, log2 ++ );
        height = (int) 1 << log2;
        if( height < img->Height )
        {
            height <<= 1;
        }
    }
    else
    {
        width  = img->Width;
        height = img->Height;
    }

    // Do we really need to rescale?
    if( width != img->Width || height != img->Height )
    {
        // Allocate memory for new (upsampled) image data
        newsize = width * height * img->BytesPerPixel;
        data = (unsigned char *) malloc( newsize );
        if( data == NULL )
        {
            free( img->Data );
            return GL_FALSE;
        }

        // Copy old image data to new image data with interpolation
        _glfwUpsampleImage( img->Data, data, img->Width, img->Height,
                            width, height, img->BytesPerPixel );

        // Free memory for old image data (not needed anymore)
        free( img->Data );

        // Set pointer to new image data, and set new image dimensions
        img->Data   = data;
        img->Width  = width;
        img->Height = height;
    }

    // Interpret BytesPerPixel as an OpenGL format
    switch( img->BytesPerPixel )
    {
        default:
        case 1:
            img->Format = GL_LUMINANCE;
            break;
        case 3:
            img->Format = GL_RGB;
            break;
        case 4:
            img->Format = GL_RGBA;
            break;
    }

    return GL_TRUE;
}


//========================================================================
// glfwFreeImage() - Free allocated memory for an image
//========================================================================

GLFWAPI void GLFWAPIENTRY glfwFreeImage( GLFWimage *img )
{
    // Free memory
    if( img->Data != NULL )
    {
        free( img->Data );
        img->Data = NULL;
    }

    // Clear all fields
    img->Width         = 0;
    img->Height        = 0;
    img->Format        = 0;
    img->BytesPerPixel = 0;
}


//========================================================================
// glfwLoadTexture2D() - Read an image from a file, and upload it to
// texture memory
//========================================================================

GLFWAPI int GLFWAPIENTRY glfwLoadTexture2D( const char *name, int flags )
{
    GLFWimage img;
    GLint   UnpackAlignment, GenMipMap;
    int     level, AutoGen;

    // Is GLFW initialized?
    if( !_glfwInitialized || !_glfwWin.Opened )
    {
        return GL_FALSE;
    }

    // Read image from file
    if( !glfwReadImage( name, &img, flags & (~GLFW_NO_RESCALE_BIT) ) )
    {
        return GL_FALSE;
    }

    // Is the GL_SGIS_generate_mipmap extension initialized?
    if( _glfwWin.Has_GL_SGIS_generate_mipmap < 0 )
    {
        _glfwWin.Has_GL_SGIS_generate_mipmap =
            glfwExtensionSupported( "GL_SGIS_generate_mipmap" );
    }

    // Set unpack alignment to one byte
    glGetIntegerv( GL_UNPACK_ALIGNMENT, &UnpackAlignment );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    // Should we use automatic mipmap generation?
    AutoGen = ( flags & GLFW_BUILD_MIPMAPS_BIT ) &&
              _glfwWin.Has_GL_SGIS_generate_mipmap;

    // Enable automatic mipmap generation
    if( AutoGen )
    {
        glGetTexParameteriv( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS,
            &GenMipMap );
        glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS,
            GL_TRUE );
    }

    // Upload to texture memeory
    level = 0;
    do
    {
        // Upload this mipmap level
        glTexImage2D( GL_TEXTURE_2D, level, img.BytesPerPixel,
            img.Width, img.Height, 0, img.Format,
            GL_UNSIGNED_BYTE, (void*) img.Data );

        // Build next mipmap level manually, if required
        if( ( flags & GLFW_BUILD_MIPMAPS_BIT ) && !AutoGen )
        {
            level = _glfwHalveImage( img.Data, &img.Width,
                        &img.Height, img.BytesPerPixel ) ?
                    level + 1 : 0;
        }
    }
    while( level != 0 );

    // Restore old automatic mipmap generation state
    if( AutoGen )
    {
        glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS,
            GenMipMap );
    }

    // Restore old unpack alignment
    glPixelStorei( GL_UNPACK_ALIGNMENT, UnpackAlignment );

    // Data buffer is not needed anymore
    free( img.Data );

    return GL_TRUE;
}
