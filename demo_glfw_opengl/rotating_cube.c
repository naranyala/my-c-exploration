
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static void error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// Fixed perspective projection matrix
static void myPerspective(double fovy, double aspect, double zNear,
                          double zFar) {
  double f = 1.0 / tan(fovy * M_PI / 360.0);
  GLfloat m[16] = {(GLfloat)(f / aspect),
                   0,
                   0,
                   0,
                   0,
                   (GLfloat)f,
                   0,
                   0,
                   0,
                   0,
                   (GLfloat)((zFar + zNear) / (zNear - zFar)),
                   -1,
                   0,
                   0,
                   (GLfloat)((2 * zFar * zNear) / (zNear - zFar)),
                   0};
  glMultMatrixf(m);
}

// Fixed lookAt function
static void myLookAt(double eyeX, double eyeY, double eyeZ, double centerX,
                     double centerY, double centerZ, double upX, double upY,
                     double upZ) {
  double forward[3] = {centerX - eyeX, centerY - eyeY, centerZ - eyeZ};
  double len = sqrt(forward[0] * forward[0] + forward[1] * forward[1] +
                    forward[2] * forward[2]);
  if (len > 0) {
    forward[0] /= len;
    forward[1] /= len;
    forward[2] /= len;
  }

  double side[3] = {forward[1] * upZ - forward[2] * upY,
                    forward[2] * upX - forward[0] * upZ,
                    forward[0] * upY - forward[1] * upX};

  len = sqrt(side[0] * side[0] + side[1] * side[1] + side[2] * side[2]);
  if (len > 0) {
    side[0] /= len;
    side[1] /= len;
    side[2] /= len;
  }

  double up[3] = {side[1] * forward[2] - side[2] * forward[1],
                  side[2] * forward[0] - side[0] * forward[2],
                  side[0] * forward[1] - side[1] * forward[0]};

  GLfloat m[16] = {(GLfloat)side[0],
                   (GLfloat)up[0],
                   (GLfloat)-forward[0],
                   0,
                   (GLfloat)side[1],
                   (GLfloat)up[1],
                   (GLfloat)-forward[1],
                   0,
                   (GLfloat)side[2],
                   (GLfloat)up[2],
                   (GLfloat)-forward[2],
                   0,
                   0,
                   0,
                   0,
                   1};

  glMultMatrixf(m);
  glTranslated(-eyeX, -eyeY, -eyeZ);
}

void drawCube(void) {
  glBegin(GL_QUADS);

  // Front face (red)
  glColor3f(1, 0, 0);
  glNormal3f(0, 0, 1);
  glVertex3f(-1, -1, 1);
  glVertex3f(1, -1, 1);
  glVertex3f(1, 1, 1);
  glVertex3f(-1, 1, 1);

  // Back face (green)
  glColor3f(0, 1, 0);
  glNormal3f(0, 0, -1);
  glVertex3f(-1, -1, -1);
  glVertex3f(-1, 1, -1);
  glVertex3f(1, 1, -1);
  glVertex3f(1, -1, -1);

  // Top face (blue)
  glColor3f(0, 0, 1);
  glNormal3f(0, 1, 0);
  glVertex3f(-1, 1, -1);
  glVertex3f(-1, 1, 1);
  glVertex3f(1, 1, 1);
  glVertex3f(1, 1, -1);

  // Bottom face (yellow)
  glColor3f(1, 1, 0);
  glNormal3f(0, -1, 0);
  glVertex3f(-1, -1, -1);
  glVertex3f(1, -1, -1);
  glVertex3f(1, -1, 1);
  glVertex3f(-1, -1, 1);

  // Right face (magenta)
  glColor3f(1, 0, 1);
  glNormal3f(1, 0, 0);
  glVertex3f(1, -1, -1);
  glVertex3f(1, 1, -1);
  glVertex3f(1, 1, 1);
  glVertex3f(1, -1, 1);

  // Left face (cyan)
  glColor3f(0, 1, 1);
  glNormal3f(-1, 0, 0);
  glVertex3f(-1, -1, -1);
  glVertex3f(-1, -1, 1);
  glVertex3f(-1, 1, 1);
  glVertex3f(-1, 1, -1);

  glEnd();
}

int main(void) {
  GLFWwindow *window;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rotating Cube", NULL,
                            NULL);
  if (!window) {
    fprintf(stderr, "Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Set clear color and clear the screen
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  // Lighting setup
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  // Light properties
  GLfloat lightPos[] = {2.0f, 2.0f, 5.0f, 1.0f};
  GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};

  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

  // Material properties
  GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat matShininess[] = {50.0f};
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

  printf("OpenGL version: %s\n", glGetString(GL_VERSION));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));

  while (!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    myPerspective(45.0, ratio, 0.1, 100.0);

    // Modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera position - moved slightly further back
    myLookAt(0, 0, 8, 0, 0, 0, 0, 1, 0);

    // Rotation animation
    double t = glfwGetTime();
    glRotatef((float)t * 30.0f, 1, 0, 0);
    glRotatef((float)t * 50.0f, 0, 1, 0);

    drawCube();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
