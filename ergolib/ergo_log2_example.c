/* Usage */
#include "ergo_log2.h"

int main() {
    LOG_INFO("Server started on port %d", 8080);
    LOG_WARN("Config file missing, using defaults");
    LOG_ERROR("Failed to bind socket");
}

