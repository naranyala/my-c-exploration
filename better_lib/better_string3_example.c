
#include "better_string3.h"

int main() {
    char buf[32];
    strcpy_safe(buf, sizeof(buf), "Hello");
    strcat_safe(buf, sizeof(buf), " World!");
    printf("%s (len=%zu, utf8=%zu)\n", buf, strlen(buf), utf8_len(buf));
}
