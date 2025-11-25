// raylib_demo.c - Interactive 3D shapes visualization
// Compile: gcc raylib_demo.c -o demo -lraylib -lm
// Make sure shapes3d.h is in the same directory

#define SHAPES3D_IMPLEMENTATION
#include "shapes3d.h"
#include "raylib.h"
#include "raymath.h"
#include <string.h>

#define MAX_SHAPES 7

typedef struct {
    Shape3D* shape;
    Color color;
    Vector3 position;
} RenderShape;

// Convert Point3D to raylib Vector3
Vector3 point_to_vec3(Point3D p) {
    return (Vector3){p.x, p.y, p.z};
}

// Convert raylib Vector3 to Point3D
Point3D vec3_to_point(Vector3 v) {
    return make_point3d(v.x, v.y, v.z);
}

// Draw a 3D shape using raylib
void draw_shape3d(const Shape3D* shape, Color color) {
    if (!shape || !shape->data) return;
    
    switch (shape->type) {
        case SHAPE3D_SPHERE: {
            SphereData* s = (SphereData*)shape->data;
            DrawSphere(point_to_vec3(s->center), s->radius, color);
            DrawSphereWires(point_to_vec3(s->center), s->radius, 16, 16, BLACK);
            break;
        }
        case SHAPE3D_CUBE: {
            CubeData* c = (CubeData*)shape->data;
            DrawCube(point_to_vec3(c->center), c->width, c->height, c->depth, color);
            DrawCubeWires(point_to_vec3(c->center), c->width, c->height, c->depth, BLACK);
            break;
        }
        case SHAPE3D_CYLINDER: {
            CylinderData* cy = (CylinderData*)shape->data;
            Vector3 base = point_to_vec3(cy->base_center);
            Vector3 top = {base.x, base.y + cy->height, base.z};
            DrawCylinder(base, cy->radius, cy->radius, cy->height, 32, color);
            DrawCylinderWires(base, cy->radius, cy->radius, cy->height, 32, BLACK);
            break;
        }
        case SHAPE3D_CONE: {
            ConeData* co = (ConeData*)shape->data;
            Vector3 base = point_to_vec3(co->base_center);
            DrawCylinder(base, 0.0f, co->radius, co->height, 32, color);
            DrawCylinderWires(base, 0.0f, co->radius, co->height, 32, BLACK);
            break;
        }
        case SHAPE3D_PYRAMID: {
            PyramidData* p = (PyramidData*)shape->data;
            Vector3 center = point_to_vec3(p->base_center);
            
            // Draw pyramid using triangles
            float hw = p->base_width / 2.0f;
            float hd = p->base_depth / 2.0f;
            Vector3 apex = {center.x, center.y + p->height, center.z};
            Vector3 v1 = {center.x - hw, center.y, center.z - hd};
            Vector3 v2 = {center.x + hw, center.y, center.z - hd};
            Vector3 v3 = {center.x + hw, center.y, center.z + hd};
            Vector3 v4 = {center.x - hw, center.y, center.z + hd};
            
            // Base
            DrawTriangle3D(v1, v2, v3, color);
            DrawTriangle3D(v1, v3, v4, color);
            
            // Sides
            DrawTriangle3D(v1, v2, apex, color);
            DrawTriangle3D(v2, v3, apex, color);
            DrawTriangle3D(v3, v4, apex, color);
            DrawTriangle3D(v4, v1, apex, color);
            
            // Wireframe
            DrawLine3D(v1, v2, BLACK); DrawLine3D(v2, v3, BLACK);
            DrawLine3D(v3, v4, BLACK); DrawLine3D(v4, v1, BLACK);
            DrawLine3D(v1, apex, BLACK); DrawLine3D(v2, apex, BLACK);
            DrawLine3D(v3, apex, BLACK); DrawLine3D(v4, apex, BLACK);
            break;
        }
        case SHAPE3D_TETRAHEDRON: {
            TetrahedronData* t = (TetrahedronData*)shape->data;
            Vector3 a = point_to_vec3(t->a);
            Vector3 b = point_to_vec3(t->b);
            Vector3 c = point_to_vec3(t->c);
            Vector3 d = point_to_vec3(t->d);
            
            // Draw faces
            DrawTriangle3D(a, b, c, color);
            DrawTriangle3D(a, b, d, color);
            DrawTriangle3D(a, c, d, color);
            DrawTriangle3D(b, c, d, color);
            
            // Draw edges
            DrawLine3D(a, b, BLACK); DrawLine3D(a, c, BLACK);
            DrawLine3D(a, d, BLACK); DrawLine3D(b, c, BLACK);
            DrawLine3D(b, d, BLACK); DrawLine3D(c, d, BLACK);
            break;
        }
        case SHAPE3D_POINT: {
            Point3D* pt = (Point3D*)shape->data;
            DrawSphere(point_to_vec3(*pt), 0.1f, color);
            break;
        }
        default:
            break;
    }
}

int main(void) {
    const int screenWidth = 1200;
    const int screenHeight = 800;
    
    InitWindow(screenWidth, screenHeight, "3D Shapes Library Demo - Raylib");
    
    Camera3D camera = {0};
    camera.position = (Vector3){15.0f, 15.0f, 15.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    // Create all shape types
    RenderShape shapes[MAX_SHAPES] = {0};
    int current_shape = 0;
    
    // Sphere
    shapes[0].shape = create_sphere(make_point3d(-5, 2, 0), 1.5);
    shapes[0].color = RED;
    
    // Cube
    shapes[1].shape = create_cube(make_point3d(-2, 1.5, 0), 2.5, 3, 2.5);
    shapes[1].color = BLUE;
    
    // Cylinder
    shapes[2].shape = create_cylinder(make_point3d(2, 0, 0), 1.2, 3.5);
    shapes[2].color = GREEN;
    
    // Cone
    shapes[3].shape = create_cone(make_point3d(5, 0, 0), 1.5, 3.0);
    shapes[3].color = YELLOW;
    
    // Pyramid
    shapes[4].shape = create_pyramid(make_point3d(-5, 0, -5), 2.5, 2.5, 3.0);
    shapes[4].color = PURPLE;
    
    // Tetrahedron
    shapes[5].shape = create_tetrahedron(
        make_point3d(0, 0, -5),
        make_point3d(2, 0, -5),
        make_point3d(1, 0, -7),
        make_point3d(1, 3, -6)
    );
    shapes[5].color = ORANGE;
    
    // Point
    shapes[6].shape = create_point3d(make_point3d(5, 1, -5));
    shapes[6].color = MAGENTA;
    
    // Test point for collision detection
    Vector3 test_point_pos = {0, 2, 0};
    bool show_test_point = false;
    
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        // Update
        UpdateCamera(&camera, CAMERA_ORBITAL);
        
        // Toggle test point
        if (IsKeyPressed(KEY_SPACE)) {
            show_test_point = !show_test_point;
        }
        
        // Move test point
        if (show_test_point) {
            if (IsKeyDown(KEY_UP)) test_point_pos.y += 0.1f;
            if (IsKeyDown(KEY_DOWN)) test_point_pos.y -= 0.1f;
            if (IsKeyDown(KEY_LEFT)) test_point_pos.x -= 0.1f;
            if (IsKeyDown(KEY_RIGHT)) test_point_pos.x += 0.1f;
            if (IsKeyDown(KEY_W)) test_point_pos.z -= 0.1f;
            if (IsKeyDown(KEY_S)) test_point_pos.z += 0.1f;
        }
        
        // Change selected shape
        if (IsKeyPressed(KEY_TAB)) {
            current_shape = (current_shape + 1) % MAX_SHAPES;
        }
        
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        BeginMode3D(camera);
        
        // Draw grid
        DrawGrid(20, 1.0f);
        
        // Draw all shapes
        for (int i = 0; i < MAX_SHAPES; i++) {
            if (shapes[i].shape) {
                Color col = shapes[i].color;
                if (i == current_shape) {
                    col.a = 200; // Highlight selected
                } else {
                    col.a = 150;
                }
                draw_shape3d(shapes[i].shape, col);
            }
        }
        
        // Draw test point and check collisions
        if (show_test_point) {
            Point3D test_pt = vec3_to_point(test_point_pos);
            bool inside_any = false;
            
            for (int i = 0; i < MAX_SHAPES; i++) {
                if (shapes[i].shape && is_point_in_shape3d(shapes[i].shape, test_pt)) {
                    inside_any = true;
                    break;
                }
            }
            
            Color point_color = inside_any ? GREEN : RED;
            DrawSphere(test_point_pos, 0.2f, point_color);
            DrawSphereWires(test_point_pos, 0.2f, 8, 8, BLACK);
        }
        
        EndMode3D();
        
        // UI
        DrawRectangle(10, 10, 350, 280, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(10, 10, 350, 280, BLUE);
        
        DrawText("3D SHAPES LIBRARY DEMO", 20, 20, 20, DARKBLUE);
        DrawText("-----------------------------------", 20, 45, 20, DARKBLUE);
        
        // Current shape info
        if (shapes[current_shape].shape) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "Current: Shape %d", current_shape + 1);
            DrawText(buffer, 20, 70, 15, BLACK);
            
            double vol = shape3d_volume(shapes[current_shape].shape);
            double area = shape3d_surface_area(shapes[current_shape].shape);
            
            snprintf(buffer, sizeof(buffer), "Volume: %.2f", vol);
            DrawText(buffer, 20, 90, 15, BLACK);
            
            snprintf(buffer, sizeof(buffer), "Surface Area: %.2f", area);
            DrawText(buffer, 20, 110, 15, BLACK);
        }
        
        DrawText("Controls:", 20, 140, 15, DARKGRAY);
        DrawText("  TAB - Next shape", 20, 160, 12, BLACK);
        DrawText("  SPACE - Toggle test point", 20, 175, 12, BLACK);
        DrawText("  ARROWS - Move point (X/Y)", 20, 190, 12, BLACK);
        DrawText("  W/S - Move point (Z)", 20, 205, 12, BLACK);
        DrawText("  Mouse - Rotate camera", 20, 220, 12, BLACK);
        DrawText("  Scroll - Zoom", 20, 235, 12, BLACK);
        
        if (show_test_point) {
            Point3D test_pt = vec3_to_point(test_point_pos);
            bool inside = is_point_in_shape3d(shapes[current_shape].shape, test_pt);
            const char* status = inside ? "INSIDE" : "OUTSIDE";
            Color status_color = inside ? GREEN : RED;
            DrawText(status, 20, 255, 20, status_color);
        }
        
        DrawFPS(screenWidth - 100, 10);
        
        EndDrawing();
    }
    
    // Cleanup
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].shape) {
            destroy_shape3d(shapes[i].shape);
        }
    }
    
    CloseWindow();
    return 0;
}
