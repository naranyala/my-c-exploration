// canvasplotlib.h - Ultimate Math Function Plotter Edition
// Drop this file into any raylib project and run the demo!

#ifndef CANVASPLOTLIB_H
#define CANVASPLOTLIB_H

#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CPL_MAX_POINTS 10000
#define CPL_MAX_TRACES 8

typedef struct PlotContext {
    float xmin, xmax, ymin, ymax;
    int width, height;
    Rectangle area;
    Color bgColor;
    bool showGrid;
} PlotContext;

static float scale_x(PlotContext* ctx, float x) {
    float range = ctx->xmax - ctx->xmin;
    if (range == 0) range = 1;
    return ctx->area.x + (x - ctx->xmin) / range * ctx->area.width;
}

static float scale_y(PlotContext* ctx, float y) {
    float range = ctx->ymax - ctx->ymin;
    if (range == 0) range = 1;
    return ctx->area.y + ctx->area.height - (y - ctx->ymin) / range * ctx->area.height;
}

static void PlotInit(PlotContext* ctx, int w, int h) {
    *ctx = (PlotContext){
        .xmin = -10, .xmax = 10,
        .ymin = -10, .ymax = 10,
        .width = w, .height = h,
        .area = {70, 50, w-100, h-100},
        .bgColor = GetColor(0xF8F9FAFF),
        .showGrid = true
    };
}

static void PlotDrawGridAndAxes(PlotContext* ctx) {
    ClearBackground(ctx->bgColor);

    if (ctx->showGrid) {
        // Light grid
        for (int i = (int)ctx->xmin; i <= (int)ctx->xmax; i++) {
            if (i == 0) continue;
            float x = scale_x(ctx, i);
            DrawLine(x, ctx->area.y, x, ctx->area.y + ctx->area.height, GetColor(0xE0E0E0FF));
        }
        for (int i = (int)ctx->ymin; i <= (int)ctx->ymax; i++) {
            if (i == 0) continue;
            float y = scale_y(ctx, i);
            DrawLine(ctx->area.x, y, ctx->area.x + ctx->area.width, y, GetColor(0xE0E0E0FF));
        }
    }

    // Axes
    float zero_x = scale_x(ctx, 0);
    float zero_y = scale_y(ctx, 0);
    if (zero_x >= ctx->area.x && zero_x <= ctx->area.x + ctx->area.width)
        DrawLine(zero_x, ctx->area.y, zero_x, ctx->area.y + ctx->area.height, DARKGRAY);
    if (zero_y >= ctx->area.y && zero_y <= ctx->area.y + ctx->area.height)
        DrawLine(ctx->area.x, zero_y, ctx->area.x + ctx->area.width, zero_y, DARKGRAY);

    DrawRectangleLinesEx(ctx->area, 2, DARKGRAY);
}

static void PlotFunction(PlotContext* ctx, Color color, float t_min, float t_max, int steps,
                         float (*f_x)(float), float (*f_y)(float), float time) {
    Vector2 points[CPL_MAX_POINTS];
    int count = 0;

    for (int i = 0; i <= steps && count < CPL_MAX_POINTS; i++) {
        float t = t_min + (t_max - t_min) * i / (float)steps;
        float x = f_x(t + time);
        float y = f_y(t + time);

        if (x >= ctx->xmin && x <= ctx->xmax && y >= ctx->ymin && y <= ctx->ymax) {
            points[count++] = (Vector2){ scale_x(ctx, x), scale_y(ctx, y) };
        } else if (count > 0) {
            // Draw line segments
            for (int j = 0; j < count - 1; j++) {
                DrawLineEx(points[j], points[j + 1], 3.0f, color);
            }
            count = 0;
        }
    }
    if (count > 1) {
        for (int j = 0; j < count - 1; j++) {
            DrawLineEx(points[j], points[j + 1], 3.0f, color);
        }
    }
}

static void DrawTitle(const char* text) {
    int fw = MeasureText(text, 32);
    DrawText(text, (GetScreenWidth() - fw)/2, 15, 32, GetColor(0x2C3E50FF));
}

// Chart data structure
typedef struct {
    const char* label;
    float value;
    Color color;
} ChartData;

// Pie Chart
static void DrawPieChart(Vector2 center, float radius, ChartData* data, int count) {
    float total = 0;
    for (int i = 0; i < count; i++) {
        total += data[i].value;
    }
    if (total == 0) return;

    float startAngle = -90; // Start from top
    
    for (int i = 0; i < count; i++) {
        float sweepAngle = (data[i].value / total) * 360.0f;
        float endAngle = startAngle + sweepAngle;
        
        // Draw slice
        DrawCircleSector(center, radius, startAngle, endAngle, 32, data[i].color);
        DrawCircleSectorLines(center, radius, startAngle, endAngle, 32, DARKGRAY);
        
        // Draw label on slice
        float midAngle = (startAngle + endAngle) / 2.0f * DEG2RAD;
        float labelRadius = radius * 0.7f;
        Vector2 labelPos = {
            center.x + cosf(midAngle) * labelRadius,
            center.y + sinf(midAngle) * labelRadius
        };
        
        char percent[32];
        snprintf(percent, sizeof(percent), "%.1f%%", (data[i].value / total) * 100);
        int textWidth = MeasureText(percent, 18);
        DrawText(percent, labelPos.x - textWidth/2, labelPos.y - 9, 18, WHITE);
        
        startAngle = endAngle;
    }
}

// Bar Chart
static void DrawBarChart(Rectangle area, ChartData* data, int count) {
    if (count == 0) return;
    
    // Find max value for scaling
    float maxValue = 0;
    for (int i = 0; i < count; i++) {
        if (data[i].value > maxValue) maxValue = data[i].value;
    }
    if (maxValue == 0) maxValue = 1;
    
    float barWidth = area.width / count * 0.8f;
    float spacing = area.width / count * 0.2f;
    
    // Draw axes
    DrawLineEx((Vector2){area.x, area.y + area.height}, 
               (Vector2){area.x + area.width, area.y + area.height}, 2, DARKGRAY);
    DrawLineEx((Vector2){area.x, area.y}, 
               (Vector2){area.x, area.y + area.height}, 2, DARKGRAY);
    
    // Draw horizontal grid lines
    for (int i = 0; i <= 5; i++) {
        float y = area.y + area.height - (area.height * i / 5.0f);
        DrawLine(area.x, y, area.x + area.width, y, GetColor(0xE0E0E0FF));
        char label[16];
        snprintf(label, sizeof(label), "%.0f", maxValue * i / 5.0f);
        DrawText(label, area.x - 40, y - 8, 16, DARKGRAY);
    }
    
    // Draw bars
    for (int i = 0; i < count; i++) {
        float x = area.x + spacing/2 + i * (barWidth + spacing);
        float barHeight = (data[i].value / maxValue) * area.height;
        float y = area.y + area.height - barHeight;
        
        DrawRectangle(x, y, barWidth, barHeight, data[i].color);
        DrawRectangleLines(x, y, barWidth, barHeight, DARKGRAY);
        
        // Draw value on top of bar
        char valueText[32];
        snprintf(valueText, sizeof(valueText), "%.0f", data[i].value);
        int textWidth = MeasureText(valueText, 16);
        DrawText(valueText, x + barWidth/2 - textWidth/2, y - 20, 16, DARKGRAY);
        
        // Draw label below bar
        int labelWidth = MeasureText(data[i].label, 16);
        DrawText(data[i].label, x + barWidth/2 - labelWidth/2, area.y + area.height + 10, 16, DARKGRAY);
    }
}

// Line Chart
static void DrawLineChart(Rectangle area, ChartData* data, int count) {
    if (count < 2) return;
    
    // Find max value for scaling
    float maxValue = 0;
    for (int i = 0; i < count; i++) {
        if (data[i].value > maxValue) maxValue = data[i].value;
    }
    if (maxValue == 0) maxValue = 1;
    
    // Draw axes
    DrawLineEx((Vector2){area.x, area.y + area.height}, 
               (Vector2){area.x + area.width, area.y + area.height}, 2, DARKGRAY);
    DrawLineEx((Vector2){area.x, area.y}, 
               (Vector2){area.x, area.y + area.height}, 2, DARKGRAY);
    
    // Draw horizontal grid lines
    for (int i = 0; i <= 5; i++) {
        float y = area.y + area.height - (area.height * i / 5.0f);
        DrawLine(area.x, y, area.x + area.width, y, GetColor(0xE0E0E0FF));
        char label[16];
        snprintf(label, sizeof(label), "%.0f", maxValue * i / 5.0f);
        DrawText(label, area.x - 40, y - 8, 16, DARKGRAY);
    }
    
    // Calculate points
    float xStep = area.width / (count - 1);
    
    // Draw line segments
    for (int i = 0; i < count - 1; i++) {
        float x1 = area.x + i * xStep;
        float y1 = area.y + area.height - (data[i].value / maxValue) * area.height;
        float x2 = area.x + (i + 1) * xStep;
        float y2 = area.y + area.height - (data[i + 1].value / maxValue) * area.height;
        
        DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 3, BLUE);
    }
    
    // Draw points and labels
    for (int i = 0; i < count; i++) {
        float x = area.x + i * xStep;
        float y = area.y + area.height - (data[i].value / maxValue) * area.height;
        
        DrawCircle(x, y, 6, data[i].color);
        DrawCircleLines(x, y, 6, DARKGRAY);
        
        // Draw value above point
        char valueText[32];
        snprintf(valueText, sizeof(valueText), "%.0f", data[i].value);
        int textWidth = MeasureText(valueText, 16);
        DrawText(valueText, x - textWidth/2, y - 25, 16, DARKGRAY);
        
        // Draw label below
        int labelWidth = MeasureText(data[i].label, 16);
        DrawText(data[i].label, x - labelWidth/2, area.y + area.height + 10, 16, DARKGRAY);
    }
}

#endif // CANVASPLOTLIB_H
