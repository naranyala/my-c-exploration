// nchart_demo.c - Interactive Pie Chart Demo
#include "raylib.h"
#include "../lib/canvasplotlib.h"
#include <stdio.h>


int main() {
    const int WIDTH = 800;
    const int HEIGHT = 600;
    InitWindow(WIDTH, HEIGHT, "Chart Gallery - Press 1/2/3 to switch");
    SetTargetFPS(60);

    // Sample data: Monthly sales
    ChartData salesData[] = {
        {"Jan", 45.0f, SKYBLUE},
        {"Feb", 52.0f, BLUE},
        {"Mar", 38.0f, DARKBLUE},
        {"Apr", 65.0f, PURPLE},
        {"May", 58.0f, VIOLET},
        {"Jun", 72.0f, PINK},
    };
    
    // Sample data: Market share
    ChartData marketData[] = {
        {"Product A", 35.0f, RED},
        {"Product B", 28.0f, ORANGE},
        {"Product C", 20.0f, YELLOW},
        {"Product D", 12.0f, GREEN},
        {"Product E", 5.0f,  BLUE},
    };
    
    int dataCount = sizeof(salesData) / sizeof(salesData[0]);
    int marketCount = sizeof(marketData) / sizeof(marketData[0]);
    
    int currentChart = 0; // 0 = pie, 1 = bar, 2 = line

    while (!WindowShouldClose()) {
        // Switch charts with number keys
        if (IsKeyPressed(KEY_ONE)) currentChart = 0;
        if (IsKeyPressed(KEY_TWO)) currentChart = 1;
        if (IsKeyPressed(KEY_THREE)) currentChart = 2;

        BeginDrawing();
            ClearBackground(GetColor(0xF8F9FAFF));
            
            if (currentChart == 0) {
                // Pie Chart
                DrawTitle("Market Share Distribution");
                Vector2 center = {WIDTH / 2.0f, HEIGHT / 2.0f};
                DrawPieChart(center, 220.0f, marketData, marketCount);
                
                // Legend
                int legendX = 50;
                int legendY = HEIGHT - 200;
                DrawText("Legend:", legendX, legendY - 30, 20, DARKGRAY);
                
                float total = 0;
                for (int i = 0; i < marketCount; i++) {
                    total += marketData[i].value;
                }
                
                for (int i = 0; i < marketCount; i++) {
                    int y = legendY + i * 30;
                    DrawRectangle(legendX, y, 20, 20, marketData[i].color);
                    DrawRectangleLines(legendX, y, 20, 20, DARKGRAY);
                    
                    char text[64];
                    snprintf(text, sizeof(text), "%s: %.1f%%", 
                            marketData[i].label, 
                            (marketData[i].value / total) * 100);
                    DrawText(text, legendX + 30, y + 2, 18, DARKGRAY);
                }
                
            } else if (currentChart == 1) {
                // Bar Chart
                DrawTitle("Monthly Sales (Units)");
                Rectangle chartArea = {150, 150, WIDTH - 250, HEIGHT - 300};
                DrawBarChart(chartArea, salesData, dataCount);
                
            } else if (currentChart == 2) {
                // Line Chart
                DrawTitle("Sales Trend Over Time");
                Rectangle chartArea = {150, 150, WIDTH - 250, HEIGHT - 300};
                DrawLineChart(chartArea, salesData, dataCount);
            }
            
            // Instructions
        DrawText("Press 1: Pie Chart  |  2: Bar Chart  |  3: Line Chart", (WIDTH-600), (HEIGHT-40), 20, GRAY);
            
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
