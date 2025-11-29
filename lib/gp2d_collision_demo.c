#include "gp2d_collision.h" // Your collision library
#include "gp2d_math.h"      // Required for gp2d_vec2 and math helpers
#include "raylib.h"

#define SCREEN_W 1024
#define SCREEN_H 768

// State for the Circle Collision Demo
gp2d_circle circle_A; // Mouse controlled
gp2d_circle circle_B; // Stationary/draggable

// State for the Raycasting Demo
gp2d_aabb box_static;
gp2d_circle circle_static;

// Initial setup
void InitDemo(void) {
  // --- Circle Collision Init ---
  circle_A = gp2d_circle_make(gp2d_v2(300, 300), 50.0f);
  circle_B = gp2d_circle_make(gp2d_v2(550, 300), 75.0f);

  // --- Raycasting Init ---
  gp2d_vec2 box_size = gp2d_v2(200, 100);
  box_static = gp2d_aabb_from_center(gp2d_v2(SCREEN_W - 200, 200),
                                     gp2d_v2_scale(box_size, 0.5f));

  circle_static =
      gp2d_circle_make(gp2d_v2(SCREEN_W - 200, SCREEN_H - 150), 60.0f);
}

// Update logic
void UpdateDemo(float dt) {
  Vector2 mouse_pos = GetMousePosition();

  // --- Circle Collision Update ---

  // 1. Move Circle A to the mouse position
  circle_A.center = gp2d_v2(mouse_pos.x, mouse_pos.y);

  // 2. Resolve collision and separation (gp2d_separate_circles is the key
  // function here) NOTE: This modifies the centers of both circles to push them
  // apart.
  gp2d_separate_circles(&circle_A, &circle_B);

  // Add drag functionality to move Circle B with right-click
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    circle_B.center = gp2d_v2(mouse_pos.x, mouse_pos.y);
  }
}

// Drawing logic
void DrawDemo(void) {
  // Get mouse position for raycasting
  Vector2 mouse_pos = GetMousePosition();
  gp2d_vec2 mouse_v2 = gp2d_v2(mouse_pos.x, mouse_pos.y);

  DrawText("gp2d_collision.h Raylib Demo", 10, 10, 20, DARKGRAY);
  DrawLine(0, SCREEN_H / 2, SCREEN_W, SCREEN_H / 2, DARKGRAY);
  DrawText("TOP HALF: Circle-to-Circle Separation (Mouse moves Circle A, "
           "Right-Click moves Circle B)",
           10, 40, 15, WHITE);
  DrawText("BOTTOM HALF: Raycasting (Mouse controls ray direction/origin)", 10,
           SCREEN_H / 2 + 20, 15, WHITE);

  // =========================================================================
  // --- TOP HALF: Circle Collision Draw ---
  // =========================================================================

  // Check if collision is active to change color
  gp2d_collision_result col_res = gp2d_circle_collision(circle_A, circle_B);
  Color color_A = col_res.hit ? RED : SKYBLUE;
  Color color_B = col_res.hit ? RED : GREEN;

  // Draw Circle A
  DrawCircle(circle_A.center.x, circle_A.center.y, circle_A.radius,
             Fade(color_A, 0.7f));
  DrawCircleLines(circle_A.center.x, circle_A.center.y, circle_A.radius,
                  color_A);
  DrawText("Circle A (Mouse)", circle_A.center.x - 40, circle_A.center.y, 10,
           WHITE);

  // Draw Circle B
  DrawCircle(circle_B.center.x, circle_B.center.y, circle_B.radius,
             Fade(color_B, 0.7f));
  DrawCircleLines(circle_B.center.x, circle_B.center.y, circle_B.radius,
                  color_B);
  DrawText("Circle B (Hold R-Click)", circle_B.center.x - 60, circle_B.center.y,
           10, WHITE);

  // Draw collision information
  if (col_res.hit) {
    DrawText(TextFormat("COLLISION! Depth: %.2f", col_res.depth), 10, 70, 20,
             RED);

    // Draw Normal Vector (Visualization of gp2d_collision_result.normal)
    gp2d_vec2 midpoint = gp2d_v2_lerp(circle_A.center, circle_B.center, 0.5f);
    gp2d_vec2 normal_end =
        gp2d_v2_add(midpoint, gp2d_v2_scale(col_res.normal, 80.0f));
    DrawLineEx((Vector2){midpoint.x, midpoint.y},
               (Vector2){normal_end.x, normal_end.y}, 3, YELLOW);
    DrawText("Normal", normal_end.x + 5, normal_end.y, 10, YELLOW);
  }

  // =========================================================================
  // --- BOTTOM HALF: Raycasting Draw ---
  // =========================================================================

  // --- Raycast 1: Ray vs AABB ---

  // Setup ray: Starts at mouse, points to AABB
  gp2d_vec2 ray1_origin = mouse_v2;
  gp2d_vec2 ray1_target = gp2d_aabb_center(box_static);
  gp2d_vec2 ray1_dir = gp2d_v2_norm(gp2d_v2_sub(ray1_target, ray1_origin));
  gp2d_ray ray1 = gp2d_ray_make(ray1_origin, ray1_dir);

  gp2d_raycast_result res_aabb = gp2d_raycast_aabb(ray1, box_static);

  // Draw AABB
  DrawRectangleLines(
      box_static.min.x, box_static.min.y, box_static.max.x - box_static.min.x,
      box_static.max.y - box_static.min.y, res_aabb.hit ? RED : GRAY);

  // Draw Ray
  Color ray1_color = res_aabb.hit ? RED : LIME;
  gp2d_vec2 ray1_end =
      res_aabb.hit ? res_aabb.point
                   : gp2d_v2_add(ray1_origin, gp2d_v2_scale(ray1_dir, 1000.0f));
  DrawLineEx(mouse_pos, (Vector2){ray1_end.x, ray1_end.y}, 2, ray1_color);

  // Draw hit point and normal
  if (res_aabb.hit) {
    DrawCircleV((Vector2){res_aabb.point.x, res_aabb.point.y}, 8, PURPLE);
    gp2d_vec2 normal_end =
        gp2d_v2_add(res_aabb.point, gp2d_v2_scale(res_aabb.normal, 50.0f));
    DrawLineEx((Vector2){res_aabb.point.x, res_aabb.point.y},
               (Vector2){normal_end.x, normal_end.y}, 3, PURPLE);
    DrawText(TextFormat("AABB Hit! Dist: %.0f", res_aabb.distance),
             res_aabb.point.x, res_aabb.point.y - 30, 12, PURPLE);
  }

  // --- Raycast 2: Ray vs Circle ---

  // Setup ray: Origin is fixed, direction controlled by mouse
  gp2d_vec2 ray2_origin = gp2d_v2(SCREEN_W / 2, SCREEN_H - 100);
  gp2d_vec2 ray2_dir = gp2d_v2_norm(gp2d_v2_sub(mouse_v2, ray2_origin));
  gp2d_ray ray2 = gp2d_ray_make(ray2_origin, ray2_dir);

  gp2d_raycast_result res_circle = gp2d_raycast_circle(ray2, circle_static);

  // Draw Static Circle
  DrawCircle(circle_static.center.x, circle_static.center.y,
             circle_static.radius, Fade(res_circle.hit ? GOLD : BLUE, 0.3f));
  DrawCircleLines(circle_static.center.x, circle_static.center.y,
                  circle_static.radius, res_circle.hit ? GOLD : BLUE);

  // Draw Ray
  Color ray2_color = res_circle.hit ? GOLD : LIME;
  gp2d_vec2 ray2_end =
      res_circle.hit
          ? res_circle.point
          : gp2d_v2_add(ray2_origin, gp2d_v2_scale(ray2_dir, 500.0f));
  DrawLineEx((Vector2){ray2_origin.x, ray2_origin.y},
             (Vector2){ray2_end.x, ray2_end.y}, 2, ray2_color);

  // Draw hit point and normal
  if (res_circle.hit) {
    DrawCircleV((Vector2){res_circle.point.x, res_circle.point.y}, 8, GOLD);
    gp2d_vec2 normal_end =
        gp2d_v2_add(res_circle.point, gp2d_v2_scale(res_circle.normal, 50.0f));
    DrawLineEx((Vector2){res_circle.point.x, res_circle.point.y},
               (Vector2){normal_end.x, normal_end.y}, 3, GOLD);
    DrawText(TextFormat("Circle Hit! Dist: %.0f", res_circle.distance),
             res_circle.point.x, res_circle.point.y - 30, 12, GOLD);
  }
}

// Main raylib loop
int main(void) {
  InitWindow(SCREEN_W, SCREEN_H, "gp2d_collision.h Raylib Demo");
  SetTargetFPS(60);

  InitDemo();

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    UpdateDemo(dt);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawDemo();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
