// presentation_02.c - FIXED AND READY TO RUN
// gcc presentation_02.c -o demo -lraylib -lm -ldl -lpthread -lGL -lrt -lX11

#include "raylib.h"
#include "../lib/gp2d.h"  // Make sure gp2d.h is in the same folder or adjust path
#include <stdlib.h>
#include <time.h>

#define W  1000
#define H  600

// ---------- 5 slides ----------------------------------------------------------
static void draw0(float t){ (void)t;
    DrawText("gp2d.h",          100, 150, 120, WHITE);
    DrawText("The Ultimate 2D Motion Toolkit", 100, 320, 48, LIGHTGRAY);
}
static void draw1(float t){ (void)t;
    float s = 1.0f + 0.3f*sinf(t);                // lazy scale pulse
    Vector2 c = { W*0.5f, H*0.5f };
    BeginMode2D((Camera2D){ .offset={0}, .target=c, .rotation=0, .zoom=s });
        DrawText("Transforms", c.x-220, c.y-80, 100, YELLOW);
    EndMode2D();
    DrawText("• Translation\n• Rotation\n• Scale", 180, 400, 40, WHITE);
}
static void draw2(float t){
    Vector2 p0={300,400}, p1={500,150}, p2={900,650}, p3={1100,400};
    // DrawLineBezierCubic(p0,p3,p1,p2, 4, WHITE);
    
//     DrawLineBezierCubicV((Vector2){p0.x,p0.y},(Vector2){p3.x,p3.y}, 
// (Vector2){p1.x,p1.y},(Vector2){p2.x,p2.y},thic
//                          k,col)
    //

    float bt = fmodf(t*0.25f,1.0f);
    gp2d_vec2 pos = gp2d_v2_bezier_cubic(gp2d_v2(p0.x,p0.y),
                                         gp2d_v2(p1.x,p1.y),
                                         gp2d_v2(p2.x,p2.y),
                                         gp2d_v2(p3.x,p3.y), bt);
    DrawCircle(pos.x, pos.y, 20, RED);
}
static void draw3(float t){ (void)t;
    DrawText("Particles & Easing", 100, 120, 80, PURPLE);
    DrawText("All powered by gp2d.h", 100, 220, 50, LIGHTGRAY);
}
static void draw4(float t){ (void)t;
    DrawText("Thank You!", 200, 200, 100, GOLD);
    DrawText("gp2d.h is now your full 2D engine", 120, 350, 50, WHITE);
    DrawText("Zero dependencies • Header-only • Blazing fast", 100, 450, 40, LIGHTGRAY);
}
// -----------------------------------------------------------------------------

typedef struct { Color bg; void (*draw)(float); } Slide;
static const Slide slides[] = {
    { BLACK,   draw0 },
    { DARKBLUE,draw1 },
    { DARKGREEN,draw2},
    { MAROON,  draw3 },
    { RAYWHITE,draw4},
};
static const int N = sizeof(slides)/sizeof(slides[0]);
static int idx = 0;

int main(){
    InitWindow(W, H, "gp2d.h – tiny presentation");
    SetTargetFPS(60);
    float t = 0.f;

    while (!WindowShouldClose()){
        t += GetFrameTime();

        // ----- navigation -----
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(0))
            if (idx < N-1) ++idx;
        if (IsKeyPressed(KEY_LEFT))
            if (idx > 0) --idx;

        // ----- draw ----------
        BeginDrawing();
        ClearBackground(slides[idx].bg);
        slides[idx].draw(t);
        DrawText(TextFormat("%d / %d   →", idx+1, N), W-220, H-40, 30, WHITE);
        EndDrawing();
    }
    CloseWindow();
}
