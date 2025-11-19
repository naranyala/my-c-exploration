int ini_parse(const char *filename,
              int (*handler)(void *, const char *, const char *, const char *),
              void *user) {
  FILE *f = fopen(filename, "r");
  char line[512], section[128] = "";
  while (fgets(line, sizeof(line), f)) {
    char *key = line, *value = line;
    if (sscanf(line, "[%127[^]]]", section) == 1)
      continue;
    if (strchr(key, '=') && (value = strchr(key, '='))) {
      *value++ = '\0';
      trim(key);
      trim(value);
      if (handler(user, section, key, value))
        return -1;
    }
  }
  fclose(f);
  return 0;
}
