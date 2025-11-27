/*
 * jsonwrite.h - A simple, type-safe JSON writer for C
 * 
 * Usage:
 *   #define JSONWRITE_IMPLEMENTATION
 *   #include "jsonwrite.h"
 * 
 * Features:
 *   - Type-safe JSON generation
 *   - Streaming output (file, memory, string)
 *   - No dynamic memory allocation in core operations
 *   - Proper escaping of strings
 *   - Human-readable or compact output
 * 
 * License: MIT
 */

#ifndef JSONWRITE_H
#define JSONWRITE_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct json_writer_t json_writer_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Output targets */
typedef enum {
    JSON_TARGET_FILE,
    JSON_TARGET_BUFFER,
    JSON_TARGET_DYNAMIC
} json_target_type_t;

/* Writer creation and destruction */
json_writer_t* json_writer_create_file(FILE* file);
json_writer_t* json_writer_create_buffer(char* buffer, size_t size);
json_writer_t* json_writer_create_dynamic(size_t initial_capacity);
void json_writer_destroy(json_writer_t* jw);

/* Output control */
void json_writer_set_indent(json_writer_t* jw, int indent);
void json_writer_set_compact(json_writer_t* jw, int compact);
const char* json_writer_get_buffer(const json_writer_t* jw);
size_t json_writer_get_size(const json_writer_t* jw);

/* Document structure */
void json_begin_object(json_writer_t* jw);
void json_end_object(json_writer_t* jw);
void json_begin_array(json_writer_t* jw);
void json_end_array(json_writer_t* jw);

/* Value writing with keys (for objects) */
void json_write_null(json_writer_t* jw, const char* key);
void json_write_bool(json_writer_t* jw, const char* key, int value);
void json_write_int(json_writer_t* jw, const char* key, long long value);
void json_write_uint(json_writer_t* jw, const char* key, unsigned long long value);
void json_write_double(json_writer_t* jw, const char* key, double value);
void json_write_string(json_writer_t* jw, const char* key, const char* value);
void json_write_raw(json_writer_t* jw, const char* key, const char* raw_json);

/* Value writing without keys (for arrays) */
void json_write_null_value(json_writer_t* jw);
void json_write_bool_value(json_writer_t* jw, int value);
void json_write_int_value(json_writer_t* jw, long long value);
void json_write_uint_value(json_writer_t* jw, unsigned long long value);
void json_write_double_value(json_writer_t* jw, double value);
void json_write_string_value(json_writer_t* jw, const char* value);
void json_write_raw_value(json_writer_t* jw, const char* raw_json);

/* Utility functions */
int json_writer_ok(const json_writer_t* jw);
void json_writer_reset(json_writer_t* jw);

#ifdef __cplusplus
}
#endif

#endif /* JSONWRITE_H */

#ifdef JSONWRITE_IMPLEMENTATION

typedef struct json_state_t {
    int depth;
    int needs_comma;
    int is_object;
} json_state_t;

struct json_writer_t {
    json_target_type_t target_type;
    union {
        FILE* file;
        struct {
            char* buffer;
            size_t size;
            size_t capacity;
        } buffer;
    } output;
    
    json_state_t* stack;
    size_t stack_size;
    size_t stack_capacity;
    
    int indent;
    int compact;
    int error;
};

static const size_t JSONWRITER_DEFAULT_CAPACITY = 1024;
static const size_t JSONWRITER_STACK_CAPACITY = 32;

/* Internal utility functions */
static void json_write_char(json_writer_t* jw, char c) {
    if (jw->error) return;
    
    switch (jw->target_type) {
        case JSON_TARGET_FILE:
            if (fputc(c, jw->output.file) == EOF) {
                jw->error = 1;
            }
            break;
            
        case JSON_TARGET_BUFFER:
            if (jw->output.buffer.size < jw->output.buffer.capacity) {
                jw->output.buffer.buffer[jw->output.buffer.size++] = c;
            } else {
                jw->error = 1;
            }
            break;
            
        case JSON_TARGET_DYNAMIC:
            if (jw->output.buffer.size >= jw->output.buffer.capacity) {
                size_t new_capacity = jw->output.buffer.capacity * 2;
                if (new_capacity < jw->output.buffer.capacity) {
                    jw->error = 1;
                    return;
                }
                
                char* new_buffer = (char*)realloc(jw->output.buffer.buffer, new_capacity);
                if (!new_buffer) {
                    jw->error = 1;
                    return;
                }
                
                jw->output.buffer.buffer = new_buffer;
                jw->output.buffer.capacity = new_capacity;
            }
            jw->output.buffer.buffer[jw->output.buffer.size++] = c;
            break;
    }
}

static void json_write_string_raw(json_writer_t* jw, const char* str) {
    if (!str) {
        json_write_string_raw(jw, "null");
        return;
    }
    
    while (*str) {
        json_write_char(jw, *str++);
    }
}

static void json_write_escaped_string(json_writer_t* jw, const char* str) {
    if (!str) {
        json_write_string_raw(jw, "null");
        return;
    }
    
    json_write_char(jw, '"');
    
    for (const char* p = str; *p; p++) {
        unsigned char c = (unsigned char)*p;
        switch (c) {
            case '"':  json_write_string_raw(jw, "\\\""); break;
            case '\\': json_write_string_raw(jw, "\\\\"); break;
            case '\b': json_write_string_raw(jw, "\\b"); break;
            case '\f': json_write_string_raw(jw, "\\f"); break;
            case '\n': json_write_string_raw(jw, "\\n"); break;
            case '\r': json_write_string_raw(jw, "\\r"); break;
            case '\t': json_write_string_raw(jw, "\\t"); break;
            default:
                if (c < 0x20 || c == 0x7F) {
                    /* Control characters */
                    char hex[7];
                    snprintf(hex, sizeof(hex), "\\u%04x", c);
                    json_write_string_raw(jw, hex);
                } else {
                    json_write_char(jw, c);
                }
                break;
        }
    }
    
    json_write_char(jw, '"');
}

static void json_write_indent(json_writer_t* jw) {
    if (jw->compact) return;
    
    json_write_char(jw, '\n');
    for (int i = 0; i < jw->stack->depth * jw->indent; i++) {
        json_write_char(jw, ' ');
    }
}

static void json_ensure_stack(json_writer_t* jw) {
    if (jw->stack_size >= jw->stack_capacity) {
        size_t new_capacity = jw->stack_capacity * 2;
        json_state_t* new_stack = (json_state_t*)realloc(jw->stack, new_capacity * sizeof(json_state_t));
        if (!new_stack) {
            jw->error = 1;
            return;
        }
        jw->stack = new_stack;
        jw->stack_capacity = new_capacity;
    }
}

static void json_push_state(json_writer_t* jw, int is_object) {
    json_ensure_stack(jw);
    if (jw->error) return;
    
    jw->stack_size++;
    jw->stack[jw->stack_size - 1].depth = jw->stack_size;
    jw->stack[jw->stack_size - 1].needs_comma = 0;
    jw->stack[jw->stack_size - 1].is_object = is_object;
}

static void json_pop_state(json_writer_t* jw) {
    if (jw->stack_size > 0) {
        jw->stack_size--;
    }
}

static json_state_t* json_current_state(json_writer_t* jw) {
    return jw->stack_size > 0 ? &jw->stack[jw->stack_size - 1] : NULL;
}

static void json_write_comma_if_needed(json_writer_t* jw) {
    json_state_t* state = json_current_state(jw);
    if (state && state->needs_comma) {
        json_write_char(jw, ',');
        if (!jw->compact) {
            json_write_char(jw, ' ');
        }
    }
    if (state) {
        state->needs_comma = 1;
    }
}

/* Public API implementation */
json_writer_t* json_writer_create_file(FILE* file) {
    if (!file) return NULL;
    
    json_writer_t* jw = (json_writer_t*)calloc(1, sizeof(json_writer_t));
    if (!jw) return NULL;
    
    jw->target_type = JSON_TARGET_FILE;
    jw->output.file = file;
    jw->indent = 2;
    jw->compact = 0;
    
    jw->stack_capacity = JSONWRITER_STACK_CAPACITY;
    jw->stack = (json_state_t*)malloc(jw->stack_capacity * sizeof(json_state_t));
    if (!jw->stack) {
        free(jw);
        return NULL;
    }
    
    return jw;
}

json_writer_t* json_writer_create_buffer(char* buffer, size_t size) {
    if (!buffer || size == 0) return NULL;
    
    json_writer_t* jw = (json_writer_t*)calloc(1, sizeof(json_writer_t));
    if (!jw) return NULL;
    
    jw->target_type = JSON_TARGET_BUFFER;
    jw->output.buffer.buffer = buffer;
    jw->output.buffer.size = 0;
    jw->output.buffer.capacity = size;
    jw->indent = 2;
    jw->compact = 0;
    
    jw->stack_capacity = JSONWRITER_STACK_CAPACITY;
    jw->stack = (json_state_t*)malloc(jw->stack_capacity * sizeof(json_state_t));
    if (!jw->stack) {
        free(jw);
        return NULL;
    }
    
    return jw;
}

json_writer_t* json_writer_create_dynamic(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = JSONWRITER_DEFAULT_CAPACITY;
    }
    
    json_writer_t* jw = (json_writer_t*)calloc(1, sizeof(json_writer_t));
    if (!jw) return NULL;
    
    jw->target_type = JSON_TARGET_DYNAMIC;
    jw->output.buffer.buffer = (char*)malloc(initial_capacity);
    if (!jw->output.buffer.buffer) {
        free(jw);
        return NULL;
    }
    
    jw->output.buffer.size = 0;
    jw->output.buffer.capacity = initial_capacity;
    jw->indent = 2;
    jw->compact = 0;
    
    jw->stack_capacity = JSONWRITER_STACK_CAPACITY;
    jw->stack = (json_state_t*)malloc(jw->stack_capacity * sizeof(json_state_t));
    if (!jw->stack) {
        free(jw->output.buffer.buffer);
        free(jw);
        return NULL;
    }
    
    return jw;
}

void json_writer_destroy(json_writer_t* jw) {
    if (!jw) return;
    
    if (jw->target_type == JSON_TARGET_DYNAMIC) {
        free(jw->output.buffer.buffer);
    }
    
    free(jw->stack);
    free(jw);
}

void json_writer_set_indent(json_writer_t* jw, int indent) {
    if (jw) {
        jw->indent = indent >= 0 ? indent : 0;
    }
}

void json_writer_set_compact(json_writer_t* jw, int compact) {
    if (jw) {
        jw->compact = compact ? 1 : 0;
    }
}

const char* json_writer_get_buffer(const json_writer_t* jw) {
    if (!jw || (jw->target_type != JSON_TARGET_BUFFER && 
                jw->target_type != JSON_TARGET_DYNAMIC)) {
        return NULL;
    }
    return jw->output.buffer.buffer;
}

size_t json_writer_get_size(const json_writer_t* jw) {
    if (!jw || (jw->target_type != JSON_TARGET_BUFFER && 
                jw->target_type != JSON_TARGET_DYNAMIC)) {
        return 0;
    }
    return jw->output.buffer.size;
}

void json_begin_object(json_writer_t* jw) {
    if (!jw || jw->error) return;
    
    json_write_comma_if_needed(jw);
    json_write_char(jw, '{');
    json_push_state(jw, 1);
}

void json_end_object(json_writer_t* jw) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || !state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_pop_state(jw);
    
    if (!jw->compact && state->needs_comma) {
        json_write_indent(jw);
    }
    json_write_char(jw, '}');
}

void json_begin_array(json_writer_t* jw) {
    if (!jw || jw->error) return;
    
    json_write_comma_if_needed(jw);
    json_write_char(jw, '[');
    json_push_state(jw, 0);
}

void json_end_array(json_writer_t* jw) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_pop_state(jw);
    
    if (!jw->compact && state->needs_comma) {
        json_write_indent(jw);
    }
    json_write_char(jw, ']');
}

/* Key-value pair writers */
void json_write_null(json_writer_t* jw, const char* key) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || !state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    if (!jw->compact) {
        json_write_indent(jw);
    }
    
    json_write_escaped_string(jw, key);
    json_write_char(jw, ':');
    if (!jw->compact) {
        json_write_char(jw, ' ');
    }
    json_write_string_raw(jw, "null");
}

void json_write_bool(json_writer_t* jw, const char* key, int value) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || !state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    if (!jw->compact) {
        json_write_indent(jw);
    }
    
    json_write_escaped_string(jw, key);
    json_write_char(jw, ':');
    if (!jw->compact) {
        json_write_char(jw, ' ');
    }
    json_write_string_raw(jw, value ? "true" : "false");
}

void json_write_int(json_writer_t* jw, const char* key, long long value) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || !state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    if (!jw->compact) {
        json_write_indent(jw);
    }
    
    json_write_escaped_string(jw, key);
    json_write_char(jw, ':');
    if (!jw->compact) {
        json_write_char(jw, ' ');
    }
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%lld", value);
    json_write_string_raw(jw, buffer);
}

void json_write_uint(json_writer_t* jw, const char* key, unsigned long long value) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || !state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    if (!jw->compact) {
        json_write_indent(jw);
    }
    
    json_write_escaped_string(jw, key);
    json_write_char(jw, ':');
    if (!jw->compact) {
        json_write_char(jw, ' ');
    }
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%llu", value);
    json_write_string_raw(jw, buffer);
}

void json_write_double(json_writer_t* jw, const char* key, double value) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || !state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    if (!jw->compact) {
        json_write_indent(jw);
    }
    
    json_write_escaped_string(jw, key);
    json_write_char(jw, ':');
    if (!jw->compact) {
        json_write_char(jw, ' ');
    }
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.15g", value);
    json_write_string_raw(jw, buffer);
}

void json_write_string(json_writer_t* jw, const char* key, const char* value) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || !state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    if (!jw->compact) {
        json_write_indent(jw);
    }
    
    json_write_escaped_string(jw, key);
    json_write_char(jw, ':');
    if (!jw->compact) {
        json_write_char(jw, ' ');
    }
    json_write_escaped_string(jw, value);
}

void json_write_raw(json_writer_t* jw, const char* key, const char* raw_json) {
    if (!jw || jw->error || !raw_json) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || !state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    if (!jw->compact) {
        json_write_indent(jw);
    }
    
    json_write_escaped_string(jw, key);
    json_write_char(jw, ':');
    if (!jw->compact) {
        json_write_char(jw, ' ');
    }
    json_write_string_raw(jw, raw_json);
}

/* Array value writers */
void json_write_null_value(json_writer_t* jw) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    json_write_string_raw(jw, "null");
}

void json_write_bool_value(json_writer_t* jw, int value) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    json_write_string_raw(jw, value ? "true" : "false");
}

void json_write_int_value(json_writer_t* jw, long long value) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%lld", value);
    json_write_string_raw(jw, buffer);
}

void json_write_uint_value(json_writer_t* jw, unsigned long long value) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%llu", value);
    json_write_string_raw(jw, buffer);
}

void json_write_double_value(json_writer_t* jw, double value) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.15g", value);
    json_write_string_raw(jw, buffer);
}

void json_write_string_value(json_writer_t* jw, const char* value) {
    if (!jw || jw->error) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    json_write_escaped_string(jw, value);
}

void json_write_raw_value(json_writer_t* jw, const char* raw_json) {
    if (!jw || jw->error || !raw_json) return;
    
    json_state_t* state = json_current_state(jw);
    if (!state || state->is_object) {
        jw->error = 1;
        return;
    }
    
    json_write_comma_if_needed(jw);
    json_write_string_raw(jw, raw_json);
}

int json_writer_ok(const json_writer_t* jw) {
    return jw && !jw->error;
}

void json_writer_reset(json_writer_t* jw) {
    if (!jw) return;
    
    jw->error = 0;
    jw->stack_size = 0;
    
    if (jw->target_type == JSON_TARGET_BUFFER || jw->target_type == JSON_TARGET_DYNAMIC) {
        jw->output.buffer.size = 0;
        if (jw->output.buffer.capacity > 0) {
            jw->output.buffer.buffer[0] = '\0';
        }
    }
}

#endif /* JSONWRITE_IMPLEMENTATION */
