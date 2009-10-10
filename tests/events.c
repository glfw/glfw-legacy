//========================================================================
// Event linter (event spewer)
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
// This test hooks every available callback and outputs their arguments
//
// Every event also gets a (sequential) number to aid discussion of logs
//
//========================================================================

#include <GL/glfw.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static unsigned int counter = 0;

static const char* get_action_name(int action)
{
    switch (action)
    {
        case GLFW_PRESS:
            return "pressed";
        case GLFW_RELEASE:
            return "released";
        default:
            return "caused unknown action";
    }
}

static void window_size_callback(int width, int height)
{
    printf("%08x: Window size: %i %i\n", counter++, width, height);
    glViewport(0, 0, width, height);
}

static int window_close_callback(void)
{
    printf("%08x: Window close\n", counter++);
    return 1;
}

static void window_refresh_callback(void)
{
    printf("%08x: Window refresh\n", counter++);
}

static void mouse_button_callback(int button, int action)
{
    printf("%08x: Mouse button %i was %s\n",
           counter++,
           button,
           get_action_name(action));
}

static void mouse_position_callback(int x, int y)
{
    printf("%08x: Mouse position: %i %i\n", counter++, x, y);
}

static void mouse_wheel_callback(int position)
{
    printf("%08x: Mouse wheel: %i\n", counter++, position);
}

static void key_callback(int key, int action)
{
    printf("%08x: Key 0x%04x was %s\n",
           counter++,
           key,
           get_action_name(action));
}

static void char_callback(int character, int action)
{
    if (isgraph(character))
    {
        printf("%08x: Character 0x%04x (%c) was %s\n",
               counter++,
               character,
               character,
               get_action_name(action));
    }
    else
    {
        printf("%08x: Character 0x%04x was %s\n",
               counter++,
               character,
               get_action_name(action));
    }
}

int main(void)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }

    printf("Library initialized\n");

    if (!glfwOpenWindow(640, 480, 8, 8, 8, 0, 0, 0, GLFW_WINDOW))
    {
        glfwTerminate();

        fprintf(stderr, "Failed to create GLFW window");
        exit(1);
    }

    printf("Window opened\n");

    glfwSetWindowTitle("Event Linter");
    glfwSwapInterval(1);
    glfwDisable(GLFW_AUTO_POLL_EVENTS);

    glfwSetWindowSizeCallback(window_size_callback);
    glfwSetWindowCloseCallback(window_close_callback);
    glfwSetWindowRefreshCallback(window_refresh_callback);
    glfwSetMouseButtonCallback(mouse_button_callback);
    glfwSetMousePosCallback(mouse_position_callback);
    glfwSetMouseWheelCallback(mouse_wheel_callback);
    glfwSetKeyCallback(key_callback);
    glfwSetCharCallback(char_callback);

    glClearColor(0.f, 0.f, 0.f, 0.f);

    printf("Main loop starting\n");

    while (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers();
        glfwWaitEvents();
    }

    glfwTerminate();
    exit(0);
}

