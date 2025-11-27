#ifndef SIMPLE_INI_H
#define SIMPLE_INI_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ini_section_s {
    char* name;
    struct ini_key_s* keys;
    struct ini_section_s* next;
} ini_section_t;

typedef struct ini_key_s {
    char* name;
    char* value;
    struct ini_key_s* next;
} ini_key_t;

typedef struct {
    ini_section_t* sections;
} ini_t;

// Parse INI file content into ini_t structure.
// Returns NULL on failure.
ini_t* ini_parse(const char* content);

// Get value from section/key. Returns NULL if not found.
const char* ini_get(const ini_t* ini, const char* section, const char* key);

// Free ini_t and all allocations.
void ini_free(ini_t* ini);

#ifdef __cplusplus
}
#endif

#endif // SIMPLE_INI_H

#ifdef SIMPLE_INI_IMPLEMENTATION

static char* trim(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

static ini_section_t* find_section(ini_section_t* sections, const char* name) {
    for (; sections; sections = sections->next) {
        if (strcmp(sections->name, name) == 0) return sections;
    }
    return NULL;
}

ini_t* ini_parse(const char* content) {
    ini_t* ini = (ini_t*)calloc(1, sizeof(ini_t));
    if (!ini) return NULL;
    
    ini_section_t** cur_section = &ini->sections;
    ini_key_t** cur_key = NULL;
    char* line = NULL;
    size_t linecap = 0;
    size_t pos = 0;
    size_t i;

    while (getline(&line, &linecap, (FILE*)content) > 0) {  // Note: content as FILE*
        // Simple line parsing - assumes content is null-terminated string
        char* original_line = strdup(line);
        char* l = trim(original_line);
        
        if (*l == ';' || *l == '#' || *l == '\0' || *l == '[') {
            if (*l == '[') {
                char* end = strchr(l, ']');
                if (end) {
                    *end = '\0';
                    char* secname = trim(l + 1);
                    ini_section_t* section = (ini_section_t*)calloc(1, sizeof(ini_section_t));
                    if (!section) continue;
                    section->name = strdup(secname);
                    *cur_section = section;
                    cur_section = &section->next;
                    cur_key = &section->keys;
                }
            }
            free(original_line);
            continue;
        }
        
        char* eq = strchr(l, '=');
        if (eq && cur_key) {
            *eq = '\0';
            char* keyname = trim(l);
            char* value = trim(eq + 1);
            
            ini_key_t* key = (ini_key_t*)calloc(1, sizeof(ini_key_t));
            if (key) {
                key->name = strdup(keyname);
                key->value = strdup(value);
                *cur_key = key;
                cur_key = &key->next;
            }
        }
        free(original_line);
    }
    free(line);
    return ini;
}

const char* ini_get(const ini_t* ini, const char* section, const char* key) {
    ini_section_t* sec = find_section(ini->sections, section);
    if (!sec) return NULL;
    
    for (ini_key_t* k = sec->keys; k; k = k->next) {
        if (strcmp(k->name, key) == 0) {
            return k->value;
        }
    }
    return NULL;
}

static void free_keys(ini_key_t* keys) {
    while (keys) {
        ini_key_t* next = keys->next;
        free(keys->name);
        free(keys->value);
        free(keys);
        keys = next;
    }
}

static void free_sections(ini_section_t* sections) {
    while (sections) {
        ini_section_t* next = sections->next;
        free_sections(sections->keys);
        free(sections->name);
        free(sections);
        sections = next;
    }
}

void ini_free(ini_t* ini) {
    if (ini) {
        free_sections(ini->sections);
        free(ini);
    }
}

#endif // SIMPLE_INI_IMPLEMENTATION

