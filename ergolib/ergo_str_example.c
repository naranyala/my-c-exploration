#define ERGO_STR_IMPLEMENTATION
#include "ergo_str.h"
#include <stdio.h>

int main(void) {
    char buf[20];
    ergo_strcpy_s(buf, sizeof buf, "Hello");
    ergo_strcat_s(buf, sizeof buf, ", World!");
    printf("Result: '%s'\n", buf);  // "Hello, World!"

    printf("Equal? %d\n", ergo_streq("test", "test"));  // 1

    char input[] = "  \t  hello  \n";
    char *clean = ergo_strtrim(input);
    printf("Trimmed: '%s'\n", clean);  // "hello"

    char *copy = ergo_strdup("dynamic");
    printf("Copy: %s\n", copy);
    free(copy);
    return 0;
}
