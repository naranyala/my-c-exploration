#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COMPONENTS 4
#define MAX_BUS_MESSAGES 10
#define MAX_LOG_ENTRIES 15

// Bus message types
typedef enum {
    MSG_READ_REQUEST,
    MSG_READ_RESPONSE,
    MSG_WRITE_REQUEST,
    MSG_WRITE_ACK,
    MSG_INTERRUPT,
    MSG_BROADCAST
} MessageType;

// Bus message structure
typedef struct {
    MessageType type;
    int sourceId;
    int destId;  // -1 for broadcast
    int address;
    int data;
    float timestamp;
    bool active;
    float progress; // 0.0 to 1.0 for animation
} BusMessage;

// Component types
typedef enum {
    COMP_CPU,
    COMP_MEMORY,
    COMP_IO_DEVICE,
    COMP_DMA_CONTROLLER
} ComponentType;

// Component structure
typedef struct {
    int id;
    ComponentType type;
    const char* name;
    Vector2 position;
    Color color;
    int registerValue;
    bool isBusy;
    float busyTimer;
    int messageCount;
} Component;

// System bus structure
typedef struct {
    BusMessage messages[MAX_BUS_MESSAGES];
    int messageCount;
    bool busLocked;
    int busOwner;
    float clockCycle;
} SystemBus;

// Log entry
typedef struct {
    char text[256];
    float timestamp;
    Color color;
} LogEntry;

// Global system state
typedef struct {
    Component components[MAX_COMPONENTS];
    SystemBus bus;
    LogEntry logs[MAX_LOG_ENTRIES];
    int logCount;
    float systemTime;
    bool paused;
} SystemState;

// Function prototypes
void InitSystem(SystemState* state);
void AddLog(SystemState* state, const char* text, Color color);
void SendMessage(SystemState* state, MessageType type, int source, int dest, int addr, int data);
void ProcessBus(SystemState* state);
void ProcessComponent(SystemState* state, int compId);
void RenderSystem(SystemState* state);

// Initialize the system
void InitSystem(SystemState* state) {
    state->systemTime = 0.0f;
    state->paused = false;
    state->logCount = 0;
    
    // Initialize components
    state->components[0] = (Component){
        0, COMP_CPU, "CPU", {100, 150}, BLUE, 0, false, 0, 0
    };
    state->components[1] = (Component){
        1, COMP_MEMORY, "Memory", {100, 350}, GREEN, 0, false, 0, 0
    };
    state->components[2] = (Component){
        2, COMP_IO_DEVICE, "I/O Device", {600, 150}, ORANGE, 0, false, 0, 0
    };
    state->components[3] = (Component){
        3, COMP_DMA_CONTROLLER, "DMA Ctrl", {600, 350}, PURPLE, 0, false, 0, 0
    };
    
    // Initialize bus
    state->bus.messageCount = 0;
    state->bus.busLocked = false;
    state->bus.busOwner = -1;
    state->bus.clockCycle = 0.0f;
    
    for (int i = 0; i < MAX_BUS_MESSAGES; i++) {
        state->bus.messages[i].active = false;
    }
    
    AddLog(state, "System initialized", DARKGRAY);
}

// Add log entry
void AddLog(SystemState* state, const char* text, Color color) {
    if (state->logCount < MAX_LOG_ENTRIES) {
        snprintf(state->logs[state->logCount].text, 256, "[%.2f] %s", 
                 state->systemTime, text);
        state->logs[state->logCount].timestamp = state->systemTime;
        state->logs[state->logCount].color = color;
        state->logCount++;
    } else {
        // Shift logs up
        for (int i = 0; i < MAX_LOG_ENTRIES - 1; i++) {
            state->logs[i] = state->logs[i + 1];
        }
        snprintf(state->logs[MAX_LOG_ENTRIES - 1].text, 256, "[%.2f] %s", 
                 state->systemTime, text);
        state->logs[MAX_LOG_ENTRIES - 1].timestamp = state->systemTime;
        state->logs[MAX_LOG_ENTRIES - 1].color = color;
    }
}

// Send message on bus
void SendMessage(SystemState* state, MessageType type, int source, int dest, int addr, int data) {
    // Find empty slot
    for (int i = 0; i < MAX_BUS_MESSAGES; i++) {
        if (!state->bus.messages[i].active) {
            state->bus.messages[i].type = type;
            state->bus.messages[i].sourceId = source;
            state->bus.messages[i].destId = dest;
            state->bus.messages[i].address = addr;
            state->bus.messages[i].data = data;
            state->bus.messages[i].timestamp = state->systemTime;
            state->bus.messages[i].active = true;
            state->bus.messages[i].progress = 0.0f;
            state->bus.messageCount++;
            
            const char* msgTypes[] = {"READ_REQ", "READ_RESP", "WRITE_REQ", 
                                     "WRITE_ACK", "INTERRUPT", "BROADCAST"};
            char logMsg[256];
            snprintf(logMsg, 256, "%s -> %s: %s (addr:%d, data:%d)", 
                    state->components[source].name,
                    dest == -1 ? "ALL" : state->components[dest].name,
                    msgTypes[type], addr, data);
            AddLog(state, logMsg, state->components[source].color);
            
            state->components[source].messageCount++;
            return;
        }
    }
}

// Process bus messages
void ProcessBus(SystemState* state) {
    state->bus.clockCycle += GetFrameTime();
    
    for (int i = 0; i < MAX_BUS_MESSAGES; i++) {
        if (state->bus.messages[i].active) {
            BusMessage* msg = &state->bus.messages[i];
            msg->progress += GetFrameTime() * 0.5f; // Animation speed
            
            // Message delivered
            if (msg->progress >= 1.0f) {
                int dest = msg->destId;
                
                // Handle broadcast
                if (dest == -1) {
                    for (int j = 0; j < MAX_COMPONENTS; j++) {
                        if (j != msg->sourceId) {
                            state->components[j].registerValue = msg->data;
                        }
                    }
                } else if (dest >= 0 && dest < MAX_COMPONENTS) {
                    // Process message at destination
                    switch (msg->type) {
                        case MSG_READ_REQUEST:
                            // Memory sends back data
                            if (state->components[dest].type == COMP_MEMORY) {
                                SendMessage(state, MSG_READ_RESPONSE, dest, 
                                          msg->sourceId, msg->address, 
                                          rand() % 256);
                            }
                            break;
                        case MSG_READ_RESPONSE:
                            state->components[dest].registerValue = msg->data;
                            break;
                        case MSG_WRITE_REQUEST:
                            state->components[dest].registerValue = msg->data;
                            SendMessage(state, MSG_WRITE_ACK, dest, 
                                      msg->sourceId, msg->address, 0);
                            break;
                        case MSG_WRITE_ACK:
                            state->components[dest].isBusy = false;
                            break;
                        case MSG_INTERRUPT:
                            state->components[dest].isBusy = true;
                            state->components[dest].busyTimer = 2.0f;
                            break;
                        default:
                            break;
                    }
                }
                
                msg->active = false;
                state->bus.messageCount--;
            }
        }
    }
    
    // Update component busy timers
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (state->components[i].isBusy && state->components[i].busyTimer > 0) {
            state->components[i].busyTimer -= GetFrameTime();
            if (state->components[i].busyTimer <= 0) {
                state->components[i].isBusy = false;
            }
        }
    }
}

// Render the system
void RenderSystem(SystemState* state) {
    ClearBackground(RAYWHITE);
    
    // Draw title
    DrawText("SYSTEM BUS MECHANISM DEMO", 20, 10, 24, BLACK);
    DrawText(state->paused ? "[PAUSED]" : "[RUNNING]", 20, 40, 16, RED);
    DrawText(TextFormat("System Time: %.2f", state->systemTime), 20, 60, 16, DARKGRAY);
    
    // Draw bus (horizontal line in middle)
    int busY = 250;
    DrawLineEx((Vector2){50, busY}, (Vector2){750, busY}, 4, DARKGRAY);
    DrawText("SYSTEM BUS", 330, busY - 25, 16, DARKGRAY);
    
    // Draw components
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        Component* comp = &state->components[i];
        Rectangle rect = {comp->position.x, comp->position.y, 120, 80};
        
        Color compColor = comp->isBusy ? ColorAlpha(RED, 0.7f) : comp->color;
        DrawRectangleRec(rect, ColorAlpha(compColor, 0.3f));
        DrawRectangleLinesEx(rect, 2, compColor);
        
        // Draw connection to bus
        DrawLineEx(
            (Vector2){comp->position.x + 60, comp->position.y + (comp->position.y < busY ? 80 : 0)},
            (Vector2){comp->position.x + 60, busY},
            2, comp->color
        );
        
        // Component info
        DrawText(comp->name, comp->position.x + 10, comp->position.y + 10, 12, BLACK);
        DrawText(TextFormat("Reg: %d", comp->registerValue), 
                comp->position.x + 10, comp->position.y + 30, 10, DARKGRAY);
        DrawText(TextFormat("Msgs: %d", comp->messageCount), 
                comp->position.x + 10, comp->position.y + 45, 10, DARKGRAY);
        if (comp->isBusy) {
            DrawText("BUSY", comp->position.x + 10, comp->position.y + 60, 10, RED);
        }
    }
    
    // Draw messages traveling on bus
    for (int i = 0; i < MAX_BUS_MESSAGES; i++) {
        if (state->bus.messages[i].active) {
            BusMessage* msg = &state->bus.messages[i];
            
            Vector2 start = {state->components[msg->sourceId].position.x + 60, busY};
            Vector2 end;
            
            if (msg->destId == -1) {
                // Broadcast - animate across entire bus
                end = (Vector2){750, busY};
            } else {
                end = (Vector2){state->components[msg->destId].position.x + 60, busY};
            }
            
            Vector2 pos = {
                start.x + (end.x - start.x) * msg->progress,
                start.y
            };
            
            // Draw message packet
            DrawCircleV(pos, 8, state->components[msg->sourceId].color);
            DrawCircleLines(pos.x, pos.y, 8, BLACK);
            
            // Draw message type indicator
            const char* msgLabels[] = {"R?", "R!", "W?", "W!", "INT", "BC"};
            DrawText(msgLabels[msg->type], pos.x - 8, pos.y - 20, 10, BLACK);
        }
    }
    
    // Draw controls
    DrawText("CONTROLS:", 430, 480, 14, BLACK);
    DrawText("1: CPU Read from Memory", 430, 500, 12, DARKGRAY);
    DrawText("2: CPU Write to Memory", 430, 515, 12, DARKGRAY);
    DrawText("3: DMA Transfer", 430, 530, 12, DARKGRAY);
    DrawText("4: I/O Interrupt to CPU", 430, 545, 12, DARKGRAY);
    DrawText("5: Broadcast Message", 430, 560, 12, DARKGRAY);
    DrawText("SPACE: Pause/Resume", 430, 575, 12, DARKGRAY);
    
    // Draw log
    DrawText("BUS LOG:", 20, 480, 14, BLACK);
    DrawRectangleLines(20, 500, 400, 100, LIGHTGRAY);
    
    int logY = 505;
    int logsToShow = state->logCount > 5 ? 5 : state->logCount;
    for (int i = state->logCount - logsToShow; i < state->logCount; i++) {
        DrawText(state->logs[i].text, 25, logY, 10, state->logs[i].color);
        logY += 15;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 650;
    
    InitWindow(screenWidth, screenHeight, "System Bus Mechanism Demo");
    SetTargetFPS(60);
    
    SystemState state = {0};
    InitSystem(&state);
    
    while (!WindowShouldClose()) {
        // Handle input
        if (IsKeyPressed(KEY_SPACE)) {
            state.paused = !state.paused;
        }
        
        if (!state.paused) {
            state.systemTime += GetFrameTime();
            
            // Test scenarios
            if (IsKeyPressed(KEY_ONE)) {
                SendMessage(&state, MSG_READ_REQUEST, 0, 1, 0x1000, 0);
            }
            if (IsKeyPressed(KEY_TWO)) {
                SendMessage(&state, MSG_WRITE_REQUEST, 0, 1, 0x2000, rand() % 256);
            }
            if (IsKeyPressed(KEY_THREE)) {
                SendMessage(&state, MSG_READ_REQUEST, 3, 1, 0x3000, 0);
                state.components[3].isBusy = true;
                state.components[3].busyTimer = 3.0f;
            }
            if (IsKeyPressed(KEY_FOUR)) {
                SendMessage(&state, MSG_INTERRUPT, 2, 0, 0, 1);
            }
            if (IsKeyPressed(KEY_FIVE)) {
                SendMessage(&state, MSG_BROADCAST, 0, -1, 0, rand() % 100);
            }
            
            ProcessBus(&state);
        }
        
        BeginDrawing();
        RenderSystem(&state);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
