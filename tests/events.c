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

static const char* get_key_name(int key)
{
    switch (key)
    {
        case GLFW_KEY_UNKNOWN:      return "unknown";
        case GLFW_KEY_SPACE:        return "space";
        case GLFW_KEY_ESC:          return "escape";
        case GLFW_KEY_F1:           return "F1";
        case GLFW_KEY_F2:           return "F2";
        case GLFW_KEY_F3:           return "F3";
        case GLFW_KEY_F4:           return "F4";
        case GLFW_KEY_F5:           return "F5";
        case GLFW_KEY_F6:           return "F6";
        case GLFW_KEY_F7:           return "F7";
        case GLFW_KEY_F8:           return "F8";
        case GLFW_KEY_F9:           return "F9";
        case GLFW_KEY_F10:          return "F10";
        case GLFW_KEY_F11:          return "F11";
        case GLFW_KEY_F12:          return "F12";
        case GLFW_KEY_F13:          return "F13";
        case GLFW_KEY_F14:          return "F14";
        case GLFW_KEY_F15:          return "F15";
        case GLFW_KEY_F16:          return "F16";
        case GLFW_KEY_F17:          return "F17";
        case GLFW_KEY_F18:          return "F18";
        case GLFW_KEY_F19:          return "F19";
        case GLFW_KEY_F20:          return "F20";
        case GLFW_KEY_F21:          return "F21";
        case GLFW_KEY_F22:          return "F22";
        case GLFW_KEY_F23:          return "F23";
        case GLFW_KEY_F24:          return "F24";
        case GLFW_KEY_F25:          return "F25";
        case GLFW_KEY_UP:           return "up";
        case GLFW_KEY_DOWN:         return "down";
        case GLFW_KEY_LEFT:         return "left";
        case GLFW_KEY_RIGHT:        return "right";
        case GLFW_KEY_LSHIFT:       return "left shift";
        case GLFW_KEY_RSHIFT:       return "right shift";
        case GLFW_KEY_LCTRL:        return "left control";
        case GLFW_KEY_RCTRL:        return "right control";
        case GLFW_KEY_LALT:         return "left alt";
        case GLFW_KEY_RALT:         return "right alt";
        case GLFW_KEY_TAB:          return "tab";
        case GLFW_KEY_ENTER:        return "enter";
        case GLFW_KEY_BACKSPACE:    return "backspace";
        case GLFW_KEY_INSERT:       return "insert";
        case GLFW_KEY_DEL:          return "delete";
        case GLFW_KEY_PAGEUP:       return "page up";
        case GLFW_KEY_PAGEDOWN:     return "page down";
        case GLFW_KEY_HOME:         return "home";
        case GLFW_KEY_END:          return "end";
        case GLFW_KEY_KP_0:         return "keypad 0";
        case GLFW_KEY_KP_1:         return "keypad 1";
        case GLFW_KEY_KP_2:         return "keypad 2";
        case GLFW_KEY_KP_3:         return "keypad 3";
        case GLFW_KEY_KP_4:         return "keypad 4";
        case GLFW_KEY_KP_5:         return "keypad 5";
        case GLFW_KEY_KP_6:         return "keypad 6";
        case GLFW_KEY_KP_7:         return "keypad 7";
        case GLFW_KEY_KP_8:         return "keypad 8";
        case GLFW_KEY_KP_9:         return "keypad 9";
        case GLFW_KEY_KP_DIVIDE:    return "keypad divide";
        case GLFW_KEY_KP_MULTIPLY:  return "keypad multiply";
        case GLFW_KEY_KP_SUBTRACT:  return "keypad subtract";
        case GLFW_KEY_KP_ADD:       return "keypad add";
        case GLFW_KEY_KP_DECIMAL:   return "keypad decimal";
        case GLFW_KEY_KP_EQUAL:     return "keypad equal";
        case GLFW_KEY_KP_ENTER:     return "keypad enter";
        default:                    return NULL;
    }
}

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
    const char* name = get_key_name(key);
    if (name)
    {
        printf("%08x: Key 0x%04x (%s) was %s\n",
               counter++,
               key,
               name,
               get_action_name(action));
    }
    else
    {
        printf("%08x: Key 0x%04x was %s\n",
               counter++,
               key,
               get_action_name(action));
    }
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

