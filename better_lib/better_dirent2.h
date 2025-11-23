
#ifndef BETTER_DIRENT_H
#define BETTER_DIRENT_H

#include <dirent.h>
#include <stdio.h>

// Iterate files in directory
#define FOR_EACH_FILE(path, entry) \
    for (DIR *dir = opendir(path); \
         dir && (entry = readdir(dir)); ) \
        if (entry->d_name[0] != '.')

#endif // BETTER_DIRENT_H
