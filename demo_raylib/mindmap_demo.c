#include <stdio.h>

// 1. Define the implementation macro in ONLY ONE C file before including the header.
#define MINDRL_IMPLEMENTATION
#include "../lib/mindmap.h"

int main(void)
{
    // --- Initialization ---
    const int screenWidth = 1200;
    const int screenHeight = 700;
    
    InitWindow(screenWidth, screenHeight, "raylib Mind Map Demo");
    SetTargetFPS(60);
    
    // --- Mind Map Structure Setup ---
    
    // 1. Create the Root Node (The main topic)
    MindMapNode* root = CreateNode("Building a Raylib App", NULL, SKYBLUE);
    
    // 2. Create Level 1 Branches (Main categories)
    MindMapNode* logic = CreateNode("Core Logic/Data", root, LIME);
    MindMapNode* graphics = CreateNode("Raylib Graphics", root, RED);
    MindMapNode* iohandling = CreateNode("Input & IO", root, ORANGE);
    
    AddChild(root, logic);
    AddChild(root, graphics);
    AddChild(root, iohandling);
    
    // 3. Create Level 2 Sub-Branches (Details for each category)
    
    // Details for Logic
    MindMapNode* ds = CreateNode("Tree Data Structure", logic, GREEN);
    MindMapNode* alloc = CreateNode("Dynamic Allocation (malloc)", logic, GREEN);
    AddChild(logic, ds);
    AddChild(logic, alloc);
    
    // Details for Graphics
    MindMapNode* drawprimitives = CreateNode("Draw Primitives (Circle, Line)", graphics, PINK);
    MindMapNode* drawtext = CreateNode("Draw Text Centered", graphics, PINK);
    MindMapNode* colors = CreateNode("Color Palette", graphics, PINK);
    AddChild(graphics, drawprimitives);
    AddChild(graphics, drawtext);
    AddChild(graphics, colors);

    // Details for IO
    MindMapNode* mouse = CreateNode("Mouse Input (Position)", iohandling, YELLOW);
    MindMapNode* destroy = CreateNode("Memory Cleanup (DestroyMap)", iohandling, YELLOW);
    AddChild(iohandling, mouse);
    AddChild(iohandling, destroy);

    // 4. Demonstrate RemoveChild (Optional: removing a planned feature)
    RemoveChild(graphics, colors);
    
    // --- Layout Calculation ---
    // Calculate node positions once, centered on the screen (X: 300, Y: 350)
    Vector2 startPosition = { 300.0f, (float)screenHeight / 2.0f };
    CalculateNodePositions(root, startPosition);


    // --- Main Game Loop ---
    while (!WindowShouldClose())
    {
        // --- Input (Optional: Recalculate layout on key press if nodes change)
        if (IsKeyPressed(KEY_SPACE))
        {
            // Re-center the layout (useful if nodes were moved or added dynamically)
            CalculateNodePositions(root, startPosition);
            printf("Layout Recalculated.\n");
        }

        // --- Drawing ---
        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        DrawText("Mind Map Demo (raylib + mindmap.h)", 10, 10, 20, GRAY);
        DrawText("Press SPACE to re-center the layout.", 10, 40, 15, DARKGRAY);

        // Draw the entire mind map using the library function
        DrawMindMap(root);

        EndDrawing();
    }

    // --- De-Initialization ---
    // IMPORTANT: Free all allocated memory using the library function
    DestroyMap(root); 
    CloseWindow();
    
    return 0;
}
