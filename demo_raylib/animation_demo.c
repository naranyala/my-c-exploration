

#include <stdio.h>
#include <stdlib.h>
#define ANIM2D_IMPLEMENTATION
#include "../lib/anim2d.h"
#include "raylib.h"

#define SCREEN_W 1200
#define SCREEN_H 800

// Global state for different animation demos
typedef struct {
  anim_clip circle_clip;
  anim_clip bounce_clip;
  anim_clip color_clip;
  anim_clip lissajous_clip;
  anim_clip rotate_clip;
} demo_state;

// Initialize bouncing ball animation
void init_bounce_animation(anim_clip *clip) {
  anim_clip_reset(clip);
  anim_clip_config(clip, 2.0f, 1.0f, 0.0f, ANIM_PLAY_LOOP);

  // Position track (x, y)
  anim_track pos_track;
  anim_track_reset(&pos_track);
  anim_vec2 start = {200, 200};
  anim_vec2 mid = {200, 600};
  anim_vec2 end = {200, 200};

  anim_track_add_vec2(&pos_track, 0.0f, 1.0f, start, mid, anim_ease_inquad);
  anim_track_add_vec2(&pos_track, 1.0f, 2.0f, mid, end, anim_ease_outquad);
  anim_clip_add(clip, pos_track);

  // Scale track (squash and stretch)
  anim_track scale_track;
  anim_track_reset(&scale_track);
  anim_track_add_scalar(&scale_track, 0.0f, 0.9f, 1.0f, 1.0f, anim_ease_linear);
  anim_track_add_scalar(&scale_track, 0.9f, 1.0f, 1.0f, 0.6f, anim_ease_inquad);
  anim_track_add_scalar(&scale_track, 1.0f, 1.2f, 1.4f, 1.0f,
                        anim_ease_outquad);
  anim_track_add_scalar(&scale_track, 1.2f, 2.0f, 1.0f, 1.0f, anim_ease_linear);
  anim_clip_add(clip, scale_track);
}

// Initialize circular motion
void init_circle_animation(anim_clip *clip) {
  anim_clip_reset(clip);
  anim_clip_config(clip, 3.0f, 1.0f, 0.0f, ANIM_PLAY_LOOP);

  anim_track circle_track;
  anim_circle_params params = {{600, 400}, 150};
  anim_track_from_path(&circle_track, anim_path_circle, &params, 3.0f, 60,
                       anim_ease_linear);
  anim_clip_add(clip, circle_track);
}

// Initialize color pulse animation
void init_color_animation(anim_clip *clip) {
  anim_clip_reset(clip);
  anim_clip_config(clip, 2.0f, 1.0f, 0.0f, ANIM_PLAY_LOOP);

  anim_track color_track;
  anim_track_reset(&color_track);
  anim_rgba red = {1.0f, 0.2f, 0.2f, 1.0f};
  anim_rgba blue = {0.2f, 0.4f, 1.0f, 1.0f};
  anim_rgba green = {0.2f, 1.0f, 0.4f, 1.0f};
  anim_rgba yellow = {1.0f, 1.0f, 0.2f, 1.0f};

  anim_track_add_color(&color_track, 0.0f, 0.5f, red, blue, anim_ease_smooth);
  anim_track_add_color(&color_track, 0.5f, 1.0f, blue, green, anim_ease_smooth);
  anim_track_add_color(&color_track, 1.0f, 1.5f, green, yellow,
                       anim_ease_smooth);
  anim_track_add_color(&color_track, 1.5f, 2.0f, yellow, red, anim_ease_smooth);
  anim_clip_add(clip, color_track);
}

// Initialize Lissajous curve
void init_lissajous_animation(anim_clip *clip) {
  anim_clip_reset(clip);
  anim_clip_config(clip, 6.28f, 1.0f, 0.0f, ANIM_PLAY_LOOP);

  anim_track lissa_track;
  anim_lissajous_params params = {150, 150, 3, 2, 1.57f};
  anim_track_from_path(&lissa_track, anim_path_lissajous, &params, 6.28f, 100,
                       anim_ease_linear);
  anim_clip_add(clip, lissa_track);
}

// Initialize rotation animation
void init_rotate_animation(anim_clip *clip) {
  anim_clip_reset(clip);
  anim_clip_config(clip, 4.0f, 1.0f, 0.0f, ANIM_PLAY_PINGPONG);

  anim_track rot_track;
  anim_track_reset(&rot_track);
  anim_track_add_scalar(&rot_track, 0.0f, 2.0f, 0.0f, 360.0f,
                        anim_ease_inoutquad);
  anim_track_add_scalar(&rot_track, 2.0f, 4.0f, 360.0f, 0.0f,
                        anim_ease_inoutquad);
  anim_clip_add(clip, rot_track);
}

int main(void) {
  InitWindow(SCREEN_W, SCREEN_H, "anim2d + Raylib Demo");
  SetTargetFPS(60);

  demo_state state = {0};
  init_bounce_animation(&state.bounce_clip);
  init_circle_animation(&state.circle_clip);
  init_color_animation(&state.color_clip);
  init_lissajous_animation(&state.lissajous_clip);
  init_rotate_animation(&state.rotate_clip);

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // Update all animations
    anim_clip_update(&state.bounce_clip, dt);
    anim_clip_update(&state.circle_clip, dt);
    anim_clip_update(&state.color_clip, dt);
    anim_clip_update(&state.lissajous_clip, dt);
    anim_clip_update(&state.rotate_clip, dt);

    BeginDrawing();
    ClearBackground((Color){15, 15, 25, 255});

    // Draw title
    DrawText("anim2d Animation Library Demo", 20, 20, 30, WHITE);
    DrawText("Press ESC to exit", 20, 60, 20, LIGHTGRAY);

    // 1. Bouncing ball with squash & stretch
    {
      anim_eval evals[2];
      anim_clip_eval(&state.bounce_clip, evals);

      if (evals[0].active && evals[1].active) {
        Vector2 pos = {evals[0].value.v.x, evals[0].value.v.y};
        float scale = evals[1].value.s;

        DrawText("Bouncing Ball (squash & stretch)", 50, 150, 20, YELLOW);
        DrawCircleV(pos, 30 * scale, ORANGE);
      }
    }

    // 2. Circular motion
    {
      anim_eval eval;
      anim_clip_eval(&state.circle_clip, &eval);

      if (eval.active) {
        Vector2 pos = {eval.value.v.x, eval.value.v.y};
        DrawText("Circular Path", 450, 250, 20, SKYBLUE);
        DrawCircleV((Vector2){600, 400}, 150, Fade(DARKGRAY, 0.2f));
        DrawCircleV(pos, 20, SKYBLUE);
      }
    }

    // 3. Color pulse animation
    {
      anim_eval eval;
      anim_clip_eval(&state.color_clip, &eval);

      if (eval.active) {
        Color c = {(unsigned char)(eval.value.c.r * 255),
                   (unsigned char)(eval.value.c.g * 255),
                   (unsigned char)(eval.value.c.b * 255),
                   (unsigned char)(eval.value.c.a * 255)};
        DrawText("Color Pulse", 850, 150, 20, c);
        DrawRectangle(850, 200, 150, 150, c);
      }
    }

    // 4. Lissajous curve
    {
      anim_eval eval;
      anim_clip_eval(&state.lissajous_clip, &eval);

      if (eval.active) {
        Vector2 pos = {eval.value.v.x + 600, eval.value.v.y + 600};
        DrawText("Lissajous Curve", 450, 450, 20, LIME);
        DrawCircleV(pos, 8, LIME);

        // Draw trail
        static Vector2 trail[100] = {0};
        static int trail_idx = 0;
        trail[trail_idx] = pos;
        trail_idx = (trail_idx + 1) % 100;

        for (int i = 0; i < 100; i++) {
          if (trail[i].x != 0) {
            DrawCircleV(trail[i], 2, Fade(LIME, 0.3f));
          }
        }
      }
    }

    // 5. Rotating square
    {
      anim_eval eval;
      anim_clip_eval(&state.rotate_clip, &eval);

      if (eval.active) {
        float angle = eval.value.s;
        Vector2 center = {950, 500};

        DrawText("Rotating Square (ping-pong)", 800, 400, 20, MAGENTA);

        // Draw rotated square using DrawRectanglePro
        Rectangle rect = {center.x, center.y, 80, 80};
        Vector2 origin = {40, 40};
        DrawRectanglePro(rect, origin, angle, MAGENTA);

        // Draw center point
        DrawCircleV(center, 4, WHITE);
      }
    }

    // Draw animation info
    DrawText(TextFormat("Bounce Time: %.2f", state.bounce_clip.time), 20,
             SCREEN_H - 100, 16, GRAY);
    DrawText(TextFormat("Circle Time: %.2f", state.circle_clip.time), 20,
             SCREEN_H - 75, 16, GRAY);
    DrawText(TextFormat("Color Time: %.2f", state.color_clip.time), 20,
             SCREEN_H - 50, 16, GRAY);
    DrawText(TextFormat("Lissajous Time: %.2f", state.lissajous_clip.time), 20,
             SCREEN_H - 25, 16, GRAY);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
