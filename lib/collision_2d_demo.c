// main.c - Ultimate tinyphys_2d v2 + collision.h v2 + raylib demo
// No merging. Perfect stacking. Looks like a real engine!

#include "raylib.h"
#include "tinyphys_2d.h"   // ← your new fixed version
#include "collision.h"     // ← your new fixed version (for static walls/floor)

#define MAX_BODIES 400

// Draw body
void DrawBody(const tp_body* b)
{
    Color col = (b->type == TP_CIRCLE) ? BLUE : MAROON;
    if (b->sleeping) col = Fade(col, 0.5f);

    if (b->type == TP_CIRCLE) {
        DrawCircle((int)b->x, (int)b->y, b->radius, col);
        DrawCircleLines((int)b->x, (int)b->y, b->radius, DARKBLUE);
        DrawLine((int)b->x, (int)b->y,
                 (int)(b->x + b->vx * 5), (int)(b->y + b->vy * 5), RED);
    } else {
        Rectangle r = { b->x - b->w*0.5f, b->y - b->h*0.5f, b->w, b->h };
        DrawRectangleRec(r, col);
        DrawRectangleLinesEx(r, 2, DARKPURPLE);
    }
}

// Draw static walls using collision.h shapes
void DrawStatic(const col_shape* s, Color c)
{
    if (s->type == COL_AABB) {
        Rectangle r = { s->x - s->w*0.5f, s->y - s->h*0.5f, s->w, s->h };
        DrawRectangleRec(r, c);
        DrawRectangleLinesEx(r, 4, BLACK);
    }
}

int main(void)
{
    const int W = 1280;
    const int H = 720;
    InitWindow(W, H, "tinyphys_2d v2 + collision.h v2 — Perfect Physics Demo");
    SetTargetFPS(60);

    // Physics world
    tp_body bodies[MAX_BODIES];
    tp_world world;
    tp_world_init(&world, bodies, MAX_BODIES, 1.0f/60.0f);
    world.gy = 1200.0f;  // strong gravity = nice stacking

    // Static walls (using collision.h)
    col_shape walls[4] = {0};
    // Floor
    walls[0].type = COL_AABB;
    walls[0].x = W/2; walls[0].y = H - 40;
    walls[0].w = W + 200; walls[0].h = 80;
    // Left wall
    walls[1].type = COL_AABB;
    walls[1].x = -40; walls[1].y = H/2;
    walls[1].w = 80; walls[1].h = H + 200;
    // Right wall
    walls[2].type = COL_AABB;
    walls[2].x = W + 40; walls[2].y = H/2;
    walls[2].w = 80; walls[2].h = H + 200;
    // Ceiling (optional)
    walls[3].type = COL_AABB;
    walls[3].x = W/2; walls[3].y = -40;
    walls[3].w = W + 200; walls[3].h = 80;

    // Mouse spawning
    Vector2 mouse = {0};
    bool mouseDown = false;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        mouse = GetMousePosition();

        // Spawn on mouse hold (drag to rain!)
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            if (!mouseDown || GetTime() - mouseDown > 0.05)
            {
                mouseDown = true;
                if (world.count < MAX_BODIES - 5)
                {
                    tp_body* b = tp_world_add(&world);
                    b->x = mouse.x + GetRandomValue(-20, 20);
                    b->y = mouse.y + GetRandomValue(-20, 20);

                    bool isCircle = GetRandomValue(0, 100) < 60;

                    if (isCircle) {
                        b->type = TP_CIRCLE;
                        b->radius = GetRandomValue(12, 35);
                        tp_set_mass(b, b->radius * b->radius * 0.08f);
                        b->restitution = 0.35f + GetRandomValue(0, 40)*0.01f;
                        b->friction = 0.15f;
                    } else {
                        b->type = TP_AABB;
                        b->w = GetRandomValue(25, 70);
                        b->h = GetRandomValue(25, 70);
                        tp_set_mass(b, b->w * b->h * 0.04f);
                        b->restitution = 0.1f;
                        b->friction = 0.4f;
                    }

                    // Give a little random velocity
                    b->vx = GetRandomValue(-200, 200);
                    b->vy = GetRandomValue(-100, 100);
                }
            }
        } else {
            mouseDown = false;
        }

        // Clear screen on C
        if (IsKeyPressed(KEY_C)) {
            world.count = 0;
        }

        // === COLLISION WITH STATIC WALLS ===
        for (int i = 0; i < world.count; i++) {
            tp_body* b = &world.bodies[i];
            col_shape shape = { .x = b->x, .y = b->y, .r = b->radius, .w = b->w, .h = b->h, .type = (b->type == TP_CIRCLE) ? COL_CIRCLE : COL_AABB };

            for (int w = 0; w < 4; w++) {
                col_manifold m = {0};
                if (col_intersect(&shape, &walls[w], &m)) {
                    // Positional correction
                    float correction = (m.depth - 0.01f) * 0.8f;
                    if (correction > 0) {
                        b->x += m.nx * correction;
                        b->y += m.ny * correction;
                    }

                    // Impulse (wall has infinite mass)
                    float rv = b->vx * m.nx + b->vy * m.ny;
                    if (rv < 0) {
                        float e = b->restitution * 0.8f;
                        float j = -(1 + e) * rv;
                        b->vx += m.nx * j;
                        b->vy += m.ny * j;

                        // Friction
                        float tx = -m.ny, ty = m.nx;
                        float vt = b->vx * tx + b->vy * ty;
                        float jt = -vt * 0.5f;
                        float mu = 0.6f;
                        if (jt > mu * j) jt = mu * j;
                        if (jt < -mu * j) jt = -mu * j;
                        b->vx += tx * jt;
                        b->vy += ty * jt;
                    }
                }
            }
        }

        // === PHYSICS STEP ===
        tp_world_step(&world);

        // === RENDERING ===
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw static walls
        DrawStatic(&walls[0], DARKGRAY);  // floor
        DrawStatic(&walls[1], GRAY);
 brux        DrawStatic(&walls[2], GRAY);
        DrawStatic(&walls[3], LIGHTGRAY);

        // Draw bodies
        for (int i = 0; i < world.count; i++)
            DrawBody(&world.bodies[i]);

        // UI
        DrawFPS(10, 10);
        DrawText("Hold LEFT MOUSE to spawn shapes", 10, 40, 20, DARKGRAY);
        DrawText("Press C to clear", 10, 70, 20, DARKGRAY);
        DrawText(TextFormat("Bodies: %d", world.count), 10, 100, 20, DARKGREEN);
        DrawText("tinyphys_2d v2 + collision.h v2 — NO MERGING!", 10, H - 30, 20, DARKBLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
