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
// This test is a pale imitation of glxinfo(1)
//
// It dumps GLFW and OpenGL version information
//
//========================================================================

#include <GL/glfw.h>

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int major, minor, revision;
    const GLubyte* extensions;

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }

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

