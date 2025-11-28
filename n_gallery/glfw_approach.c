#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#include "tinyfiledialogs.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>

bool load_gltf_file(const char* filename) {
    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, filename, &data);
    
    if (result == cgltf_result_success) {
        result = cgltf_load_buffers(&options, data, filename);
    }
    
    if (result == cgltf_result_success) {
        // Process 3D data here
        printf("Loaded GLTF with %zu meshes\n", data->meshes_count);
        cgltf_free(data);
        return true;
    }
    
    return false;
}

int main() {
    // Initialize GLFW
    glfwInit();
    
    // Create window with OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Gallery", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewInit();
    
    // File dialog for 3D files
    char const * filters[] = {"*.glb", "*.gltf"};
    char const * filepath = tinyfd_openFileDialog(
        "Select 3D Model",
        "",
        2,
        filters,
        "3D Model Files",
        0
    );
    
    if (filepath) {
        if (load_gltf_file(filepath)) {
            printf("Successfully loaded 3D model: %s\n", filepath);
        }
    }
    
    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Render 3D model here
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
