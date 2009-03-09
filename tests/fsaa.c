
#include <GL/glfw.h>

#include <stdlib.h>

static int running = 1;

static void keyboard(int key, int pressed)
{
  if (key == GLFW_KEY_ESC && pressed)
    running = 0;
}

static void resized(int width, int height)
{
  glViewport(0, 0, width, height);
}

int main(void)
{
  if (!glfwInit())
    exit(1);

  glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);

  if (glfwOpenWindow(640, 480, 8, 8, 8, 0, 0, 0, GLFW_FULLSCREEN))
  {
    glfwSetWindowTitle("Aliasing Detector");
    glfwSetWindowSizeCallback(resized);
    glfwSetKeyCallback(keyboard);
    glfwSwapInterval(1);

    glClearColor(0.f, 0.f, 0.f, 0.f);

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.f, 1.f, 0.f, 1.f);

    while (running)
    {
      glClear(GL_COLOR_BUFFER_BIT);

      glLoadIdentity();
      glRotatef(glfwGetTime(), 0.f, 0.f, 1.f);

      glBegin(GL_QUADS);
      glColor3f(1.f, 1.f, 1.f);
      glVertex2f(-0.25f,  0.25f);
      glVertex2f( 0.25f,  0.25f);
      glVertex2f( 0.25f, -0.25f);
      glVertex2f(-0.25f, -0.25f);
      glEnd();

      glfwSwapBuffers();

      if (!glfwGetWindowParam(GLFW_OPENED))
	running = 0;
    }
  }
  else
    exit(1);

  glfwTerminate();
}

