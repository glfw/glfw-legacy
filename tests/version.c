//========================================================================
// Version information dumper
// Copyright (c) Camilla Berglund <elmindreda@users.sourceforge.net>
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
//
// This test is a pale imitation of glxinfo(1), except not really
//
// It dumps GLFW and OpenGL version information
//
//========================================================================

#include <GL/glfw.h>

#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"

static void usage(void)
{
    printf("version [-h] [-m MAJOR] [-n MINOR]\n");
}

int main(int argc, char** argv)
{
    const GLubyte* extensions;
    int ch, major = 0, minor = 0, revision;

    while ((ch = getopt(argc, argv, "hm:n:")) != -1)
    {
        switch (ch)
        {
            case 'h':
                usage();
                exit(0);
            case 'm':
                major = atoi(optarg);
                break;
            case 'n':
                minor = atoi(optarg);
                break;
            default:
                usage();
                exit(1);
        }
    }

    argc -= optind;
    argv += optind;

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }

    if (major > 0)
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, major);

    if (minor > 0)
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, minor);

    // We assume here that we stand a better chance of success by leaving all
    // possible details of pixel format selection to GLFW

    if (!glfwOpenWindow(0, 0, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
    {
        glfwTerminate();

        fprintf(stderr, "Failed to open GLFW window\n");
        exit(1);
    }

    glfwGetVersion(&major, &minor, &revision);

    printf("GLFW header version: %u.%u.%u\n",
           GLFW_VERSION_MAJOR,
           GLFW_VERSION_MINOR,
           GLFW_VERSION_REVISION);

    printf("GLFW library version: %u.%u.%u\n", major, minor, revision);

    if (major != GLFW_VERSION_MAJOR ||
        minor != GLFW_VERSION_MINOR ||
        revision != GLFW_VERSION_REVISION)
        printf("*** WARNING: GLFW version mismatch! ***\n");

    glfwGetGLVersion(&major, &minor, &revision);

    printf("OpenGL context version parsed by GLFW: %u.%u.%u\n", major, minor, revision);

    printf("OpenGL context version string: \"%s\"\n", glGetString(GL_VERSION));
    printf("OpenGL context renderer string: \"%s\"\n", glGetString(GL_RENDERER));
    printf("OpenGL context vendor string: \"%s\"\n", glGetString(GL_VENDOR));

    printf("OpenGL context supported extensions:\n");

    extensions = glGetString(GL_EXTENSIONS);
    while (*extensions != '\0')
    {
        if (*extensions == ' ')
            putchar('\n');
        else
            putchar(*extensions);

        extensions++;
    }

    putchar('\n');

    glfwTerminate();
    exit(0);
}

