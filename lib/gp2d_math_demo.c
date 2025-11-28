#include "raylib.h"
#include "gp2d_math.h"

#define SCREEN_W 1024
#define SCREEN_H 768

// --- Demo 1: Vector Reflection State ---
typedef struct {
    gp2d_vec2 position;
    gp2d_vec2 velocity;
    float radius;
    gp2d_vec2 normal_p1;
    gp2d_vec2 normal_p2;
    gp2d_vec2 normal_vec;
} BallDemo;

// --- GLOBAL STATE VARIABLES (Replaces GetWindowData) ---
BallDemo ball_demo_state; 
gp2d_mat3 transform_matrix;
gp2d_bezier_segment path_segments[2];
float path_time = 0.0f;
float matrix_angle = 0.0f;
float matrix_scale = 1.0f;
float ease_time = 0.0f;
float ease_duration = 3.0f;
// --------------------------------------------------------

// Initialization function
void InitDemos(void) {
    // Demo 1: Vector Reflection
    ball_demo_state.position = gp2d_v2(100.0f, 100.0f);
    ball_demo_state.velocity = gp2d_v2(300.0f, 250.0f); // Pixels per second
    ball_demo_state.radius = 15.0f;

    // Define a "wall" segment
    ball_demo_state.normal_p1 = gp2d_v2(SCREEN_W - 400, 100);
    ball_demo_state.normal_p2 = gp2d_v2(SCREEN_W - 100, 400);

    // Calculate the normal vector
    gp2d_vec2 wall_direction = gp2d_v2_sub(ball_demo_state.normal_p2, ball_demo_state.normal_p1);
    ball_demo_state.normal_vec = gp2d_v2_norm(gp2d_v2_perp(wall_direction));

    // Demo 2: Bezier Path
    // Segment 1
    path_segments[0].points[0] = gp2d_v2(100, 400);
    path_segments[0].points[1] = gp2d_v2(300, 200);
    path_segments[0].points[2] = gp2d_v2(500, 500);
    path_segments[0].points[3] = gp2d_v2(600, 300);
    // Segment 2 (starts where segment 1 ends)
    path_segments[1].points[0] = path_segments[0].points[3];
    path_segments[1].points[1] = gp2d_v2(700, 100);
    path_segments[1].points[2] = gp2d_v2(900, 500);
    path_segments[1].points[3] = gp2d_v2(SCREEN_W - 100, SCREEN_H - 100);

    // Demo 3: Matrix Transform (Initial matrix is identity)
    transform_matrix = gp2d_m3_identity();
}

// Update function
void UpdateDemos(float dt, float global_time) {
    // --- Demo 1: Vector Reflection ---
    // Use pointer to the global state
    BallDemo *B = &ball_demo_state; 

    // Update position
    B->position = gp2d_v2_add(B->position, gp2d_v2_scale(B->velocity, dt));

    // Simple bounds reflection
    if (B->position.x - B->radius < 0 || B->position.x + B->radius > SCREEN_W) {
        B->velocity.x *= -1.0f;
        B->position.x = gp2d_clamp(B->position.x, B->radius, SCREEN_W - B->radius);
    }
    if (B->position.y - B->radius < 0 || B->position.y + B->radius > SCREEN_H) {
        B->velocity.y *= -1.0f;
        B->position.y = gp2d_clamp(B->position.y, B->radius, SCREEN_H - B->radius);
    }
    
    // Line reflection check 
    gp2d_vec2 normal_line_dir = gp2d_v2_sub(B->normal_p2, B->normal_p1);
    gp2d_vec2 normal_perp = gp2d_v2_norm(gp2d_v2_perp(normal_line_dir));
    float dist = gp2d_v2_dot(gp2d_v2_sub(B->position, B->normal_p1), normal_perp);
    
    if (dist <= B->radius && gp2d_v2_dot(B->velocity, normal_perp) < 0) {
        B->velocity = gp2d_v2_reflect(B->velocity, normal_perp);
        B->position = gp2d_v2_add(B->position, gp2d_v2_scale(normal_perp, B->radius - dist)); // Nudge out
    }


    // --- Demo 2: Bezier Path Following ---
    path_time += dt / 4.0f; 
    if (path_time > 1.0f) path_time = 0.0f;

    // --- Demo 3: Matrix Transform Update ---
    matrix_angle += dt * 90.0f * DEG2RAD; 
    matrix_scale = 1.0f + 0.2f * sinf(global_time * 2.0f); 

    // Chain the transformations: Scale -> Rotate -> Translate
    gp2d_mat3 M_S = gp2d_m3_scale(matrix_scale, matrix_scale);
    gp2d_mat3 M_R = gp2d_m3_rotate(matrix_angle);
    gp2d_mat3 M_T = gp2d_m3_translate(200, 600); 

    // M = T * R * S (Applied in order: S, then R, then T)
    gp2d_mat3 R_S = gp2d_m3_mul(M_R, M_S); 
    transform_matrix = gp2d_m3_mul(M_T, R_S); 

    // --- Demo 4: Easing Update ---
    ease_time = fmodf(global_time, ease_duration * 2.0f);
}

// Drawing function
void DrawDemos(void) {
    BallDemo *B = &ball_demo_state;
    
    // Text Labels
    DrawText("gp2d_math.h Raylib Demo", 10, 10, 20, DARKGRAY);
    DrawText("1. Vector Reflection (gp2d_v2_reflect, gp2d_v2_norm)", 10, 30, 15, WHITE);
    DrawText("2. Bezier Path Following (gp2d_path_sample, gp2d_v2_bezier_cubic)", 10, 250, 15, WHITE);
    DrawText("3. Matrix Transformation (gp2d_m3_mul, gp2d_m3_transform_point)", 10, 550, 15, WHITE);
    DrawText("4. Easing Function (gp2d_ease_out_elastic)", SCREEN_W/2 + 10, 550, 15, WHITE);

    // --- Demo 1: Vector Reflection Draw ---
    DrawLine(B->normal_p1.x, B->normal_p1.y, B->normal_p2.x, B->normal_p2.y, GREEN);
    DrawCircle(B->position.x, B->position.y, B->radius, RED);
    
    // Draw the normal vector at the wall segment midpoint
    gp2d_vec2 midpoint = gp2d_v2_lerp(B->normal_p1, B->normal_p2, 0.5f);
    gp2d_vec2 normal_end = gp2d_v2_add(midpoint, gp2d_v2_scale(B->normal_vec, 50.0f));
    DrawLineEx((Vector2){midpoint.x, midpoint.y}, (Vector2){normal_end.x, normal_end.y}, 2, BLUE);
    DrawText("Wall Normal", normal_end.x + 5, normal_end.y - 10, 10, BLUE);


    // --- Demo 2: Bezier Path Following Draw ---
    gp2d_vec2 path_p;
    gp2d_vec2 prev_p = path_segments[0].points[0];

    // Draw the path itself (many small segments)
    for (int i = 0; i <= 2 * 100; i++) {
        float t = (float)i / (2 * 100.0f);
        path_p = gp2d_path_sample(path_segments, 2, t);
        DrawLineV((Vector2){prev_p.x, prev_p.y}, (Vector2){path_p.x, path_p.y}, PURPLE);
        prev_p = path_p;
    }

    // Draw the object following the path
    gp2d_vec2 object_pos = gp2d_path_sample(path_segments, 2, path_time);
    DrawCircle(object_pos.x, object_pos.y, 10, YELLOW);
    

    // --- Demo 3: Matrix Transformation Draw ---
    gp2d_vec2 rect_center = gp2d_v2(0, 0);
    gp2d_vec2 rect_points[4] = {
        gp2d_v2(-50, -30), gp2d_v2(50, -30), gp2d_v2(50, 30), gp2d_v2(-50, 30)
    };
    
    gp2d_vec2 transformed_points[4];

    // Transform all points using the combined matrix
    for (int i = 0; i < 4; i++) {
        transformed_points[i] = gp2d_m3_transform_point(transform_matrix, rect_points[i]);
    }

    // Draw transformed rectangle
    DrawLineV((Vector2){transformed_points[0].x, transformed_points[0].y}, (Vector2){transformed_points[1].x, transformed_points[1].y}, ORANGE);
    DrawLineV((Vector2){transformed_points[1].x, transformed_points[1].y}, (Vector2){transformed_points[2].x, transformed_points[2].y}, ORANGE);
    DrawLineV((Vector2){transformed_points[2].x, transformed_points[2].y}, (Vector2){transformed_points[3].x, transformed_points[3].y}, ORANGE);
    DrawLineV((Vector2){transformed_points[3].x, transformed_points[3].y}, (Vector2){transformed_points[0].x, transformed_points[0].y}, ORANGE);
    
    // Draw center point
    gp2d_vec2 center_pos = gp2d_m3_transform_point(transform_matrix, rect_center);
    DrawCircle(center_pos.x, center_pos.y, 5, WHITE);


    // --- Demo 4: Easing Function Draw ---
    float t_norm = gp2d_clamp01(gp2d_time_normalize(ease_time, 0.0f, ease_duration));
    float t_ease = gp2d_ease_out_elastic(t_norm);
    
    float start_y = SCREEN_H - 100;
    float end_y = SCREEN_H - 300;
    
    // Linear (for comparison)
    // Need to check for loop to reverse motion after duration is passed
    if (ease_time > ease_duration) {
        t_norm = gp2d_clamp01(gp2d_time_normalize(ease_time, ease_duration, ease_duration * 2.0f));
        t_ease = 1.0f - gp2d_ease_out_elastic(t_norm);
        t_norm = 1.0f - t_norm;
    }
    
    float linear_y = start_y + (end_y - start_y) * t_norm;
    DrawText("Linear", SCREEN_W/2 + 20, linear_y, 15, GRAY);

    // Eased (your function)
    float eased_y = start_y + (end_y - start_y) * t_ease;
    DrawText("gp2d_ease_out_elastic", SCREEN_W/2 + 200, eased_y, 20, SKYBLUE);
}

// Main raylib loop
int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "gp2d_math.h Raylib Demo");
    SetTargetFPS(60);
    
    InitDemos();

    // Use raylib's main loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float global_time = GetTime();
        
        UpdateDemos(dt, global_time);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawDemos();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
