
/* better_json.h — minimalist JSON token scanner */

#ifndef BETTER_JSON_H
#define BETTER_JSON_H

#include <ctype.h>
#include <string.h>

typedef enum {
    BJ_NONE,
    BJ_OBJ_OPEN,      
    BJ_OBJ_CLOSE,     
    BJ_ARR_OPEN,      
    BJ_ARR_CLOSE,     
    BJ_COLON,         
    BJ_COMMA,         
    BJ_STRING,        
    BJ_NUMBER,        
    BJ_TRUE,          
    BJ_FALSE,         
    BJ_NULL           
} bj_type;

typedef struct {
    bj_type type;
    const char *start;
    size_t len;
} bj_token;

static inline int bj_isnum(char c) {
    return (c >= '0' && c <= '9') || c == '-' || c == '.';
}

static inline const char *bj_next(const char *p, bj_token *tok) {
    while (*p && isspace((unsigned char)*p)) p++;
    if (!*p) { tok->type = BJ_NONE; return p; }

    switch (*p) {
        case '{': tok->type = BJ_OBJ_OPEN;  tok->len = 1; return p + 1;
        case '}': tok->type = BJ_OBJ_CLOSE; tok->len = 1; return p + 1;
        case '[': tok->type = BJ_ARR_OPEN;  tok->len = 1; return p + 1;
        case ']': tok->type = BJ_ARR_CLOSE; tok->len = 1; return p + 1;
        case ':': tok->type = BJ_COLON;     tok->len = 1; return p + 1;
        case ',': tok->type = BJ_COMMA;     tok->len = 1; return p + 1;
    }

    if (*p == '"') {
        tok->type = BJ_STRING;
        tok->start = ++p;
        while (*p && *p != '"') p++;
        tok->len = (size_t)(p - tok->start);
        return *p ? p + 1 : p;
    }

    if (bj_isnum(*p)) {
        tok->type = BJ_NUMBER;
        tok->start = p;
        while (bj_isnum(*p)) p++;
        tok->len = (size_t)(p - tok->start);
        return p;
    }

    if (!strncmp(p, "true", 4))  { tok->type = BJ_TRUE;  tok->len = 4;  return p+4; }
    if (!strncmp(p, "false", 5)) { tok->type = BJ_FALSE; tok->len = 5;  return p+5; }
    if (!strncmp(p, "null", 4))  { tok->type = BJ_NULL;  tok->len = 4;  return p+4; }

    tok->type = BJ_NONE;
    return p + 1;
}

#endif
