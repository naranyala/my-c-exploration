#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ConfigEntry {
  char *key;
  char *value;
  struct ConfigEntry *next;
} ConfigEntry;

typedef struct {
  ConfigEntry *head;
} Config;

Config *config_create() { return calloc(1, sizeof(Config)); }

void config_set(Config *config, const char *key, const char *value) {
  ConfigEntry *entry = malloc(sizeof(ConfigEntry));
  entry->key = strdup(key);
  entry->value = strdup(value);
  entry->next = config->head;
  config->head = entry;
}

char *config_get(Config *config, const char *key) {
  ConfigEntry *current = config->head;
  while (current) {
    if (strcmp(current->key, key) == 0) {
      return current->value;
    }
    current = current->next;
  }
  return NULL;
}

void config_load_from_file(Config *config, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file)
    return;

  char line[512];
  while (fgets(line, sizeof(line), file)) {
    // Remove newline
    line[strcspn(line, "\n")] = 0;

    // Skip comments and empty lines
    char *trimmed = line;
    while (*trimmed && isspace(*trimmed))
      trimmed++;
    if (*trimmed == '#' || *trimmed == 0)
      continue;

    // Parse key=value
    char *equals = strchr(line, '=');
    if (equals) {
      *equals = 0;
      char *key = trimmed;
      char *value = equals + 1;

      // Trim key
      char *end = key + strlen(key) - 1;
      while (end > key && isspace(*end))
        *end-- = 0;

      // Trim value
      while (*value && isspace(*value))
        value++;
      end = value + strlen(value) - 1;
      while (end > value && isspace(*end))
        *end-- = 0;

      config_set(config, key, value);
    }
  }
  fclose(file);
}

void config_free(Config *config) {
  ConfigEntry *current = config->head;
  while (current) {
    ConfigEntry *next = current->next;
    free(current->key);
    free(current->value);
    free(current);
    current = next;
  }
  free(config);
}

int main() {
  Config *config = config_create();

  // Create a sample config file
  FILE *file = fopen("sample.conf", "w");
  fprintf(file, "# Sample config\n");
  fprintf(file, "server_port=8080\n");
  fprintf(file, "database_host=localhost\n");
  fprintf(file, "max_connections=100\n");
  fclose(file);

  config_load_from_file(config, "sample.conf");

  printf("Port: %s\n", config_get(config, "server_port"));
  printf("Host: %s\n", config_get(config, "database_host"));
  printf("Max connections: %s\n", config_get(config, "max_connections"));

  config_free(config);
  remove("sample.conf"); // Clean up
  return 0;
}
