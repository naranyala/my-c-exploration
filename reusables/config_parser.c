/**
 * Configuration file parser
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_CONFIG_ENTRIES 100

typedef struct {
  char key[64];
  char value[128];
} ConfigEntry;

typedef struct {
  ConfigEntry entries[MAX_CONFIG_ENTRIES];
  int count;
} Config;

void config_init(Config *config) { config->count = 0; }

int config_parse_file(Config *config, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening config file");
    return 0;
  }

  char line[MAX_LINE_LENGTH];
  int line_num = 0;

  while (fgets(line, sizeof(line), file)) {
    line_num++;

    // Remove trailing newline
    line[strcspn(line, "\n")] = 0;

    // Skip empty lines and comments
    if (line[0] == '\0' || line[0] == '#' || line[0] == ';') {
      continue;
    }

    // Find key and value
    char *equals = strchr(line, '=');
    if (!equals) {
      fprintf(stderr, "Invalid config line %d: %s\n", line_num, line);
      continue;
    }

    *equals = '\0';
    char *key = line;
    char *value = equals + 1;

    // Trim whitespace
    while (isspace(*key))
      key++;
    char *key_end = key + strlen(key) - 1;
    while (key_end > key && isspace(*key_end))
      *key_end-- = '\0';

    while (isspace(*value))
      value++;
    char *value_end = value + strlen(value) - 1;
    while (value_end > value && isspace(*value_end))
      *value_end-- = '\0';

    // Store config entry
    if (config->count < MAX_CONFIG_ENTRIES) {
      strncpy(config->entries[config->count].key, key,
              sizeof(config->entries[0].key) - 1);
      strncpy(config->entries[config->count].value, value,
              sizeof(config->entries[0].value) - 1);
      config->count++;
    }
  }

  fclose(file);
  return 1;
}

const char *config_get_value(const Config *config, const char *key) {
  for (int i = 0; i < config->count; i++) {
    if (strcmp(config->entries[i].key, key) == 0) {
      return config->entries[i].value;
    }
  }
  return NULL;
}

int config_get_int(const Config *config, const char *key, int default_value) {
  const char *value = config_get_value(config, key);
  return value ? atoi(value) : default_value;
}

double config_get_double(const Config *config, const char *key,
                         double default_value) {
  const char *value = config_get_value(config, key);
  return value ? atof(value) : default_value;
}

// Example usage
int main(void) {
  Config config;
  config_init(&config);

  if (config_parse_file(&config, "config.txt")) {
    const char *server = config_get_value(&config, "server");
    int port = config_get_int(&config, "port", 8080);
    double timeout = config_get_double(&config, "timeout", 30.0);

    printf("Server: %s\n", server ? server : "Not found");
    printf("Port: %d\n", port);
    printf("Timeout: %.2f\n", timeout);
  }

  return 0;
}
