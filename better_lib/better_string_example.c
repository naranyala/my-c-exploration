#include "better_string.h"

int main() {
    char* hello = bstr_aprintf("Hello %s %d", "world", 2025);
    printf("%s\n", hello); free(hello);

    const char* parts[] = {"quick", "brown", "fox"};
    char* joined = bstr_join(" - ", parts, 3);
    printf("%s\n", joined); // quick - brown - fox
    free(joined);
}
