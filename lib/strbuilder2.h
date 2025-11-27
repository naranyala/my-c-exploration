/*
 * strbuilder.h - A simple, efficient string builder for C
 * 
 * Usage:
 *   #define STRBUILDER_IMPLEMENTATION
 *   #include "strbuilder.h"
 * 
 * License: MIT
 */

#ifndef STRBUILDER_H
#define STRBUILDER_H

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct str_builder_t str_builder_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Creation and destruction */
str_builder_t* str_builder_create(void);
str_builder_t* str_builder_create_with_capacity(size_t initial_capacity);
void str_builder_destroy(str_builder_t* sb);

/* Basic string operations */
void str_builder_append(str_builder_t* sb, const char* str);
void str_builder_append_char(str_builder_t* sb, char c);
void str_builder_append_n(str_builder_t* sb, const char* str, size_t len);

/* Formatted output */
void str_builder_append_format(str_builder_t* sb, const char* format, ...);
void str_builder_append_format_va(str_builder_t* sb, const char* format, va_list args);

/* Memory management */
void str_builder_clear(str_builder_t* sb);
void str_builder_trim(str_builder_t* sb);
char* str_builder_dump(const str_builder_t* sb, size_t* len);
char* str_builder_cstr(const str_builder_t* sb);

/* Information */
size_t str_builder_length(const str_builder_t* sb);
size_t str_builder_capacity(const str_builder_t* sb);
int str_builder_is_empty(const str_builder_t* sb);

/* Advanced operations */
void str_builder_insert(str_builder_t* sb, size_t pos, const char* str);
void str_builder_insert_char(str_builder_t* sb, size_t pos, char c);
void str_builder_delete(str_builder_t* sb, size_t pos, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* STRBUILDER_H */

#ifdef STRBUILDER_IMPLEMENTATION

struct str_builder_t {
    char* buffer;
    size_t length;
    size_t capacity;
};

static const size_t STRBUILDER_DEFAULT_CAPACITY = 32;
static const double STRBUILDER_GROWTH_FACTOR = 1.5;

str_builder_t* str_builder_create(void) {
    return str_builder_create_with_capacity(STRBUILDER_DEFAULT_CAPACITY);
}

str_builder_t* str_builder_create_with_capacity(size_t initial_capacity) {
    str_builder_t* sb = (str_builder_t*)malloc(sizeof(str_builder_t));
    if (!sb) return NULL;
    
    sb->capacity = initial_capacity > 0 ? initial_capacity : 1;
    sb->buffer = (char*)malloc(sb->capacity);
    if (!sb->buffer) {
        free(sb);
        return NULL;
    }
    
    sb->buffer[0] = '\0';
    sb->length = 0;
    return sb;
}

void str_builder_destroy(str_builder_t* sb) {
    if (sb) {
        free(sb->buffer);
        free(sb);
    }
}

static void str_builder_ensure_capacity(str_builder_t* sb, size_t additional) {
    size_t required = sb->length + additional + 1; // +1 for null terminator
    
    if (required > sb->capacity) {
        size_t new_capacity = sb->capacity;
        while (new_capacity < required) {
            new_capacity = (size_t)(new_capacity * STRBUILDER_GROWTH_FACTOR);
            if (new_capacity < sb->capacity) { // Overflow check
                new_capacity = required;
                break;
            }
        }
        
        char* new_buffer = (char*)realloc(sb->buffer, new_capacity);
        if (new_buffer) {
            sb->buffer = new_buffer;
            sb->capacity = new_capacity;
        }
    }
}

void str_builder_append(str_builder_t* sb, const char* str) {
    if (!sb || !str) return;
    size_t len = strlen(str);
    str_builder_append_n(sb, str, len);
}

void str_builder_append_char(str_builder_t* sb, char c) {
    if (!sb) return;
    str_builder_ensure_capacity(sb, 1);
    sb->buffer[sb->length++] = c;
    sb->buffer[sb->length] = '\0';
}

void str_builder_append_n(str_builder_t* sb, const char* str, size_t len) {
    if (!sb || !str || len == 0) return;
    str_builder_ensure_capacity(sb, len);
    memcpy(sb->buffer + sb->length, str, len);
    sb->length += len;
    sb->buffer[sb->length] = '\0';
}

void str_builder_append_format(str_builder_t* sb, const char* format, ...) {
    va_list args;
    va_start(args, format);
    str_builder_append_format_va(sb, format, args);
    va_end(args);
}

void str_builder_append_format_va(str_builder_t* sb, const char* format, va_list args) {
    if (!sb || !format) return;
    
    // Make a copy of args since we might need to use it twice
    va_list args_copy;
    va_copy(args_copy, args);
    
    // Try to format with current capacity
    size_t available = sb->capacity - sb->length;
    int needed = vsnprintf(sb->buffer + sb->length, available, format, args_copy);
    va_end(args_copy);
    
    if (needed < 0) return; // Formatting error
    
    if ((size_t)needed >= available) {
        // Not enough space, ensure capacity and try again
        str_builder_ensure_capacity(sb, (size_t)needed);
        available = sb->capacity - sb->length;
        
        va_copy(args_copy, args);
        needed = vsnprintf(sb->buffer + sb->length, available, format, args_copy);
        va_end(args_copy);
        
        if (needed < 0 || (size_t)needed >= available) return;
    }
    
    sb->length += (size_t)needed;
}

void str_builder_clear(str_builder_t* sb) {
    if (sb) {
        sb->length = 0;
        if (sb->capacity > 0) {
            sb->buffer[0] = '\0';
        }
    }
}

void str_builder_trim(str_builder_t* sb) {
    if (!sb || sb->length == 0) return;
    
    // Trim right
    char* end = sb->buffer + sb->length - 1;
    while (end >= sb->buffer && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }
    end[1] = '\0';
    sb->length = (size_t)(end - sb->buffer + 1);
    
    // Trim left
    char* start = sb->buffer;
    while (*start && (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r')) {
        start++;
    }
    
    if (start != sb->buffer) {
        size_t new_len = sb->length - (start - sb->buffer);
        memmove(sb->buffer, start, new_len + 1); // +1 for null terminator
        sb->length = new_len;
    }
}

char* str_builder_dump(const str_builder_t* sb, size_t* len) {
    if (!sb || sb->length == 0) {
        if (len) *len = 0;
        char* empty = (char*)malloc(1);
        if (empty) empty[0] = '\0';
        return empty;
    }
    
    char* result = (char*)malloc(sb->length + 1);
    if (result) {
        memcpy(result, sb->buffer, sb->length + 1);
        if (len) *len = sb->length;
    }
    return result;
}

char* str_builder_cstr(const str_builder_t* sb) {
    return sb ? sb->buffer : NULL;
}

size_t str_builder_length(const str_builder_t* sb) {
    return sb ? sb->length : 0;
}

size_t str_builder_capacity(const str_builder_t* sb) {
    return sb ? sb->capacity : 0;
}

int str_builder_is_empty(const str_builder_t* sb) {
    return !sb || sb->length == 0;
}

void str_builder_insert(str_builder_t* sb, size_t pos, const char* str) {
    if (!sb || !str || pos > sb->length) return;
    
    size_t len = strlen(str);
    if (len == 0) return;
    
    str_builder_ensure_capacity(sb, len);
    
    // Make space for the new string
    memmove(sb->buffer + pos + len, sb->buffer + pos, sb->length - pos + 1); // +1 for null terminator
    
    // Insert the new string
    memcpy(sb->buffer + pos, str, len);
    sb->length += len;
}

void str_builder_insert_char(str_builder_t* sb, size_t pos, char c) {
    if (!sb || pos > sb->length) return;
    
    str_builder_ensure_capacity(sb, 1);
    
    // Make space for the new character
    memmove(sb->buffer + pos + 1, sb->buffer + pos, sb->length - pos + 1); // +1 for null terminator
    
    // Insert the character
    sb->buffer[pos] = c;
    sb->length++;
}

void str_builder_delete(str_builder_t* sb, size_t pos, size_t len) {
    if (!sb || pos >= sb->length || len == 0) return;
    
    // Adjust len if it goes beyond the string length
    if (pos + len > sb->length) {
        len = sb->length - pos;
    }
    
    // Move the remaining string over the deleted portion
    memmove(sb->buffer + pos, sb->buffer + pos + len, sb->length - pos - len + 1); // +1 for null terminator
    sb->length -= len;
}

#endif /* STRBUILDER_IMPLEMENTATION */
