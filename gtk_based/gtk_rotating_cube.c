// main.c
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>

// Global OpenGL objects
static GLuint shader_program = 0;
static GLuint vao = 0, vbo = 0, ebo = 0;

// Cube vertices (position + color)
static float vertices[] = {
    // positions          // colors
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f,  0.5f, -0.5f, -0.5f, 0.0f,
    1.0f,  0.0f,  0.5f,  0.5f, -0.5f, 0.0f,  0.0f, 1.0f,  -0.5f, 0.5f,
    -0.5f, 1.0f,  1.0f,  0.0f, -0.5f, -0.5f, 0.5f, 1.0f,  0.0f,  1.0f,
    0.5f,  -0.5f, 0.5f,  0.0f, 1.0f,  1.0f,  0.5f, 0.5f,  0.5f,  1.0f,
    1.0f,  1.0f,  -0.5f, 0.5f, 0.5f,  0.5f,  0.5f, 0.5f,
};

static unsigned int indices[] = {
    0, 1, 2, 2, 3, 0, // back
    4, 5, 6, 6, 7, 4, // front
    0, 1, 5, 5, 4, 0, // bottom
    3, 2, 6, 6, 7, 3, // top
    0, 3, 7, 7, 4, 0, // left
    1, 2, 6, 6, 5, 1  // right
};

static const char *vertex_src = "#version 460 core\n"
                                "layout (location = 0) in vec3 aPos;\n"
                                "layout (location = 1) in vec3 aColor;\n"
                                "out vec3 fragColor;\n"
                                "uniform mat4 mvp;\n"
                                "void main() {\n"
                                "   gl_Position = mvp * vec4(aPos, 1.0);\n"
                                "   fragColor = aColor;\n"
                                "}\n";

static const char *fragment_src =
    "#version 460 core\n"
    "in vec3 fragColor;\n"
    "out vec4 color;\n"
    "void main() { color = vec4(fragColor, 1.0); }\n";

static GLuint compile_shader(GLenum type, const char *src) {
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &src, NULL);
  glCompileShader(s);
  GLint ok;
  glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetShaderInfoLog(s, 512, NULL, log);
    g_error("Shader error: %s", log);
  }
  return s;
}

static void realize_cb(GtkWidget *widget) {
  gtk_gl_area_make_current(GTK_GL_AREA(widget));

  if (!gladLoadGL())
    g_error("Failed to load OpenGL");

  printf("OpenGL %s\n", glGetString(GL_VERSION));

  // Shaders
  GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_src);
  GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
  shader_program = glCreateProgram();
  glAttachShader(shader_program, vs);
  glAttachShader(shader_program, fs);
  glLinkProgram(shader_program);
  glDeleteShader(vs);
  glDeleteShader(fs);

  // VAO/VBO/EBO
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  glEnable(GL_DEPTH_TEST);
}

static gboolean render_cb(GtkGLArea *area, GdkGLContext *context) {
  glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shader_program);
  glBindVertexArray(vao);

  // --- cglm magic starts here ---
  mat4 model = GLM_MAT4_IDENTITY_INIT;
  mat4 view = GLM_MAT4_IDENTITY_INIT;
  mat4 proj = GLM_MAT4_IDENTITY_INIT;

  // Model: rotate over time
  float time = (float)gtk_get_current_event_time() / 1000.0f;
  glm_rotate_x(model, time * glm_rad(45.0f), model);
  glm_rotate_y(model, time * glm_rad(45.0f), model);

  // View: camera 3 units back
  glm_translate(view, (vec3){0.0f, 0.0f, -3.0f});

  // Projection
  int w = gtk_widget_get_allocated_width(GTK_WIDGET(area));
  int h = gtk_widget_get_allocated_height(GTK_WIDGET(area));
  glm_perspective(glm_rad(60.0f), (float)w / (float)h, 0.1f, 100.0f, proj);

  // MVP
  mat4 mvp;
  glm_mat4_mul(proj, view, mvp);
  glm_mat4_mul(mvp, model, mvp);

  glUniformMatrix4fv(glGetUniformLocation(shader_program, "mvp"), 1, GL_FALSE,
                     (float *)mvp);
  // --- cglm magic ends ---

  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

  // Request next frame (for smooth animation)
  gtk_widget_queue_draw(GTK_WIDGET(area));

  return TRUE;
}

static void resize_cb(GtkGLArea *area, int width, int height) {
  glViewport(0, 0, width, height);
}

int main(int argc, char **argv) {
  gtk_init(&argc, &argv);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window),
                       "GTK3 + glad + cglm → Rotating Cube");
  gtk_window_set_default_size(GTK_WINDOW(window), 900, 700);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window), box);

  GtkWidget *label = gtk_label_new(
      "<big><b>Rotating 3D Cube using cglm inside GTK3 + OpenGL 4.6</b></big>");
  gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
  gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 10);

  GtkWidget *gl_area = gtk_gl_area_new();
  gtk_gl_area_set_required_version(GTK_GL_AREA(gl_area), 4, 6);
  gtk_gl_area_set_has_depth_buffer(GTK_GL_AREA(gl_area), TRUE);
  gtk_gl_area_set_auto_render(GTK_GL_AREA(gl_area),
                              FALSE); // we drive animation manually

  g_signal_connect(gl_area, "realize", G_CALLBACK(realize_cb), NULL);
  g_signal_connect(gl_area, "render", G_CALLBACK(render_cb), NULL);
  g_signal_connect(gl_area, "resize", G_CALLBACK(resize_cb), NULL);

  gtk_widget_set_hexpand(gl_area, TRUE);
  gtk_widget_set_vexpand(gl_area, TRUE);
  gtk_box_pack_start(GTK_BOX(box), gl_area, TRUE, TRUE, 0);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
