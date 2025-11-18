
#include "raylib.h"
#include <string.h>
#include <stdio.h>

// === Event Bus (from earlier) =======================================
typedef enum { EVENT_NOTIFICATION_PUSH, EVENT_NOTIFICATION_CLEAR_ALL } EventType;

typedef struct {
    char message[128];
    float duration;
} NotificationPayload;

typedef struct {
    EventType type;
    union {
        NotificationPayload notif;
    };
} Event;

typedef void (*EventCallback)(Event e);

#define MAX_SUBSCRIBERS 32
typedef struct {
    EventCallback subs[MAX_SUBSCRIBERS];
    int count;
} EventBus;

void bus_init(EventBus *bus) { bus->count = 0; }
void bus_subscribe(EventBus *bus, EventCallback cb) {
    if (bus->count < MAX_SUBSCRIBERS) bus->subs[bus->count++] = cb;
}
void bus_publish(EventBus *bus, Event e) {
    for (int i = 0; i < bus->count; i++)
        bus->subs[i](e);
}

// === Notifications ===================================================
#define MAX_NOTIFICATIONS 16
typedef struct {
    char message[128];
    float timer;
    float duration;
    int active;
} Notification;

typedef struct {
    Notification list[MAX_NOTIFICATIONS];
} NotificationManager;

NotificationManager notif_mgr;

void notif_event_handler(Event e) {
    if (e.type == EVENT_NOTIFICATION_PUSH) {
        for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
            if (!notif_mgr.list[i].active) {
                strcpy(notif_mgr.list[i].message, e.notif.message);
                notif_mgr.list[i].duration = e.notif.duration;
                notif_mgr.list[i].timer = 0.0f;
                notif_mgr.list[i].active = 1;
                break;
            }
        }
    }
    else if (e.type == EVENT_NOTIFICATION_CLEAR_ALL) {
        for (int i = 0; i < MAX_NOTIFICATIONS; i++)
            notif_mgr.list[i].active = 0;
    }
}

void notif_update(float dt) {
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        if (notif_mgr.list[i].active) {
            notif_mgr.list[i].timer += dt;
            if (notif_mgr.list[i].timer >= notif_mgr.list[i].duration)
                notif_mgr.list[i].active = 0;
        }
    }
}

void notif_draw(void) {
    int y = 20;
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        if (notif_mgr.list[i].active) {
            float t = notif_mgr.list[i].timer;
            float d = notif_mgr.list[i].duration;

            float alpha = 1.0f;
            if (d - t < 0.5f) alpha = (d - t) / 0.5f;

            DrawRectangle(20, y, 300, 40, Fade(DARKGRAY, alpha));
            DrawText(notif_mgr.list[i].message, 30, y + 10, 20, Fade(RAYWHITE, alpha));
            y += 50;
        }
    }
}

// === Main ============================================================
int main(void) {
    InitWindow(800, 450, "Raylib Notification System (Event Bus)");

    EventBus bus;
    bus_init(&bus);
    bus_subscribe(&bus, notif_event_handler);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // TEST EVENTS
        if (IsKeyPressed(KEY_N)) {
            Event e = {
                .type = EVENT_NOTIFICATION_PUSH,
                .notif = {"Hello notification!", 2.0f}
            };
            bus_publish(&bus, e);
        }

        if (IsKeyPressed(KEY_C)) {
            Event e = { .type = EVENT_NOTIFICATION_CLEAR_ALL };
            bus_publish(&bus, e);
        }

        // Update notifications
        notif_update(dt);

        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("Press N to push notification", 20, 400, 20, RAYWHITE);
        DrawText("Press C to clear all", 20, 370, 20, RAYWHITE);

        notif_draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
