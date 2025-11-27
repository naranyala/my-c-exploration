#include "event_bus.h"
#include <stdio.h>

void on_damage(const char* event, void* data) {
    int dmg = *(int*)data;
    printf("[EVENT] Damage: %d\n", dmg);
}

int main() {
    eventbus_on("damage", on_damage);

    int d = 15;
    eventbus_emit("damage", &d);

    eventbus_off("damage", on_damage);

    return 0;
}

