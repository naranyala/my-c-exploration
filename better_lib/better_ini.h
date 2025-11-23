
/* better_ini.h — micro INI parser */

#ifndef BETTER_INI_H
#define BETTER_INI_H

#include <string.h>
#include <ctype.h>

static inline void bini_trim(char *s) {
    char *p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    memmove(s, p, strlen(p) + 1);

    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1])) end--;
    *end = 0;
}

static inline void bini_parse_line(char *line,
    void (*on_kv)(const char *section, const char *key,
                  const char *val, void *ud),
    char *section, size_t ssz,
    void *ud)
{
    bini_trim(line);
    if (!line[0] || line[0] == '#')
        return;

    if (line[0] == '[') {
        char *r = strchr(line, ']');
        if (r) {
            *r = 0;
            strncpy(section, line + 1, ssz);
        }
        return;
    }

    char *eq = strchr(line, '=');
    if (!eq) return;

    *eq = 0;
    char *key = line;
    char *val = eq + 1;
    bini_trim(key);
    bini_trim(val);

    on_kv(section, key, val, ud);
}

static inline void bini_parse(const char *text,
    void (*on_kv)(const char *section, const char *key,
                  const char *val, void *ud),
    void *ud)
{
    char section[64] = "";
    char buf[512];
    const char *p = text;

    while (*p) {
        size_t n = 0;
        while (p[n] && p[n] != '\n' && n < sizeof(buf)-1)
            n++;
        memcpy(buf, p, n);
        buf[n] = 0;

        bini_parse_line(buf, on_kv, section, sizeof(section), ud);
        p += n;
        if (*p == '\n') p++;
    }
}

#endif
