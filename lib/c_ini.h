#ifndef C_INI_H
#define C_INI_H

#include <stdio.h>
#include <string.h>

// Maximum size of the file we can load into the buffer
#define C_INI_MAX_FILE_SIZE 4096 

// Structure to hold the key/value/section information
typedef struct {
    const char *section;
    const char *key;
    const char *value;
} c_ini_entry_t;

// Context structure for the parser
typedef struct {
    char buffer[C_INI_MAX_FILE_SIZE];
    int size;
    // Pointers into the buffer where entries were found (max entries)
    c_ini_entry_t entries[64]; 
    int entry_count;
} c_ini_context_t;

// Function declarations
int c_ini_load_file(const char *filepath, c_ini_context_t *context);
const char *c_ini_get_value(c_ini_context_t *context, const char *section, const char *key);

// --- Implementation Section ---
#ifdef C_INI_IMPLEMENTATION

// Helper to remove leading/trailing whitespace and comments
static char *c_ini_trim(char *str) {
    if (!str || *str == '\0') return str;

    // Remove comments
    char *comment = strchr(str, ';');
    if (comment) *comment = '\0';
    comment = strchr(str, '#');
    if (comment) *comment = '\0';

    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end-- = '\0';
    }

    // Trim leading whitespace
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    return str;
}

int c_ini_load_file(const char *filepath, c_ini_context_t *context) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return 0;

    context->entry_count = 0;
    
    // Read the whole file into the buffer
    context->size = fread(context->buffer, 1, C_INI_MAX_FILE_SIZE - 1, fp);
    fclose(fp);
    if (context->size == 0) return 0;
    context->buffer[context->size] = '\0';

    char *line = context->buffer;
    char *current_section = "";
    int max_entries = sizeof(context->entries) / sizeof(context->entries[0]);

    while (line && *line != '\0' && context->entry_count < max_entries) {
        char *next_line = strchr(line, '\n');
        if (next_line) *next_line++ = '\0';
        
        char *trimmed_line = c_ini_trim(line);

        if (trimmed_line[0] == '[') {
            // Found Section: [name]
            char *end = strchr(trimmed_line, ']');
            if (end) {
                *end = '\0';
                current_section = trimmed_line + 1;
            }
        } else if (strchr(trimmed_line, '=')) {
            // Found Key-Value: key=value
            char *key_part = trimmed_line;
            char *eq_sign = strchr(key_part, '=');
            
            if (eq_sign) {
                *eq_sign = '\0'; 
                char *value_part = eq_sign + 1;
                
                context->entries[context->entry_count].section = c_ini_trim(current_section);
                context->entries[context->entry_count].key = c_ini_trim(key_part);
                context->entries[context->entry_count].value = c_ini_trim(value_part);
                context->entry_count++;
            }
        }
        
        line = next_line;
    }
    return context->entry_count;
}

const char *c_ini_get_value(c_ini_context_t *context, const char *section, const char *key) {
    for (int i = 0; i < context->entry_count; ++i) {
        if (strcmp(context->entries[i].section, section) == 0 &&
            strcmp(context->entries[i].key, key) == 0) {
            return context->entries[i].value;
        }
    }
    return NULL; // Not found
}

#endif // C_INI_IMPLEMENTATION

#endif // C_INI_H
