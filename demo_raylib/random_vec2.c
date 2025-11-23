#include "raylib.h"
#include "../better_lib/better_math.h"
#include "../better_lib/better_vec.h"

#define MAX_PARTICLES 50

typedef struct {
    Vec2 pos;
    Vec2 vel;
    float life;
    Color color;
} Particle;

int main(void) {
    InitWindow(800, 600, "better_vec + Raylib 2D Demo");
    SetTargetFPS(60);

    // Player
    Vec2 player_pos = vec2(400, 300);
    Vec2 player_vel = vec2_zero();
    float player_radius = 20.0f;
    float player_speed = 300.0f;
    float friction = 5.0f;

    // Orbiting shields
    float orbit_angle = 0.0f;
    float orbit_radius = 50.0f;
    int num_shields = 3;

    // Particles
    Particle particles[MAX_PARTICLES] = {0};
    int particle_idx = 0;

    // Target to chase player
    Vec2 enemy_pos = vec2(100, 100);
    float enemy_speed = 80.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* ── Input ────────────────────────────────────────────── */
        Vec2 input = vec2_zero();
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    input.y -= 1;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  input.y += 1;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  input.x -= 1;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) input.x += 1;

        // Normalize diagonal movement
        if (vec2_len_sq(input) > 0) {
            input = vec2_norm(input);
            player_vel = vec2_add(player_vel, vec2_scale(input, player_speed * dt));
        }

        /* ── Physics ──────────────────────────────────────────── */
        // Apply friction
        player_vel = vec2_scale(player_vel, 1.0f - friction * dt);

        // Update position
        player_pos = vec2_add(player_pos, vec2_scale(player_vel, dt));

        // Clamp to screen
        player_pos = vec2_clamp(
            player_pos,
            vec2(player_radius, player_radius),
            vec2(800 - player_radius, 600 - player_radius)
        );

        /* ── Enemy AI ─────────────────────────────────────────── */
        Vec2 to_player = vec2_sub(player_pos, enemy_pos);
        float dist = vec2_len(to_player);
        if (dist > 5.0f) {
            Vec2 dir = vec2_norm(to_player);
            enemy_pos = vec2_add(enemy_pos, vec2_scale(dir, enemy_speed * dt));
        }

        /* ── Orbiting Shields ─────────────────────────────────── */
        orbit_angle += 2.0f * dt;  // radians per second

        /* ── Particle Spawning (on movement) ──────────────────── */
        if (vec2_len(player_vel) > 20.0f) {
            Particle* p = &particles[particle_idx];
            p->pos = player_pos;
            p->vel = vec2_scale(vec2_neg(vec2_norm(player_vel)), 50.0f);
            // Add some spread using perpendicular
            Vec2 perp = vec2_perpendicular(vec2_norm(player_vel));
            p->vel = vec2_add(p->vel, vec2_scale(perp, (float)GetRandomValue(-30, 30)));
            p->life = 1.0f;
            p->color = BLUE;
            particle_idx = (particle_idx + 1) % MAX_PARTICLES;
        }

        /* ── Update Particles ─────────────────────────────────── */
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].life > 0) {
                particles[i].pos = vec2_add(
                    particles[i].pos,
                    vec2_scale(particles[i].vel, dt)
                );
                particles[i].life -= dt * 2.0f;
            }
        }

        /* ── Drawing ──────────────────────────────────────────── */
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Particles
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].life > 0) {
                float alpha = particles[i].life;
                float size = lerp(2.0f, 8.0f, alpha);
                Color c = Fade(particles[i].color, alpha);
                DrawCircle((int)particles[i].pos.x, (int)particles[i].pos.y, size, c);
            }
        }

        // Enemy
        DrawCircle((int)enemy_pos.x, (int)enemy_pos.y, 15, RED);
        // Draw direction arrow
        Vec2 enemy_dir = vec2_norm(to_player);
        Vec2 arrow_end = vec2_add(enemy_pos, vec2_scale(enemy_dir, 25));
        DrawLine(enemy_pos.x, enemy_pos.y, arrow_end.x, arrow_end.y, RED);

        // Player
        DrawCircle((int)player_pos.x, (int)player_pos.y, player_radius, BLUE);

        // Velocity indicator
        Vec2 vel_end = vec2_add(player_pos, vec2_scale(player_vel, 0.1f));
        DrawLine(player_pos.x, player_pos.y, vel_end.x, vel_end.y, DARKBLUE);

        // Orbiting shields
        for (int i = 0; i < num_shields; i++) {
            float angle = orbit_angle + (BV_PI * 2.0f / num_shields) * i;
            Vec2 shield_offset = vec2_from_angle(angle);
            Vec2 shield_pos = vec2_add(player_pos, vec2_scale(shield_offset, orbit_radius));
            DrawCircle((int)shield_pos.x, (int)shield_pos.y, 10, SKYBLUE);
        }

        // Distance to enemy
        DrawText(TextFormat("Distance: %.0f", dist), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Velocity: %.1f", vec2_len(player_vel)), 10, 35, 20, DARKGRAY);

        // Instructions
        DrawText("WASD / Arrow keys to move", 10, 560, 16, GRAY);
        DrawText("Red enemy chases you!", 10, 580, 16, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
