//========================================================================
// This is a small test application for GLFW.
// The program lists all available fullscreen video modes.
//========================================================================

/************************************************************************
 * $Id: listmodes.c,v 1.2 2003-02-02 22:34:12 marcus256 Exp $
 ************************************************************************/

#include <stdio.h>
#include <GL/glfw.h>

// Maximum number of modes that we want to list
#define MAX_NUM_MODES 400


//========================================================================
// main()
//========================================================================

int main( void )
{
    GLFWvidmode dtmode, modes[ MAX_NUM_MODES ];
    int     modecount, i;

    // Initialize GLFW
    if( !glfwInit() )
    {
        return 0;
    }

    // Show desktop video mode
    glfwGetDesktopMode( &dtmode );
    printf( "Desktop mode: %d x %d x %d\n\n",
            dtmode.Width, dtmode.Height, dtmode.RedBits +
            dtmode.GreenBits + dtmode.BlueBits );

    // List available video modes
    modecount = glfwGetVideoModes( modes, MAX_NUM_MODES );
    printf( "Available modes:\n" );
    for( i = 0; i < modecount; i ++ )
    {
        printf( "%3d: %d x %d x %d\n", i,
                modes[i].Width, modes[i].Height, modes[i].RedBits +
                modes[i].GreenBits + modes[i].BlueBits );
    }

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
