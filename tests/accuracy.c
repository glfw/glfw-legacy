
#include <GL/glfw.h>

#include <stdlib.h>

static int cursor_x = 0, cursor_y = 0;
static int window_width = 640, window_height = 480;

static void size_callback(int width, int height)
{
  window_width = width;
  window_height = height;

  glViewport(0, 0, window_width, window_height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.f, window_width, 0.f, window_height);
}

static void cursor_callback(int x, int y)
{
  cursor_x = x;
  cursor_y = y;
}

int main()
{
  if (!glfwInit())
    exit(1);

  if (glfwOpenWindow(window_width, window_height, 8, 8, 8, 0, 0, 0, GLFW_WINDOW))
  {
    glfwSetWindowTitle("Cursor accuracy");
    glfwSetMousePosCallback(cursor_callback);
    glfwSetWindowSizeCallback(size_callback);

    glClearColor(0, 0, 0, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    while (glfwGetWindowParam(GLFW_OPENED))
    {
      glClear(GL_COLOR_BUFFER_BIT);

      glColor3f(1.f, 1.f, 1.f);

      glBegin(GL_LINES);
      glVertex2f(0.f, window_height - cursor_y);
      glVertex2f(window_width, window_height - cursor_y);
      glVertex2f(cursor_x, 0.f);
      glVertex2f(cursor_x, window_height);
      glEnd();

      glfwSwapBuffers();
    }
  }
  else
    exit(1);

  glfwTerminate();
}

