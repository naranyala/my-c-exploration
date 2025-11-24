#ifndef ENVVIEW_H
#define ENVVIEW_H

typedef struct {
    char **vars;
    int count;
} envview;

envview envview_snapshot(void);
void envview_free(envview v);
const char *envview_key(const char *kv);
const char *envview_value(const char *kv);

#ifdef ENVVIEW_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>

#if _WIN32
extern char **_environ;
#define ENV _environ
#else
extern char **environ;
#define ENV environ
#endif

envview envview_snapshot(void) {
    int n = 0;
    while (ENV[n]) n++;

    envview v;
    v.vars = malloc(sizeof(char*) * n);
    v.count = n;

    for (int i = 0; i < n; i++)
        v.vars[i] = strdup(ENV[i]);

    return v;
}

void envview_free(envview v) {
    for (int i = 0; i < v.count; i++) free(v.vars[i]);
    free(v.vars);
}

const char *envview_key(const char *kv) {
    const char *p = strchr(kv, '=');
    if (!p) return kv;
    static char buf[256];
    size_t n = p - kv;
    if (n >= sizeof(buf)) n = sizeof(buf)-1;
    memcpy(buf, kv, n);
    buf[n] = 0;
    return buf;
}
const char *envview_value(const char *kv) {
    const char *p = strchr(kv, '=');
    return p ? p+1 : "";
}

#endif
#endif

