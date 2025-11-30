
// plugin.h
#ifndef PLUGIN_H
#define PLUGIN_H

typedef struct {
  const char *name;
  void (*init)(void);
  void (*run)(void);
  void (*shutdown)(void);
} Plugin;

static inline void plugin_exec(Plugin *p) {
  if (p->init)
    p->init();
  if (p->run)
    p->run();
  if (p->shutdown)
    p->shutdown();
}

#endif
