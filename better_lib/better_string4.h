/* better_string.h */
#ifndef BETTER_STRING_H
#define BETTER_STRING_H

#include <stdarg.h>

typedef struct {
    char* data;
    size_t len;
    size_t cap;
} b_string;

// Initialize a new dynamic string
b_string bs_new(const char* init_str);
// Append text (auto-resizes)
void bs_append(b_string* s, const char* txt);
// Formatted append (sprintf style)
void bs_append_fmt(b_string* s, const char* fmt, ...);
// Free memory
void bs_free(b_string* s);

#endif // BETTER_STRING_H

/* IMPLEMENTATION */
#ifdef BETTER_STRING_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void bs_grow(b_string* s, size_t needed) {
    size_t new_len = s->len + needed;
    if (new_len >= s->cap) {
        size_t new_cap = s->cap == 0 ? 64 : s->cap * 2;
        while (new_cap <= new_len) new_cap *= 2;
        s->data = (char*)realloc(s->data, new_cap);
        s->cap = new_cap;
    }
}

b_string bs_new(const char* init_str) {
    b_string s = {0};
    if (init_str) bs_append(&s, init_str);
    return s;
}

void bs_append(b_string* s, const char* txt) {
    if (!txt) return;
    size_t txt_len = strlen(txt);
    bs_grow(s, txt_len);
    memcpy(s->data + s->len, txt, txt_len);
    s->len += txt_len;
    s->data[s->len] = '\0'; // Always keep null-terminated
}

void bs_append_fmt(b_string* s, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    // Dry run to get length
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (len > 0) {
        bs_grow(s, len);
        va_start(args, fmt);
        vsnprintf(s->data + s->len, len + 1, fmt, args);
        va_end(args);
        s->len += len;
    }
}

void bs_free(b_string* s) {
    if (s->data) free(s->data);
    s->data = NULL; s->len = 0; s->cap = 0;
}
#endif
