#ifndef SIMPLE_JSON_H
#define SIMPLE_JSON_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    JSON_NULL, JSON_TRUE, JSON_FALSE,
    JSON_NUMBER, JSON_STRING,
    JSON_ARRAY, JSON_OBJECT
} json_type_e;

typedef struct json_value_s json_value_t;
typedef struct json_object_element_s {
    char* key;
    json_value_t* value;
    struct json_object_element_s* next;
} json_object_element_t;

typedef struct json_array_element_s {
    json_value_t* value;
    struct json_array_element_s* next;
} json_array_element_t;

struct json_value_s {
    json_type_e type;
    union {
        double number;
        char* string;
        json_object_element_t* object;
        json_array_element_t* array;
    } val;
};

// Parse JSON text into json_value_t.
// Returns NULL on failure.
json_value_t* json_parse(const char* src);

// Free json_value_t and all nested allocations.
void json_free(json_value_t* root);

#ifdef __cplusplus
}
#endif

#endif // SIMPLE_JSON_H

#ifdef SIMPLE_JSON_IMPLEMENTATION

#include <stdio.h>

static const char* skip_ws(const char* s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

static json_value_t* parse_value(const char** src);

static char* parse_string(const char** src) {
    const char* s = *src;
    if (*s != '"') return NULL;
    s++;
    const char* start = s;
    while (*s && *s != '"') {
        if (*s == '\\') s++; 
        s++;
    }
    if (*s != '"') return NULL;
    size_t len = s - start;
    char* str = (char*)malloc(len + 1);
    if (!str) return NULL;
    strncpy(str, start, len);
    str[len] = '\0';
    *src = s + 1;
    return str;
}

static json_object_element_t* parse_object(const char** src) {
    const char* s = *src;
    if (*s != '{') return NULL;
    s = skip_ws(s + 1);

    json_object_element_t* head = NULL;
    json_object_element_t** cur = &head;

    if (*s == '}') {
        *src = s + 1;
        return NULL; // Empty object
    }

    while (1) {
        json_object_element_t* elem = (json_object_element_t*)malloc(sizeof(json_object_element_t));
        if (!elem) return NULL;
        memset(elem, 0, sizeof(*elem));
        elem->key = parse_string(&s);
        if (!elem->key) {
            free(elem);
            break;
        }
        s = skip_ws(s);
        if (*s != ':') {
            free(elem->key);
            free(elem);
            break;
        }
        s = skip_ws(s + 1);
        elem->value = parse_value(&s);
        if (!elem->value) {
            free(elem->key);
            free(elem);
            break;
        }
        *cur = elem;
        cur = &elem->next;
        s = skip_ws(s);
        if (*s == ',') {
            s++;
            s = skip_ws(s);
            continue;
        } else if (*s == '}') {
            s++;
            break;
        } else {
            break;
        }
    }
    *cur = NULL;
    *src = s;
    return head;
}

static json_array_element_t* parse_array(const char** src) {
    const char* s = *src;
    if (*s != '[') return NULL;
    s = skip_ws(s + 1);

    json_array_element_t* head = NULL;
    json_array_element_t** cur = &head;

    if (*s == ']') {
        *src = s + 1;
        return NULL; // empty array
    }

    while (1) {
        json_array_element_t* elem = (json_array_element_t*)malloc(sizeof(json_array_element_t));
        if (!elem) return NULL;
        memset(elem, 0, sizeof(*elem));
        elem->value = parse_value(&s);
        if (!elem->value) {
            free(elem);
            break;
        }
        *cur = elem;
        cur = &elem->next;
        s = skip_ws(s);
        if (*s == ',') {
            s++;
            s = skip_ws(s);
            continue;
        } else if (*s == ']') {
            s++;
            break;
        } else {
            break;
        }
    }
    *cur = NULL;
    *src = s;
    return head;
}

static json_value_t* parse_value(const char** src) {
    const char* s = skip_ws(*src);
    if (*s == '"') {
        json_value_t* val = (json_value_t*)malloc(sizeof(json_value_t));
        if (!val) return NULL;
        val->type = JSON_STRING;
        val->val.string = parse_string(&s);
        if (!val->val.string) {
            free(val);
            return NULL;
        }
        *src = s;
        return val;
    } else if (strncmp(s, "null", 4) == 0) {
        json_value_t* val = (json_value_t*)malloc(sizeof(json_value_t));
        if (!val) return NULL;
        val->type = JSON_NULL;
        val->val.string = NULL;
        *src = s + 4;
        return val;
    } else if (strncmp(s, "true", 4) == 0) {
        json_value_t* val = (json_value_t*)malloc(sizeof(json_value_t));
        if (!val) return NULL;
        val->type = JSON_TRUE;
        val->val.string = NULL;
        *src = s + 4;
        return val;
    } else if (strncmp(s, "false", 5) == 0) {
        json_value_t* val = (json_value_t*)malloc(sizeof(json_value_t));
        if (!val) return NULL;
        val->type = JSON_FALSE;
        val->val.string = NULL;
        *src = s + 5;
        return val;
    } else if (*s == '{') {
        json_value_t* val = (json_value_t*)malloc(sizeof(json_value_t));
        if (!val) return NULL;
        val->type = JSON_OBJECT;
        val->val.object = parse_object(&s);
        *src = s;
        return val;
    } else if (*s == '[') {
        json_value_t* val = (json_value_t*)malloc(sizeof(json_value_t));
        if (!val) return NULL;
        val->type = JSON_ARRAY;
        val->val.array = parse_array(&s);
        *src = s;
        return val;
    } else {
        char* endptr;
        double number = strtod(s, &endptr);
        if (endptr == s) return NULL;  // not a number
        json_value_t* val = (json_value_t*)malloc(sizeof(json_value_t));
        if (!val) return NULL;
        val->type = JSON_NUMBER;
        val->val.number = number;
        *src = endptr;
        return val;
    }
}

json_value_t* json_parse(const char* src) {
    const char* s = src;
    json_value_t* root = parse_value(&s);
    if (!root) return NULL;
    s = skip_ws(s);
    if (*s != '\0') {
        json_free(root);
        return NULL;
    }
    return root;
}

static void free_object(json_object_element_t* obj) {
    while (obj) {
        json_object_element_t* next = obj->next;
        free(obj->key);
        json_free(obj->value);
        free(obj);
        obj = next;
    }
}

static void free_array(json_array_element_t* arr) {
    while (arr) {
        json_array_element_t* next = arr->next;
        json_free(arr->value);
        free(arr);
        arr = next;
    }
}

void json_free(json_value_t* root) {
    if (!root) return;
    switch (root->type) {
        case JSON_STRING:
            free(root->val.string);
            break;
        case JSON_OBJECT:
            free_object(root->val.object);
            break;
        case JSON_ARRAY:
            free_array(root->val.array);
            break;
        default:
            break;
    }
    free(root);
}

#endif // SIMPLE_JSON_IMPLEMENTATION

