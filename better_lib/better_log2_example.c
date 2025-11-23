#define BETTER_LOG_IMPLEMENTATION
#include "better_log2.h"

int main() {
    bl_set_level(BL_DEBUG);
    
    log_info("Application starting...");
    
    int connections = 0;
    log_warn("Connection pool low: %d remaining", connections);
    
    // Simulated error
    log_err("Failed to bind socket!");
    
    return 0;
}
