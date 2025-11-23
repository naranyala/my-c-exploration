
#include "better_dirent2.h"

int main() {
    struct dirent *entry;
    FOR_EACH_FILE(".", entry) {
        printf("File: %s\n", entry->d_name);
    }
}
