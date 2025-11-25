// shapes2d_demo.c
#define SHAPES2D_IMPLEMENTATION
#include "shapes2d.h"

#include "raylib.h"
#include <stdio.h>

// Demo configuration
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define TARGET_FPS 60

// Demo state
typedef struct {
    Shape2D* selected_shape;
    Point2D drag_start;
    bool is_dragging;
    Color drag_color;
} DemoState;

// Helper functions
Color GetShapeColor(ShapeType type);
void DrawShape2D(Shape2D* shape, Color color);
Vector2 Point2DtoVector2(Point2D p);
Point2D Vector2ToPoint2D(Vector2 v);
void CreateDemoShapes(Shape2D* shapes[], int count);

int main() {
    // Initialize window
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "shapes2d.h Raylib Demo");
    SetTargetFPS(TARGET_FPS);
    
    // Create demo shapes
    const int SHAPE_COUNT = 5;
    Shape2D* shapes[SHAPE_COUNT];
    CreateDemoShapes(shapes, SHAPE_COUNT);
    
    // Initialize demo state
    DemoState state = {0};
    state.selected_shape = NULL;
    state.is_dragging = false;
    state.drag_color = RED;
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Handle input
        Vector2 mouse = GetMousePosition();
        Point2D mouse_point = Vector2ToPoint2D(mouse);
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Check for shape selection
            for (int i = 0; i < SHAPE_COUNT; i++) {
                if (is_point_in_shape(shapes[i], mouse_point)) {
                    state.selected_shape = shapes[i];
                    state.drag_start = mouse_point;
                    state.is_dragging = true;
                    break;
                }
            }
        }
        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && state.is_dragging && state.selected_shape) {
            // Calculate offset
            Point2D current = Vector2ToPoint2D(mouse);
            double dx = current.x - state.drag_start.x;
            double dy = current.y - state.drag_start.y;
            
            // Move shape
            switch (state.selected_shape->type) {
                case SHAPE_POINT: {
                    Point2D* p = (Point2D*)state.selected_shape->data;
                    p->x += dx;
                    p->y += dy;
                    break;
                }
                case SHAPE_CIRCLE: {
                    CircleData* c = (CircleData*)state.selected_shape->data;
                    c->center.x += dx;
                    c->center.y += dy;
                    break;
                }
                case SHAPE_RECTANGLE: {
                    RectangleData* r = (RectangleData*)state.selected_shape->data;
                    r->bottom_left.x += dx;
                    r->bottom_left.y += dy;
                    break;
                }
                case SHAPE_TRIANGLE: {
                    TriangleData* t = (TriangleData*)state.selected_shape->data;
                    t->a.x += dx; t->a.y += dy;
                    t->b.x += dx; t->b.y += dy;
                    t->c.x += dx; t->c.y += dy;
                    break;
                }
                case SHAPE_LINE: {
                    LineData* l = (LineData*)state.selected_shape->data;
                    l->start.x += dx; l->start.y += dy;
                    l->end.x += dx; l->end.y += dy;
                    break;
                }
            }
            state.drag_start = current;
        }
        
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            state.is_dragging = false;
        }
        
        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw grid
        for (int i = 0; i < WINDOW_WIDTH; i += 50) {
            DrawLine(i, 0, i, WINDOW_HEIGHT, Fade(LIGHTGRAY, 0.5f));
        }
        for (int i = 0; i < WINDOW_HEIGHT; i += 50) {
            DrawLine(0, i, WINDOW_WIDTH, i, Fade(LIGHTGRAY, 0.5f));
        }
        
        // Draw axes
        DrawLine(WINDOW_WIDTH/2, 0, WINDOW_WIDTH/2, WINDOW_HEIGHT, GRAY);
        DrawLine(0, WINDOW_HEIGHT/2, WINDOW_WIDTH, WINDOW_HEIGHT/2, GRAY);
        
        // Draw shapes
        for (int i = 0; i < SHAPE_COUNT; i++) {
            Color color = GetShapeColor(shapes[i]->type);
            if (shapes[i] == state.selected_shape) {
                color = state.drag_color;
            }
            DrawShape2D(shapes[i], color);
        }
        
        // Draw UI info
        DrawText("shapes2d.h Raylib Demo", 10, 10, 20, DARKGRAY);
        DrawText("Click and drag shapes to move them", 10, 40, 16, DARKGRAY);
        DrawText("Selected shape turns red", 10, 65, 16, DARKGRAY);
        
        // Draw properties of selected shape
        if (state.selected_shape) {
            char info[256];
            double area = shape_area(state.selected_shape);
            double perimeter = shape_perimeter(state.selected_shape);
            
            snprintf(info, sizeof(info), 
                "Selected: %s\nArea: %.2f\nPerimeter: %.2f",
                state.selected_shape->type == SHAPE_CIRCLE ? "Circle" :
                state.selected_shape->type == SHAPE_RECTANGLE ? "Rectangle" :
                state.selected_shape->type == SHAPE_TRIANGLE ? "Triangle" :
                state.selected_shape->type == SHAPE_POINT ? "Point" : "Line",
                area, perimeter
            );
            
            DrawText(info, 10, 100, 16, MAROON);
        }
        
        EndDrawing();
    }
    
    // Cleanup
    for (int i = 0; i < SHAPE_COUNT; i++) {
        destroy_shape(shapes[i]);
    }
    
    CloseWindow();
    return 0;
}

// Helper implementations
Color GetShapeColor(ShapeType type) {
    switch (type) {
        case SHAPE_CIRCLE: return BLUE;
        case SHAPE_RECTANGLE: return GREEN;
        case SHAPE_TRIANGLE: return ORANGE;
        case SHAPE_POINT: return PURPLE;
        case SHAPE_LINE: return BROWN;
        default: return DARKGRAY;
    }
}

void DrawShape2D(Shape2D* shape, Color color) {
    if (!shape || !shape->data) return;
    
    switch (shape->type) {
        case SHAPE_POINT: {
            Point2D* p = (Point2D*)shape->data;
            Vector2 pos = Point2DtoVector2(*p);
            DrawCircleV(pos, 5, color);
            break;
        }
        case SHAPE_CIRCLE: {
            CircleData* c = (CircleData*)shape->data;
            Vector2 center = Point2DtoVector2(c->center);
            DrawCircleV(center, (float)c->radius, Fade(color, 0.3f));
            DrawCircleLinesV(center, (float)c->radius, color);
            break;
        }
        case SHAPE_RECTANGLE: {
            RectangleData* r = (RectangleData*)shape->data;
            Vector2 pos = Point2DtoVector2(r->bottom_left);
            // Raylib uses top-left origin, we use bottom-left
            float rectY = WINDOW_HEIGHT - pos.y - (float)r->height;
            Rectangle rect = { pos.x, rectY, (float)r->width, (float)r->height };
            DrawRectangleRec(rect, Fade(color, 0.3f));
            DrawRectangleLinesEx(rect, 2, color);
            break;
        }
        case SHAPE_TRIANGLE: {
            TriangleData* t = (TriangleData*)shape->data;
            Vector2 v1 = Point2DtoVector2(t->a);
            Vector2 v2 = Point2DtoVector2(t->b);
            Vector2 v3 = Point2DtoVector2(t->c);
            DrawTriangle(v1, v2, v3, Fade(color, 0.3f));
            DrawTriangleLines(v1, v2, v3, color);
            break;
        }
        case SHAPE_LINE: {
            LineData* l = (LineData*)shape->data;
            Vector2 start = Point2DtoVector2(l->start);
            Vector2 end = Point2DtoVector2(l->end);
            DrawLineEx(start, end, 3, color);
            break;
        }
    }
}

Vector2 Point2DtoVector2(Point2D p) {
    // Convert coordinate system: 
    // shapes2d uses Cartesian (Y-up)
    // Raylib uses screen (Y-down)
    return (Vector2){ 
        (float)(p.x + WINDOW_WIDTH/2), 
        (float)(WINDOW_HEIGHT/2 - p.y) 
    };
}

Point2D Vector2ToPoint2D(Vector2 v) {
    return (Point2D){ 
        v.x - WINDOW_WIDTH/2, 
        WINDOW_HEIGHT/2 - v.y 
    };
}

void CreateDemoShapes(Shape2D* shapes[], int count) {
    // Circle at origin
    shapes[0] = create_circle(make_point(0, 0), 50);
    
    // Rectangle
    shapes[1] = create_rectangle(make_point(-200, -100), 150, 100);
    
    // Triangle
    shapes[2] = create_triangle(
        make_point(200, -50),
        make_point(250, 50),
        make_point(150, 50)
    );
    
    // Point
    shapes[3] = create_point(make_point(-150, 150));
    
    // Line
    shapes[4] = create_line(
        make_point(0, 200),
        make_point(100, 100)
    );
}
