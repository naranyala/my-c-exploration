#ifndef MINDRL_H
#define MINDRL_H

#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_NODE_TEXT_LEN   64
#define NODE_RADIUS         20.0f
#define DEFAULT_FONT_SIZE   16
#define LINE_THICKNESS      2.0f
#define HORIZONTAL_SPACING  150.0f
#define VERTICAL_SPACING    50.0f

typedef struct MindMapNode MindMapNode;

struct MindMapNode
{
    char text[MAX_NODE_TEXT_LEN];
    Vector2 position;
    Color color;

    MindMapNode** children;
    int childrenCount;
    int childrenCapacity; 
    
    MindMapNode* parent; 
};

// --- INTERFACE ---

MindMapNode* CreateNode(const char* text, MindMapNode* parent, Color nodeColor);
void DestroyMap(MindMapNode* root);
void AddChild(MindMapNode* parent, MindMapNode* child);
void RemoveChild(MindMapNode* parent, MindMapNode* child);
void CalculateNodePositions(MindMapNode* root, Vector2 startPos);
void DrawMindMap(MindMapNode* root);

#endif // MINDRL_H

// ============================================================================
// --- IMPLEMENTATION ---
// ============================================================================
#ifdef MINDRL_IMPLEMENTATION

// --- Private Layout Helpers ---

// Pass 1: Sets X positions and determines the relative Y span required for each subtree.
// It sets the Y position for leaf nodes, but not the center Y for parent nodes yet.
static float CalculateRelativeYSpans(MindMapNode* node, float x, float y)
{
    if (node == NULL) return 0.0f;

    node->position.x = x;
    
    if (node->childrenCount == 0)
    {
        node->position.y = y;
        return 1.0f; 
    }

    float currentY = y;
    float totalHeightSpan = 0.0f;
    
    for (int i = 0; i < node->childrenCount; i++)
    {
        float childSpan = CalculateRelativeYSpans(
            node->children[i], 
            x + HORIZONTAL_SPACING, 
            currentY
        );
        currentY += childSpan * VERTICAL_SPACING;
        totalHeightSpan += childSpan;
    }

    return totalHeightSpan;
}

// Pass 2: Sets the absolute, centered Y position for all nodes based on the relative spans.
static void CenterNodePositions(MindMapNode* node, float offset)
{
    if (node == NULL) return;

    if (node->parent != NULL)
    {
        // Calculate the center Y relative to children
        float firstChildY = node->children[0]->position.y;
        float lastChildY = node->children[node->childrenCount - 1]->position.y;
        
        node->position.y = (firstChildY + lastChildY) / 2.0f;
    }
    
    // Apply the global offset determined in CalculateNodePositions
    node->position.y += offset;

    for (int i = 0; i < node->childrenCount; i++)
    {
        CenterNodePositions(node->children[i], offset);
    }
}

// --- Public API Functions ---

MindMapNode* CreateNode(const char* text, MindMapNode* parent, Color nodeColor)
{
    MindMapNode* node = (MindMapNode*)malloc(sizeof(MindMapNode));
    if (node == NULL) return NULL;

    strncpy(node->text, text, MAX_NODE_TEXT_LEN - 1);
    node->text[MAX_NODE_TEXT_LEN - 1] = '\0';
    
    node->position = (Vector2){ 0.0f, 0.0f };
    node->color = nodeColor;

    node->children = NULL;
    node->childrenCount = 0;
    node->childrenCapacity = 0; 
    
    node->parent = parent;

    return node;
}

void DestroyMap(MindMapNode* root)
{
    if (root == NULL) return;

    for (int i = 0; i < root->childrenCount; i++)
    {
        DestroyMap(root->children[i]);
    }

    if (root->children != NULL)
    {
        free(root->children);
    }
    
    free(root);
}

void AddChild(MindMapNode* parent, MindMapNode* child)
{
    if (parent == NULL || child == NULL) return;
    if (child->parent != NULL) return; // Prevent adding a node that already has a parent

    if (parent->childrenCount >= parent->childrenCapacity)
    {
        parent->childrenCapacity = (parent->childrenCapacity == 0) ? 4 : parent->childrenCapacity * 2;
        
        MindMapNode** newChildren = (MindMapNode**)realloc(
            parent->children, 
            parent->childrenCapacity * sizeof(MindMapNode*)
        );

        if (newChildren == NULL) 
        {
            parent->childrenCapacity /= 2;
            return;
        }
        parent->children = newChildren;
    }
    
    parent->children[parent->childrenCount++] = child;
    child->parent = parent;
}

void RemoveChild(MindMapNode* parent, MindMapNode* child)
{
    if (parent == NULL || child == NULL || child->parent != parent) return;

    int indexToRemove = -1;
    for (int i = 0; i < parent->childrenCount; i++)
    {
        if (parent->children[i] == child)
        {
            indexToRemove = i;
            break;
        }
    }

    if (indexToRemove != -1)
    {
        // Shift remaining children to fill the gap
        for (int i = indexToRemove; i < parent->childrenCount - 1; i++)
        {
            parent->children[i] = parent->children[i + 1];
        }
        
        parent->childrenCount--;
        child->parent = NULL; // Detach the child
    }
}


void CalculateNodePositions(MindMapNode* root, Vector2 startPos)
{
    if (root == NULL) return;

    // --- Pass 1: Calculate relative Y positions and total span ---
    // Start Y is 0.0f because we want relative vertical coordinates first.
    float totalHeightSpan = CalculateRelativeYSpans(root, startPos.x, 0.0f);
    
    // Calculate the total required vertical extent.
    float totalYExtent = totalHeightSpan * VERTICAL_SPACING;

    // --- Determine Global Offset ---
    // The nodes are currently positioned relative to Y=0.0f.
    // We want the entire tree centered around startPos.y.
    // The offset is calculated by: startPos.y - (half of the total tree extent).
    float yOffset = startPos.y - (totalYExtent / 2.0f);

    // Manually set the root's Y position before the second pass
    // (This position is based on the center of the total extent)
    root->position.y = startPos.y; 

    // --- Pass 2: Center and Apply Offset ---
    // Apply the offset to all children recursively.
    if (root->childrenCount > 0)
    {
        CenterNodePositions(root, yOffset);
    }
}


static void DrawNode(MindMapNode* node)
{
    if (node->parent != NULL)
    {
        DrawLineEx(node->parent->position, node->position, LINE_THICKNESS, node->color);
    }
    
    DrawCircleV(node->position, NODE_RADIUS, node->color);
    
    int textWidth = MeasureText(node->text, DEFAULT_FONT_SIZE);
    DrawText(node->text, 
             (int)node->position.x - textWidth / 2, 
             (int)node->position.y - DEFAULT_FONT_SIZE / 2, 
             DEFAULT_FONT_SIZE, 
             BLACK);
}

void DrawMindMap(MindMapNode* root)
{
    if (root == NULL) return;

    DrawNode(root);

    for (int i = 0; i < root->childrenCount; i++)
    {
        DrawMindMap(root->children[i]);
    }
}

#endif // MINDRL_IMPLEMENTATION
