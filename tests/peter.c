//========================================================================
// Mouse cursor disabling (grabbing, locking) test
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
// This test came about as the result of bugs #1262764 and #1726540, both
// reported by the user peterpp, hence the name
//
// The utility of this test outside of the (now fixed) bugs is uncertain
//
//========================================================================

#include <GL/glfw.h>

#include <stdio.h>
#include <stdlib.h>

static void GLFWCALL mouse_button_callback(int button, int pressed)
{
    if (button == 0)
    {
        if (pressed)
            glfwDisable(GLFW_MOUSE_CURSOR);
        else
            glfwEnable(GLFW_MOUSE_CURSOR);
    }
}

static void GLFWCALL mouse_position_callback(int x, int y)
{
    printf("%i %i\n", x, y);
}

static void GLFWCALL window_size_callback(int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }

    if (!glfwOpenWindow(640, 480, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
    {
        glfwTerminate();

        fprintf(stderr, "Failed to open GLFW window\n");
        exit(1);
    }

    glfwSetWindowTitle("Peter Detector");
    glfwDisable(GLFW_AUTO_POLL_EVENTS);
    glfwSetWindowSizeCallback(window_size_callback);
    glfwSetMousePosCallback(mouse_position_callback);
    glfwSetMouseButtonCallback(mouse_button_callback);
    glfwSwapInterval(1);

    glClearColor(0.f, 0.f, 0.f, 0.f);

    while (glfwGetWindowParam(GLFW_OPENED))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers();
        glfwWaitEvents();
    }

    glfwTerminate();
    exit(0);
}

