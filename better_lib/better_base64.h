// better_base64.h
#ifndef BETTER_BASE64_H
#define BETTER_BASE64_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Encode binary data to Base64 (null-terminated string)
// Returns malloc'd string, or NULL on error. Use b_free().
char* b_base64_encode(const unsigned char* data, size_t len);

// Decode Base64 string to binary
// Returns malloc'd buffer, sets *out_len. Returns NULL on error.
unsigned char* b_base64_decode(const char* b64, size_t* out_len);

#ifdef __cplusplus
}
#endif

#ifdef BETTER_BASE64_IMPL

#include <stdlib.h>
#include <string.h>

static const char b64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Reverse lookup table
static unsigned char b64_reverse[256];

static void b64_init_reverse(void) {
    static int inited = 0;
    if (inited) return;
    for (int i = 0; i < 256; ++i) b64_reverse[i] = 64; // invalid
    for (int i = 0; i < 64; ++i) b64_reverse[(unsigned char)b64_table[i]] = i;
    b64_reverse['='] = 0; // padding treated as 0
    inited = 1;
}

char* b_base64_encode(const unsigned char* data, size_t len) {
    if (!data) return NULL;
    size_t out_len = 4 * ((len + 2) / 3);
    char* out = (char*)malloc(out_len + 1);
    if (!out) return NULL;

    for (size_t i = 0, j = 0; i < len;) {
        uint32_t octet_a = i < len ? data[i++] : 0;
        uint32_t octet_b = i < len ? data[i++] : 0;
        uint32_t octet_c = i < len ? data[i++] : 0;
        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;
        out[j++] = b64_table[(triple >> 18) & 63];
        out[j++] = b64_table[(triple >> 12) & 63];
        out[j++] = b64_table[(triple >> 6) & 63];
        out[j++] = b64_table[(triple >> 0) & 63];
    }

    // Padding
    size_t pad = (3 - (len % 3)) % 3;
    for (size_t i = 0; i < pad; ++i) out[out_len - 1 - i] = '=';
    out[out_len] = '\0';
    return out;
}

unsigned char* b_base64_decode(const char* b64, size_t* out_len) {
    if (!b64) return NULL;
    b64_init_reverse();

    size_t len = strlen(b64);
    if (len % 4 != 0) return NULL;

    // Count padding
    size_t pad = 0;
    if (len >= 1 && b64[len - 1] == '=') pad++;
    if (len >= 2 && b64[len - 2] == '=') pad++;

    size_t out_size = (len / 4) * 3 - pad;
    unsigned char* out = (unsigned char*)malloc(out_size + 1);
    if (!out) return NULL;

    for (size_t i = 0, j = 0; i < len;) {
        uint32_t sextet_a = b64_reverse[(unsigned char)b64[i++]];
        uint32_t sextet_b = b64_reverse[(unsigned char)b64[i++]];
        uint32_t sextet_c = b64_reverse[(unsigned char)b64[i++]];
        uint32_t sextet_d = b64_reverse[(unsigned char)b64[i++]];
        if (sextet_a > 63 || sextet_b > 63 || sextet_c > 63 || sextet_d > 63) {
            free(out);
            return NULL;
        }
        uint32_t triple = (sextet_a << 18) | (sextet_b << 12) |
                          (sextet_c << 6) | sextet_d;
        if (j < out_size) out[j++] = (triple >> 16) & 0xFF;
        if (j < out_size) out[j++] = (triple >> 8) & 0xFF;
        if (j < out_size) out[j++] = triple & 0xFF;
    }
    out[out_size] = '\0';
    if (out_len) *out_len = out_size;
    return out;
}

#endif // BETTER_BASE64_IMPL

#endif // BETTER_BASE64_H
