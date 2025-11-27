#ifndef C_URL_H
#define C_URL_H

#include <string.h>
#include <stdio.h>

// --- Structure Definitions ---

typedef struct {
    char *scheme; // e.g., "http"
    char *host;   // e.g., "www.example.com"
    char *port;   // e.g., "8080"
    char *path;   // e.g., "/path/to/resource"
    char *query;  // e.g., "key=value"
    char *fragment; // e.g., "anchor"
} c_url_t;

// --- Function Declarations ---

// Parses the given URL string *in-place*. The original string will be modified 
// (null terminators inserted) but can be restored using the return value.
// Returns the original pointer if successful, NULL on failure.
char *c_url_parse_inplace(char *url_str, c_url_t *out_url);

// --- Implementation Section ---
#ifdef C_URL_IMPLEMENTATION

char *c_url_parse_inplace(char *url_str, c_url_t *out_url) {
    if (!url_str || !out_url) return NULL;

    char *original_str = url_str;
    memset(out_url, 0, sizeof(c_url_t));

    // 1. SCHEME (e.g., http://)
    char *scheme_end = strstr(url_str, "://");
    if (scheme_end) {
        *scheme_end = '\0'; // Null-terminate the scheme
        out_url->scheme = url_str;
        url_str = scheme_end + 3; // Start after "://"
    } else {
        // If no scheme, assume it starts with host/path
        out_url->scheme = ""; // Empty string for consistency
    }

    // 2. FRAGMENT (#)
    char *fragment_start = strchr(url_str, '#');
    if (fragment_start) {
        *fragment_start = '\0'; // Null-terminate everything before fragment
        out_url->fragment = fragment_start + 1;
    }

    // 3. QUERY (?)
    char *query_start = strchr(url_str, '?');
    if (query_start) {
        *query_start = '\0'; // Null-terminate everything before query
        out_url->query = query_start + 1;
    }
    
    // Remaining string is AUTHORITY (host[:port]) and PATH
    char *path_start = strchr(url_str, '/');
    char *host_end = url_str + strlen(url_str); // Default end of host is end of string

    if (path_start) {
        host_end = path_start;
        *path_start = '\0'; // Null-terminate host
        out_url->path = path_start + 1;
    } else {
        out_url->path = ""; // Empty path
    }

    // 4. HOST and PORT
    char *port_start = strchr(url_str, ':');
    if (port_start && port_start < host_end) {
        *port_start = '\0'; // Null-terminate host
        out_url->host = url_str;
        out_url->port = port_start + 1;
    } else {
        out_url->host = url_str;
        out_url->port = ""; // Empty port
    }
    
    // Add the leading slash back to the path for consistency if needed
    if (path_start) {
        *path_start = '/';
    }

    return original_str;
}

#endif // C_URL_IMPLEMENTATION

#endif // C_URL_H
