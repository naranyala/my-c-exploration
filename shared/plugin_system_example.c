/**
 * Simple plugin system using dynamic loading
 */

#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PLUGINS 10
#define PLUGIN_DIR "./plugins"

typedef struct {
  char name[64];
  void *handle;
  int (*initialize)(void);
  void (*process)(const char *data);
  void (*cleanup)(void);
} Plugin;

typedef struct {
  Plugin plugins[MAX_PLUGINS];
  int count;
} PluginManager;

PluginManager *plugin_manager_create(void) {
  PluginManager *pm = malloc(sizeof(PluginManager));
  if (!pm)
    return NULL;

  pm->count = 0;
  return pm;
}

int plugin_manager_load_plugins(PluginManager *pm, const char *directory) {
  DIR *dir = opendir(directory);
  if (!dir) {
    perror("Failed to open plugin directory");
    return 0;
  }

  struct dirent *entry;
  int loaded = 0;

  while ((entry = readdir(dir)) != NULL && pm->count < MAX_PLUGINS) {
    // Look for shared library files
    if (strstr(entry->d_name, ".so") != NULL) {
      char path[256];
      snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);

      void *handle = dlopen(path, RTLD_LAZY);
      if (!handle) {
        fprintf(stderr, "Failed to load plugin %s: %s\n", entry->d_name,
                dlerror());
        continue;
      }

      Plugin *plugin = &pm->plugins[pm->count];
      strncpy(plugin->name, entry->d_name, sizeof(plugin->name) - 1);
      plugin->handle = handle;

      // Load plugin functions
      plugin->initialize = dlsym(handle, "initialize");
      plugin->process = dlsym(handle, "process");
      plugin->cleanup = dlsym(handle, "cleanup");

      if (plugin->initialize && plugin->initialize() == 0) {
        pm->count++;
        loaded++;
        printf("Loaded plugin: %s\n", plugin->name);
      } else {
        dlclose(handle);
        fprintf(stderr, "Failed to initialize plugin: %s\n", plugin->name);
      }
    }
  }

  closedir(dir);
  return loaded;
}

void plugin_manager_process_all(PluginManager *pm, const char *data) {
  for (int i = 0; i < pm->count; i++) {
    if (pm->plugins[i].process) {
      pm->plugins[i].process(data);
    }
  }
}

void plugin_manager_destroy(PluginManager *pm) {
  for (int i = 0; i < pm->count; i++) {
    if (pm->plugins[i].cleanup) {
      pm->plugins[i].cleanup();
    }
    dlclose(pm->plugins[i].handle);
  }
  free(pm);
}

// Example plugin interface (to be implemented in separate .so files)
/*
// plugin_example.c
#include <stdio.h>

int initialize(void) {
    printf("Example plugin initialized\n");
    return 0;
}

void process(const char *data) {
    printf("Example plugin processing: %s\n", data);
}

void cleanup(void) {
    printf("Example plugin cleaned up\n");
}
*/

// Example usage
int main(void) {
  PluginManager *pm = plugin_manager_create();

  printf("Loading plugins from %s\n", PLUGIN_DIR);
  int loaded = plugin_manager_load_plugins(pm, PLUGIN_DIR);
  printf("Loaded %d plugins\n", loaded);

  plugin_manager_process_all(pm, "Hello, Plugin System!");

  plugin_manager_destroy(pm);
  return 0;
}
