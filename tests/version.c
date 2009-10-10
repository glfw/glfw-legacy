
#include <GL/glfw.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  int major, minor, revision;

  if (!glfwInit())
    exit(1);

  if (!glfwOpenWindow(100, 100, 8, 8, 8, 0, 0, 0, GLFW_WINDOW))
    exit(1);

  glfwGetVersion(&major, &minor, &revision);

  printf("GLFW header version: %u.%u.%u\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);    
  printf("GLFW library version: %u.%u.%u\n", major, minor, revision);    

  if (major != GLFW_VERSION_MAJOR ||
      minor != GLFW_VERSION_MINOR ||
      revision != GLFW_VERSION_REVISION)
    printf("*** WARNING: GLFW version mismatch! ***\n");

  printf("OpenGL context version: %s\n", glGetString(GL_VERSION));    
  printf("OpenGL context renderer: %s\n", glGetString(GL_RENDERER));    
  printf("OpenGL context vendor: %s\n", glGetString(GL_VENDOR));    
  printf("OpenGL context extensions: %s\n", glGetString(GL_EXTENSIONS));    

  exit(0);
}

