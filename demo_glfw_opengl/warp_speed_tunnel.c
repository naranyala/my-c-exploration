// warp_tunnel_fixed.c
// Compile: gcc warp_tunnel_fixed.c -o warp -lglfw -lGL -lGLEW -lm

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stddef.h> // offsetof
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define NUM_STARS 4096

typedef struct {
  float x, y, z; // position
  float speed;
  float hue; // for color variation
} Star;

Star stars[NUM_STARS];
GLuint shaderProgram, starVAO, starVBO;
float globalSpeed = 20.0f;
float timeAcc = 0.0f;

// ===================================================================
// Simple mat4 helper (only what we need)
typedef float mat4[16];
static void perspective(mat4 m, float fovy, float aspect, float near,
                        float far) {
  float tanHalf = tanf(fovy * 0.5f * M_PI / 180.0f);
  m[0] = 1.0f / (aspect * tanHalf);
  m[4] = 0;
  m[8] = 0;
  m[12] = 0;
  m[1] = 0;
  m[5] = 1.0f / tanHalf;
  m[9] = 0;
  m[13] = 0;
  m[2] = 0;
  m[6] = 0;
  m[10] = -(far + near) / (far - near);
  m[14] = -2.0f * far * near / (far - near);
  m[3] = 0;
  m[7] = 0;
  m[11] = -1;
  m[15] = 0;
}

// ===================================================================
// Shaders
const char *vertexSrc =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout(location = 1) in float aHue;\n"
    "uniform mat4 uMVP;\n"
    "uniform float uGlobalSpeed;\n"
    "out float vDepth;\n"
    "out float vHue;\n"
    "void main() {\n"
    "    vec4 p = uMVP * vec4(aPos, 1.0);\n"
    "    gl_Position = p;\n"
    "    vDepth = -aPos.z;\n" // distance from camera (user code expects this
                              // sign)
    "    vHue = aHue;\n"
    "    float size = 100.0 / (vDepth + 10.0);\n"
    "    gl_PointSize = clamp(size * (uGlobalSpeed/30.0 + 0.3), 1.0, 30.0);\n"
    "}\n";

const char *fragmentSrc =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in float vDepth;\n"
    "in float vHue;\n"
    "void main() {\n"
    "    vec2 coord = gl_PointCoord - vec2(0.5);\n"
    "    float dist = dot(coord, coord) * 4.0;\n"
    "    if (dist > 1.0) discard;\n"
    // make the falloff negative so alpha decreases with distance from center
    "    float alpha = exp(-dist * 6.0);\n"
    "    vec3 color = 0.5 + 0.5 * cos(6.28318*(vec3(0.0,0.33,0.67) + vHue));\n"
    "    float bright = clamp(vDepth / 80.0, 0.3, 2.0);\n"
    "    FragColor = vec4(color * bright, alpha);\n"
    "}\n";

// ===================================================================
GLuint compile(const char *src, GLenum type) {
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &src, NULL);
  glCompileShader(s);
  GLint ok;
  glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[1024];
    glGetShaderInfoLog(s, 1024, NULL, log);
    printf("Shader error:\n%s\n", log);
    exit(1);
  }
  return s;
}

void init() {
  glewExperimental = GL_TRUE;
  glewInit();

  GLuint vs = compile(vertexSrc, GL_VERTEX_SHADER);
  GLuint fs = compile(fragmentSrc, GL_FRAGMENT_SHADER);
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);

  // link check
  GLint ok;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
  if (!ok) {
    char log[1024];
    glGetProgramInfoLog(shaderProgram, 1024, NULL, log);
    printf("Program link error:\n%s\n", log);
    exit(1);
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  glUseProgram(shaderProgram);

  glGenVertexArrays(1, &starVAO);
  glGenBuffers(1, &starVBO);

  glBindVertexArray(starVAO);
  glBindBuffer(GL_ARRAY_BUFFER, starVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Star) * NUM_STARS, NULL,
               GL_DYNAMIC_DRAW);

  // position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Star), (void *)0);
  // hue (offset within Star)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Star),
                        (void *)offsetof(Star, hue));

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  // additive blending to get bright streaks like stars
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  // seed stars
  srand((unsigned)time(NULL));
  for (int i = 0; i < NUM_STARS; i++) {
    float angle = rand() / (float)RAND_MAX * M_PI * 2.0f;
    float radius = (rand() / (float)RAND_MAX) * 6.0f + 0.2f;
    stars[i].x = cosf(angle) * radius;
    stars[i].y = sinf(angle) * radius;
    // NOTE: in this code the vertex shader treats aPos.z positively and uses
    // -aPos.z as depth; keeping positive z values is fine for the original
    // author's logic.
    stars[i].z = rand() / (float)RAND_MAX * 120.0f + 20.0f;
    stars[i].speed = 0.5f + rand() / (float)RAND_MAX * 1.2f;
    stars[i].hue = rand() / (float)RAND_MAX;
  }
}

void update(float dt) {
  // slowly accelerate to give the sensation of speeding up
  globalSpeed += dt * 15.0f;
  if (globalSpeed > 200.0f)
    globalSpeed = 200.0f;

  for (int i = 0; i < NUM_STARS; i++) {
    stars[i].z -= stars[i].speed * globalSpeed * dt;
    if (stars[i].z < 0.5f) {
      float angle = rand() / (float)RAND_MAX * M_PI * 2.0f;
      float radius = (rand() / (float)RAND_MAX) * 6.0f + 0.2f;
      stars[i].x = cosf(angle) * radius;
      stars[i].y = sinf(angle) * radius;
      stars[i].z = 140.0f;
      stars[i].speed = 0.5f + rand() / (float)RAND_MAX * 1.4f;
      stars[i].hue = rand() / (float)RAND_MAX;
    }
  }
}

void draw(GLFWwindow *window) {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  glViewport(0, 0, w, h);
  glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  mat4 proj;
  perspective(proj, 90.0f, (float)w / (float)h, 0.5f, 300.0f);

  glUseProgram(shaderProgram);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uMVP"), 1, GL_FALSE,
                     proj);
  glUniform1f(glGetUniformLocation(shaderProgram, "uGlobalSpeed"), globalSpeed);

  // upload star positions
  glBindBuffer(GL_ARRAY_BUFFER, starVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Star) * NUM_STARS, stars);

  glBindVertexArray(starVAO);
  glDrawArrays(GL_POINTS, 0, NUM_STARS);
}

void key_callback(GLFWwindow *w, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(w, 1);
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    globalSpeed = 20.0f;
}

int main() {
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *win = glfwCreateWindow(
      WINDOW_WIDTH, WINDOW_HEIGHT, "Hyperspace Warp Tunnel – Fixed & Beautiful",
      NULL, NULL);
  if (!win) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);
  glfwSetKeyCallback(win, key_callback);

  init();

  double last = glfwGetTime();
  while (!glfwWindowShouldClose(win)) {
    double now = glfwGetTime();
    float dt = (float)(now - last);
    last = now;
    timeAcc += dt;

    update(dt);
    draw(win);

    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}
