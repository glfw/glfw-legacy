
#include <GL/glfw.h>

#include <stdio.h>
#include <stdlib.h>

static void mouse_button(int button, int pressed)
{
  if (button == 0)
  {
    if (pressed)
      glfwDisable(GLFW_MOUSE_CURSOR);
    else
      glfwEnable(GLFW_MOUSE_CURSOR);
  }
}

static void mouse_position(int x, int y)
{
  printf("%i %i\n", x, y);
}

static void resize(int width, int height)
{
  glViewport(0, 0, width, height);
}

int main(void)
{
  if (!glfwInit())
    exit(1);

  if (!glfwOpenWindow(640, 480, 8, 8, 8, 0, 0, 0, GLFW_WINDOW))
    exit(1);

  glfwSetWindowTitle("Peter Detector");
  glfwDisable(GLFW_AUTO_POLL_EVENTS);
  glfwSetWindowSizeCallback(resize);
  glfwSetMousePosCallback(mouse_position);
  glfwSetMouseButtonCallback(mouse_button);

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

