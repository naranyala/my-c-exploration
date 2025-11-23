/* Usage */
#include "ergo_path3.h"
int main() {
    char *full = path_join("/home/user", "docs/file.txt");
    printf("%s → dir=%s, base=%s\n", full, path_dirname(full), path_basename(full));
    free(full);
}

