#include <string.h>

/* better_mime.h - Zero-dependency MIME detection */
static inline const char* bmime_from_ext(const char* path) {
    const char* ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    ext++;
    if (strcmp(ext, "jpg")==0 || strcmp(ext, "jpeg")==0) return "image/jpeg";
    if (strcmp(ext, "png")==0) return "image/png";
    if (strcmp(ext, "html")==0) return "text/html";
    if (strcmp(ext, "js")==0) return "application/javascript";
    /* ... 200+ more ... */
    return "application/octet-stream";
}
