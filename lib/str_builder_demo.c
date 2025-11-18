#include <stdio.h>
#include "str_builder.h"  // or the separate header version

int main() {
    // Create string builder (automatically manages memory)
    str_builder sb = sb_create();
    
    // Append content - no manual memory management!
    sb_append(&sb, "Hello");
    sb_append(&sb, " ");
    sb_append(&sb, "World!");
    sb_append_char(&sb, '\n');
    
    sb_append_format(&sb, "Numbers: %d, %.2f\n", 42, 3.14159);
    
    // Use the built string
    printf("Result:\n%s", sb_string(&sb));
    printf("Length: %zu\n", sb_length(&sb));
    
    // Free when done (automatically frees internal buffer)
    sb_free(&sb);
    
    return 0;
}
