#include <string.h>

#ifndef ERGO_PROCMON_H
#define ERGO_PROCMON_H

#include <stdio.h>
#include <stdint.h>

static inline long ergo_proc_mem_kb(void) {
    FILE *f = fopen("/proc/self/status", "r");
    if (!f) return -1;

    long kb = -1;
    char key[32];
    long val;
    while (fscanf(f, "%31s %ld kB", key, &val) == 2) {
        if (strcmp(key, "VmRSS:") == 0) {
            kb = val;
            break;
        }
    }
    fclose(f);
    return kb;
}

static inline double ergo_proc_cpu_load(void) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) return -1;
    double v;
    fscanf(f, "%lf", &v);
    fclose(f);
    return v;
}

#endif

