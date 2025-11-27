
/*
README - Minimal GLB/GLTF Viewer in C

Files contained in this single-file demo:
 - main.c            (this file)
 - extern/cgltf.h    (require: https://github.com/jkuhlmann/cgltf)
 - extern/cgltf.c    (compile with project or link as source)
 - extern/stb_image.h (for texture loading)
 - extern/glad.c/glad.h (or use your preferred GL loader)

Dependencies (Linux example):
 - GLFW (glfw3)
 - opengl (libGL)
 - libm, dl

Build (Linux):
 1) Put cgltf.c and cgltf.h alongside this file (or in extern/)
 2) Put stb_image.h next to the project (or in extern/)
 3) Ensure you have a GL loader (glad) in the include path and compiled if needed.
 4) Example compile:
    gcc main.c cgltf.c glad.c -I. -Iextern -o gltf_viewer -lglfw -ldl -lm -lGL

What the viewer does (minimal):
 - Loads a .glb or .gltf using cgltf (C single-header loader).
 - Uploads mesh POSITION, NORMAL, TEXCOORD_0 to OpenGL buffers.
 - Loads baseColorTexture (if present) via stb_image.
 - Renders using a simple Blinn-Phong shader (not full PBR).
 - Supports orbit camera (mouse drag) and zoom (scroll).

Limitations:
 - No skinning, no morph targets, limited material support (only baseColorTexture + factors).
 - Only the first mesh/primitive is loaded (for simplicity). Extend by iterating.
 - No scene graph / node transforms applied beyond root node's matrix if present.

------------------
Single-file demo source starts here.
------------------
*/

// NOTE: This file assumes you have the following headers available in your project:
// - cgltf.h / cgltf.c (https://github.com/jkuhlmann/cgltf)
// - stb_image.h (https://github.com/nothings/stb)
// - glad loader (or any OpenGL function loader)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GLFW/glfw3.h>
// include your GL loader header (glad or gl3w) BEFORE GLFW if needed
// #include "glad.h"

// Include cgltf and stb_image headers in your project path
#include "cgltf.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ----------------------- Simple shader helpers -----------------------
static GLuint compile_shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[1024];
        glGetShaderInfoLog(s, sizeof(buf), NULL, buf);
        fprintf(stderr, "Shader compile error: %s\n", buf);
        return 0;
    }
    return s;
}

static GLuint link_program(GLuint vs, GLuint fs) {
    GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[1024];
        glGetProgramInfoLog(p, sizeof(buf), NULL, buf);
        fprintf(stderr, "Program link error: %s\n", buf);
        return 0;
    }
    return p;
}

// Basic vertex shader (positions, normals, uvs)
static const char *vs_src = "#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"layout(location=1) in vec3 aNormal;\n"
"layout(location=2) in vec2 aUV;\n"
"uniform mat4 uModel;\n"
"uniform mat4 uView;\n"
"uniform mat4 uProj;\n"
"out vec3 vNormal;\n"
"out vec3 vPos;\n"
"out vec2 vUV;\n"
"void main(){\n"
"    vec4 world = uModel * vec4(aPos,1.0);\n"
"    vPos = world.xyz;\n"
"    vNormal = mat3(transpose(inverse(uModel))) * aNormal;\n"
"    vUV = aUV;\n"
"    gl_Position = uProj * uView * world;\n"
"}\n";

// Basic fragment shader: Blinn-Phong + baseColorTexture
static const char *fs_src = "#version 330 core\n"
"in vec3 vNormal;\n"
"in vec3 vPos;\n"
"in vec2 vUV;\n"
"out vec4 fragColor;\n"
"uniform vec3 uCamPos;\n"
"uniform vec3 uLightPos;\n"
"uniform vec3 uBaseColorFactor;\n"
"uniform float uMetallic;\n"
"uniform float uRoughness;\n"
"uniform sampler2D uBaseColorTex;\n"
"uniform int uHasBaseColorTex;\n"
"void main(){\n"
"    vec3 N = normalize(vNormal);\n"
"    vec3 L = normalize(uLightPos - vPos);\n"
"    vec3 V = normalize(uCamPos - vPos);\n"
"    vec3 H = normalize(L+V);\n"
"    float diff = max(dot(N, L), 0.0);\n"
"    float spec = pow(max(dot(N,H), 0.0), 32.0);\n"
"    vec3 base = uBaseColorFactor;\n"
"    if(uHasBaseColorTex==1) base *= texture(uBaseColorTex, vUV).rgb;\n"
"    vec3 color = base * diff + vec3(1.0)*spec*0.2;\n"
"    fragColor = vec4(color, 1.0);\n"
"}\n";

// ----------------------- Simple math / camera -----------------------
typedef struct { float x,y,z; } v3;

static void v3_sub(const v3 *a, const v3 *b, v3 *out){ out->x=a->x-b->x; out->y=a->y-b->y; out->z=a->z-b->z; }
static void v3_add(const v3 *a, const v3 *b, v3 *out){ out->x=a->x+b->x; out->y=a->y+b->y; out->z=a->z+b->z; }
static void v3_scale(v3 *a, float s){ a->x*=s; a->y*=s; a->z*=s; }

static void mat4_identity(float *m){ memset(m,0,16*sizeof(float)); m[0]=m[5]=m[10]=m[15]=1.0f; }
static void mat4_mul(const float *a,const float *b,float *out){
    float t[16];
    for(int r=0;r<4;r++) for(int c=0;c<4;c++){
        t[r*4+c]=0; for(int k=0;k<4;k++) t[r*4+c]+=a[r*4+k]*b[k*4+c];
    }
    memcpy(out,t,16*sizeof(float));
}

static void mat4_translate(float *m, float x,float y,float z){ mat4_identity(m); m[12]=x; m[13]=y; m[14]=z; }

static void mat4_perspective(float *m, float fovy, float aspect, float zn, float zf){
    float f = 1.0f/tan(fovy*0.5f);
    memset(m,0,16*sizeof(float));
    m[0]=f/aspect; m[5]=f; m[10]=(zf+zn)/(zn-zf); m[11]=-1; m[14]=(2*zf*zn)/(zn-zf);
}

static void mat4_lookat(float *m, v3 eye, v3 center, v3 up){
    v3 f; v3_sub(&center,&eye,&f); float fn = sqrtf(f.x*f.x+f.y*f.y+f.z*f.z); f.x/=fn; f.y/=fn; f.z/=fn;
    v3 s; s.x = f.y*up.z - f.z*up.y; s.y = f.z*up.x - f.x*up.z; s.z = f.x*up.y - f.y*up.x; float sn = sqrtf(s.x*s.x+s.y*s.y+s.z*s.z); s.x/=sn; s.y/=sn; s.z/=sn;
    v3 u; u.x = s.y*f.z - s.z*f.y; u.y = s.z*f.x - s.x*f.z; u.z = s.x*f.y - s.y*f.x;
    mat4_identity(m);
    m[0]=s.x; m[4]=s.y; m[8]=s.z;
    m[1]=u.x; m[5]=u.y; m[9]=u.z;
    m[2]=-f.x; m[6]=-f.y; m[10]=-f.z;
    m[12]=-(s.x*eye.x + s.y*eye.y + s.z*eye.z);
    m[13]=-(u.x*eye.x + u.y*eye.y + u.z*eye.z);
    m[14]= (f.x*eye.x + f.y*eye.y + f.z*eye.z);
}

// ----------------------- GLTF loading & buffer upload -----------------------
typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    size_t index_count;
    GLuint texture;
    float baseColorFactor[3];
    int hasTexture;
} MeshGL;

static int load_first_mesh_to_gl(const char *path, MeshGL *out){
    cgltf_options options = {0};
    cgltf_data *data = NULL;
    cgltf_result res = cgltf_parse_file(&options, path, &data);
    if(res != cgltf_result_success){ fprintf(stderr, "Failed to parse glTF: %d\n", res); return 0; }
    res = cgltf_load_buffers(&options, data, path);
    if(res != cgltf_result_success){ fprintf(stderr, "Failed to load buffers: %d\n", res); cgltf_free(data); return 0; }

    // For simplicity, pick the first mesh and first primitive
    if(data->meshes_count==0){ fprintf(stderr, "No meshes in file\n"); cgltf_free(data); return 0; }
    cgltf_mesh *mesh = &data->meshes[0];
    if(mesh->primitives_count==0){ fprintf(stderr, "Mesh has no primitives\n"); cgltf_free(data); return 0; }
    cgltf_primitive *prim = &mesh->primitives[0];

    // Find accessors for POSITION, NORMAL, TEXCOORD_0
    cgltf_accessor *pos_acc = NULL; cgltf_accessor *norm_acc = NULL; cgltf_accessor *uv_acc = NULL;
    for(size_t i=0;i<prim->attributes_count;i++){
        cgltf_attribute *attr = &prim->attributes[i];
        if(attr->type == cgltf_attribute_type_position) pos_acc = attr->data;
        if(attr->type == cgltf_attribute_type_normal) norm_acc = attr->data;
        if(attr->type == cgltf_attribute_type_texcoord) uv_acc = attr->data;
    }
    if(!pos_acc){ fprintf(stderr, "Primitive has no POSITION\n"); cgltf_free(data); return 0; }

    // Read vertex data into interleaved array
    size_t vertex_count = pos_acc->count;
    float *vertices = malloc(vertex_count * (3+3+2) * sizeof(float));
    for(size_t i=0;i<vertex_count;i++){
        float px=0,py=0,pz=0; float nx=0,ny=0,nz=0; float u=0,v=0;
        cgltf_accessor_read_float(pos_acc, i, &px, 3);
        if(norm_acc) cgltf_accessor_read_float(norm_acc, i, &nx, 3);
        if(uv_acc) cgltf_accessor_read_float(uv_acc, i, &u, 2);
        vertices[i*8+0]=px; vertices[i*8+1]=py; vertices[i*8+2]=pz;
        vertices[i*8+3]=nx; vertices[i*8+4]=ny; vertices[i*8+5]=nz;
        vertices[i*8+6]=u;  vertices[i*8+7]=v;
    }

    // Indices
    unsigned int *indices = NULL; size_t index_count = 0;
    if(prim->indices){
        cgltf_accessor *ia = prim->indices;
        index_count = ia->count;
        indices = malloc(index_count * sizeof(unsigned int));
        for(size_t i=0;i<index_count;i++){
            uint32_t id=0; cgltf_accessor_read_uint(ia, i, &id, 1);
            indices[i]=id;
        }
    }

    // Material baseColor
    out->hasTexture = 0; out->texture = 0; out->baseColorFactor[0]=out->baseColorFactor[1]=out->baseColorFactor[2]=1.0f;
    if(prim->material){
        cgltf_material *mat = prim->material;
        if(mat->has_pbr_metallic_roughness){
            out->baseColorFactor[0] = mat->pbr_metallic_roughness.base_color_factor[0];
            out->baseColorFactor[1] = mat->pbr_metallic_roughness.base_color_factor[1];
            out->baseColorFactor[2] = mat->pbr_metallic_roughness.base_color_factor[2];
            if(mat->pbr_metallic_roughness.base_color_texture.texture){
                cgltf_texture *t = mat->pbr_metallic_roughness.base_color_texture.texture;
                if(t->image && t->image->uri){
                    // Load image from memory or URI
                    int w,h,n;
                    unsigned char *img = stbi_load(t->image->uri, &w, &h, &n, 4);
                    if(img){
                        GLuint tex; glGenTextures(1,&tex); glBindTexture(GL_TEXTURE_2D, tex);
                        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,img);
                        glGenerateMipmap(GL_TEXTURE_2D);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        stbi_image_free(img);
                        out->texture = tex; out->hasTexture = 1;
                    } else {
                        fprintf(stderr, "Failed to load texture image: %s\n", t->image->uri);
                    }
                }
            }
        }
    }

    // OpenGL upload
    GLuint vao,vbo,ebo; glGenVertexArrays(1,&vao); glBindVertexArray(vao);
    glGenBuffers(1,&vbo); glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count*8*sizeof(float), vertices, GL_STATIC_DRAW);
    if(indices){ glGenBuffers(1,&ebo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count*sizeof(unsigned int), indices, GL_STATIC_DRAW); }
    // attribs
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(0));
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));

    out->vao = vao; out->vbo = vbo; out->ebo = ebo; out->index_count = index_count ? index_count : vertex_count;

    // cleanup
    free(vertices); if(indices) free(indices);
    cgltf_free(data);
    return 1;
}

// ----------------------- Application & input -----------------------
static double lastX=0,lastY=0; static int left_down=0; static float yaw=0.0f,pitch=0.0f; static float distance_cam=3.0f;

static void mouse_button_cb(GLFWwindow *w, int button, int action, int mods){
    if(button==GLFW_MOUSE_BUTTON_LEFT){ left_down = (action==GLFW_PRESS); }
}
static void cursor_pos_cb(GLFWwindow *w, double x, double y){
    if(left_down){ float dx = (float)(x-lastX); float dy = (float)(y-lastY); yaw += dx*0.01f; pitch += dy*0.01f; }
    lastX = x; lastY = y;
}
static void scroll_cb(GLFWwindow *w, double x, double y){ distance_cam *= (1.0f - (float)y*0.1f); if(distance_cam<0.1f) distance_cam=0.1f; }

int main(int argc, char **argv){
    if(argc<2){ printf("Usage: %s model.glb|gltf\n", argv[0]); return 1; }
    const char *path = argv[1];

    if(!glfwInit()){ fprintf(stderr, "Failed to init GLFW\n"); return 1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *win = glfwCreateWindow(1024,768,"GLB/GLTF Viewer",NULL,NULL);
    if(!win){ fprintf(stderr, "Failed to create window\n"); glfwTerminate(); return 1; }
    glfwMakeContextCurrent(win);
    // Initialize GL loader (glad) here if you use it
    glfwSetMouseButtonCallback(win, mouse_button_cb);
    glfwSetCursorPosCallback(win, cursor_pos_cb);
    glfwSetScrollCallback(win, scroll_cb);

    MeshGL mesh = {0};
    if(!load_first_mesh_to_gl(path, &mesh)){ fprintf(stderr, "Failed to load mesh\n"); return 1; }

    GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);
    GLuint prog = link_program(vs, fs);

    glEnable(GL_DEPTH_TEST);
    while(!glfwWindowShouldClose(win)){
        int w,h; glfwGetFramebufferSize(win,&w,&h);
        glViewport(0,0,w,h);
        glClearColor(0.2f,0.2f,0.25f,1.0f); glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // camera
        v3 camPos = { distance_cam * sinf(yaw)*cosf(pitch), distance_cam * sinf(pitch), distance_cam * cosf(yaw)*cosf(pitch) };
        v3 center = {0,0,0}; v3 up = {0,1,0};
        float view[16]; mat4_lookat(view, camPos, center, up);
        float proj[16]; mat4_perspective(proj, 45.0f*(3.14159265f/180.0f), (float)w/(float)h, 0.1f, 100.0f);
        float model[16]; mat4_identity(model);

        glUseProgram(prog);
        GLint locModel = glGetUniformLocation(prog, "uModel"); glUniformMatrix4fv(locModel,1,GL_FALSE,model);
        GLint locView = glGetUniformLocation(prog, "uView"); glUniformMatrix4fv(locView,1,GL_FALSE,view);
        GLint locProj = glGetUniformLocation(prog, "uProj"); glUniformMatrix4fv(locProj,1,GL_FALSE,proj);
        GLint locCam = glGetUniformLocation(prog, "uCamPos"); glUniform3f(locCam, camPos.x, camPos.y, camPos.z);
        GLint locLight = glGetUniformLocation(prog, "uLightPos"); glUniform3f(locLight, 10.0f,10.0f,10.0f);
        GLint locBase = glGetUniformLocation(prog, "uBaseColorFactor"); glUniform3f(locBase, mesh.baseColorFactor[0], mesh.baseColorFactor[1], mesh.baseColorFactor[2]);
        GLint locHasTex = glGetUniformLocation(prog, "uHasBaseColorTex"); glUniform1i(locHasTex, mesh.hasTexture);
        if(mesh.hasTexture){ glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, mesh.texture); glUniform1i(glGetUniformLocation(prog,"uBaseColorTex"), 0); }

        glBindVertexArray(mesh.vao);
        if(mesh.ebo) glDrawElements(GL_TRIANGLES, (GLsizei)mesh.index_count, GL_UNSIGNED_INT, 0);
        else glDrawArrays(GL_TRIANGLES, 0, (GLsizei)mesh.index_count);

        glfwSwapBuffers(win); glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
