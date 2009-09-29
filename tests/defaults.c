
#include <GL/glfw.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  int param;
  char* name;
} Param;

static Param parameters[] =
{
  { GLFW_ACCELERATED, "accelerated" },
  { GLFW_RED_BITS, "red bits" },
  { GLFW_GREEN_BITS, "green bits" },
  { GLFW_BLUE_BITS, "blue bits" },
  { GLFW_ALPHA_BITS, "alpha bits" },
  { GLFW_DEPTH_BITS, "depth bits" },
  { GLFW_STENCIL_BITS, "stencil bits" },
  { GLFW_REFRESH_RATE, "refresh rate" },
  { GLFW_ACCUM_RED_BITS, "accum red bits" },
  { GLFW_ACCUM_GREEN_BITS, "accum green bits" },
  { GLFW_ACCUM_BLUE_BITS, "accum blue bits" },
  { GLFW_ACCUM_ALPHA_BITS, "accum alpha bits" },
  { GLFW_AUX_BUFFERS, "aux buffers" },
  { GLFW_STEREO, "stereo" },
  { GLFW_FSAA_SAMPLES, "FSAA samples" },
  { GLFW_OPENGL_VERSION_MAJOR, "OpenGL major" },
  { GLFW_OPENGL_VERSION_MINOR, "OpenGL minor" },
  { GLFW_OPENGL_FORWARD_COMPAT, "OpenGL forward compatible" },
  { GLFW_DEBUG_CONTEXT, "debug context" },
};

int main(int argc, char** argv)
{
  int i, width, height;

  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
    exit(1);
  }

  if (!glfwOpenWindow(0, 0, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
  {
    fprintf(stderr, "Failed to open GLFW default window\n");
    exit(1);
  }

  glfwGetWindowSize(&width, &height);

  printf("window size: %ix%i\n", width, height);

  for (i = 0;  (size_t) i < sizeof(parameters) / sizeof(parameters[0]);  i++)
  {
    printf("%s: %i\n", parameters[i].name, glfwGetWindowParam(parameters[i].param));
  }

  glfwCloseWindow();
  glfwTerminate();
  exit(0);
}

