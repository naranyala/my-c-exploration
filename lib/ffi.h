#ifndef FFI_H
#define FFI_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* API visibility */
#ifdef FFI_STATIC
#  define FFI_API
#elif defined(_WIN32)
#  define FFI_API __declspec(dllexport)
#else
#  define FFI_API __attribute__((visibility("default")))
#endif

/* FFI-safe result type - fits in register, compatible with Rust Result<isize, isize> */
typedef intptr_t ffi_result;  /* Negative=error code, Non-negative=success/value */

/* Opaque handle - FFI-safe pointer type */
typedef struct ffi_handle {
    uintptr_t _opaque;
} *ffi_handle_t;

/* FFI-safe slice representation (compatible with Rust &[u8]) */
typedef struct ffi_slice {
    const uint8_t* data;
    size_t len;
} ffi_slice_t;

/* FFI-safe mutable slice (compatible with Rust &mut [u8]) */
typedef struct ffi_slice_mut {
    uint8_t* data;
    size_t len;
} ffi_slice_mut_t;

/* FFI-safe string view (compatible with Rust &str, always UTF-8) */
typedef struct ffi_str {
    const char* data;
    size_t len;
} ffi_str_t;

/* FFI-safe option type (compatible with Rust Option<T>) */
typedef struct ffi_option_handle {
    ffi_handle_t value;
    bool is_some;
} ffi_option_handle_t;

/* Error codes - negative for FFI safety */
#define FFI_OK 0
#define FFI_ERR_ARGS -1
#define FFI_ERR_MEMORY -2
#define FFI_ERR_NULL_PTR -3
#define FFI_ERR_INVALID_UTF8 -4
#define FFI_ERR_BUFFER_TOO_SMALL -5
#define FFI_ERR_NOT_INITIALIZED -6
#define FFI_ERR_ALREADY_EXISTS -7

/* Configuration structure - #[repr(C)] compatible */
typedef struct ffi_config {
    uint32_t version;        /* API version for compatibility */
    uint32_t flags;          /* Configuration flags */
    size_t buffer_size;      /* Internal buffer size */
    void* user_data;         /* User-provided context pointer */
} ffi_config_t;

/* Callback types - FFI-safe function pointers */
typedef void (*ffi_callback_fn)(void* user_data, ffi_slice_t data);
typedef ffi_result (*ffi_transform_fn)(ffi_slice_t input, ffi_slice_mut_t output);

/* Core initialization API */
FFI_API ffi_result ffi_init(void);
FFI_API void ffi_deinit(void);
FFI_API ffi_result ffi_version(uint32_t* major, uint32_t* minor, uint32_t* patch);

/* Handle management */
FFI_API ffi_result ffi_create(ffi_handle_t* handle, const ffi_config_t* config);
FFI_API void ffi_destroy(ffi_handle_t handle);
FFI_API ffi_result ffi_clone(ffi_handle_t src, ffi_handle_t* dst);
FFI_API bool ffi_is_valid(ffi_handle_t handle);

/* Data processing - zero-copy when possible */
FFI_API ffi_result ffi_process(
    ffi_handle_t handle, 
    ffi_slice_t input,
    ffi_slice_mut_t output,
    size_t* bytes_written  /* out: actual bytes written */
);

FFI_API ffi_result ffi_process_inplace(
    ffi_handle_t handle,
    ffi_slice_mut_t buffer,
    size_t* new_len  /* out: new length after processing */
);

/* String handling - UTF-8 aware */
FFI_API ffi_result ffi_process_str(
    ffi_handle_t handle,
    ffi_str_t input,
    char* output,
    size_t output_capacity,
    size_t* bytes_written
);

/* Callback registration */
FFI_API ffi_result ffi_set_callback(
    ffi_handle_t handle,
    ffi_callback_fn callback,
    void* user_data
);

/* Error handling - thread-safe */
FFI_API const char* ffi_error_message(ffi_result result);
FFI_API ffi_result ffi_last_error(void);
FFI_API void ffi_clear_error(void);

/* Memory management helpers - for Rust Box/Vec interop */
FFI_API void* ffi_alloc(size_t size, size_t align);
FFI_API void ffi_free(void* ptr, size_t size, size_t align);

/* Utility functions */
FFI_API ffi_slice_t ffi_slice_from_raw(const uint8_t* data, size_t len);
FFI_API ffi_str_t ffi_str_from_cstr(const char* cstr);  /* NULL-terminated */
FFI_API bool ffi_str_is_valid_utf8(ffi_str_t str);

#ifdef __cplusplus
}
#endif

/* STB-style implementation */
#ifdef FFI_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FFI_VERSION_MAJOR 1
#define FFI_VERSION_MINOR 0
#define FFI_VERSION_PATCH 0

/* Thread-local error storage */
#ifndef FFI_NO_THREAD_LOCAL
#  ifdef _MSC_VER
#    define FFI_THREAD_LOCAL __declspec(thread)
#  else
#    define FFI_THREAD_LOCAL __thread
#  endif
#else
#  define FFI_THREAD_LOCAL
#endif

static FFI_THREAD_LOCAL ffi_result g_last_error = FFI_OK;

static struct {
    int initialized;
    size_t handle_count;
} g_ffi = {0, 0};

/* Internal handle structure */
struct ffi_handle_impl {
    uint32_t magic;  /* For validation */
    ffi_config_t config;
    ffi_callback_fn callback;
    void* callback_user_data;
    uint8_t* internal_buffer;
};

#define FFI_MAGIC 0x46464943  /* "FFIC" */

static void ffi_set_error(ffi_result err) {
    g_last_error = err;
}

FFI_API ffi_result ffi_init(void) {
    if (g_ffi.initialized) {
        ffi_set_error(FFI_ERR_ALREADY_EXISTS);
        return FFI_ERR_ALREADY_EXISTS;
    }
    g_ffi.initialized = 1;
    g_ffi.handle_count = 0;
    ffi_set_error(FFI_OK);
    return FFI_OK;
}

FFI_API void ffi_deinit(void) {
    g_ffi.initialized = 0;
    g_ffi.handle_count = 0;
    ffi_set_error(FFI_OK);
}

FFI_API ffi_result ffi_version(uint32_t* major, uint32_t* minor, uint32_t* patch) {
    if (!major || !minor || !patch) {
        ffi_set_error(FFI_ERR_NULL_PTR);
        return FFI_ERR_NULL_PTR;
    }
    *major = FFI_VERSION_MAJOR;
    *minor = FFI_VERSION_MINOR;
    *patch = FFI_VERSION_PATCH;
    return FFI_OK;
}

FFI_API ffi_result ffi_create(ffi_handle_t* handle, const ffi_config_t* config) {
    if (!handle) {
        ffi_set_error(FFI_ERR_NULL_PTR);
        return FFI_ERR_NULL_PTR;
    }
    if (!g_ffi.initialized) {
        ffi_set_error(FFI_ERR_NOT_INITIALIZED);
        return FFI_ERR_NOT_INITIALIZED;
    }
    
    struct ffi_handle_impl* impl = (struct ffi_handle_impl*)calloc(1, sizeof(struct ffi_handle_impl));
    if (!impl) {
        ffi_set_error(FFI_ERR_MEMORY);
        return FFI_ERR_MEMORY;
    }
    
    impl->magic = FFI_MAGIC;
    if (config) {
        impl->config = *config;
        if (config->buffer_size > 0) {
            impl->internal_buffer = (uint8_t*)malloc(config->buffer_size);
            if (!impl->internal_buffer) {
                free(impl);
                ffi_set_error(FFI_ERR_MEMORY);
                return FFI_ERR_MEMORY;
            }
        }
    }
    
    *handle = (ffi_handle_t)impl;
    g_ffi.handle_count++;
    ffi_set_error(FFI_OK);
    return FFI_OK;
}

FFI_API void ffi_destroy(ffi_handle_t handle) {
    if (!handle) return;
    
    struct ffi_handle_impl* impl = (struct ffi_handle_impl*)handle;
    if (impl->magic != FFI_MAGIC) return;
    
    if (impl->internal_buffer) {
        free(impl->internal_buffer);
    }
    impl->magic = 0;
    free(impl);
    
    if (g_ffi.handle_count > 0) {
        g_ffi.handle_count--;
    }
}

FFI_API ffi_result ffi_clone(ffi_handle_t src, ffi_handle_t* dst) {
    if (!src || !dst) {
        ffi_set_error(FFI_ERR_NULL_PTR);
        return FFI_ERR_NULL_PTR;
    }
    
    struct ffi_handle_impl* src_impl = (struct ffi_handle_impl*)src;
    if (src_impl->magic != FFI_MAGIC) {
        ffi_set_error(FFI_ERR_ARGS);
        return FFI_ERR_ARGS;
    }
    
    return ffi_create(dst, &src_impl->config);
}

FFI_API bool ffi_is_valid(ffi_handle_t handle) {
    if (!handle) return false;
    struct ffi_handle_impl* impl = (struct ffi_handle_impl*)handle;
    return impl->magic == FFI_MAGIC;
}

FFI_API ffi_result ffi_process(
    ffi_handle_t handle,
    ffi_slice_t input,
    ffi_slice_mut_t output,
    size_t* bytes_written
) {
    if (!handle || !input.data || !output.data || !bytes_written) {
        ffi_set_error(FFI_ERR_NULL_PTR);
        return FFI_ERR_NULL_PTR;
    }
    
    struct ffi_handle_impl* impl = (struct ffi_handle_impl*)handle;
    if (impl->magic != FFI_MAGIC) {
        ffi_set_error(FFI_ERR_ARGS);
        return FFI_ERR_ARGS;
    }
    
    if (output.len < input.len) {
        ffi_set_error(FFI_ERR_BUFFER_TOO_SMALL);
        return FFI_ERR_BUFFER_TOO_SMALL;
    }
    
    memcpy(output.data, input.data, input.len);
    *bytes_written = input.len;
    
    if (impl->callback) {
        impl->callback(impl->callback_user_data, input);
    }
    
    ffi_set_error(FFI_OK);
    return FFI_OK;
}

FFI_API ffi_result ffi_process_inplace(
    ffi_handle_t handle,
    ffi_slice_mut_t buffer,
    size_t* new_len
) {
    if (!handle || !buffer.data || !new_len) {
        ffi_set_error(FFI_ERR_NULL_PTR);
        return FFI_ERR_NULL_PTR;
    }
    
    struct ffi_handle_impl* impl = (struct ffi_handle_impl*)handle;
    if (impl->magic != FFI_MAGIC) {
        ffi_set_error(FFI_ERR_ARGS);
        return FFI_ERR_ARGS;
    }
    
    *new_len = buffer.len;
    ffi_set_error(FFI_OK);
    return FFI_OK;
}

FFI_API ffi_result ffi_process_str(
    ffi_handle_t handle,
    ffi_str_t input,
    char* output,
    size_t output_capacity,
    size_t* bytes_written
) {
    if (!handle || !input.data || !output || !bytes_written) {
        ffi_set_error(FFI_ERR_NULL_PTR);
        return FFI_ERR_NULL_PTR;
    }
    
    if (!ffi_str_is_valid_utf8(input)) {
        ffi_set_error(FFI_ERR_INVALID_UTF8);
        return FFI_ERR_INVALID_UTF8;
    }
    
    if (output_capacity < input.len + 1) {
        ffi_set_error(FFI_ERR_BUFFER_TOO_SMALL);
        return FFI_ERR_BUFFER_TOO_SMALL;
    }
    
    memcpy(output, input.data, input.len);
    output[input.len] = '\0';
    *bytes_written = input.len;
    
    ffi_set_error(FFI_OK);
    return FFI_OK;
}

FFI_API ffi_result ffi_set_callback(
    ffi_handle_t handle,
    ffi_callback_fn callback,
    void* user_data
) {
    if (!handle) {
        ffi_set_error(FFI_ERR_NULL_PTR);
        return FFI_ERR_NULL_PTR;
    }
    
    struct ffi_handle_impl* impl = (struct ffi_handle_impl*)handle;
    if (impl->magic != FFI_MAGIC) {
        ffi_set_error(FFI_ERR_ARGS);
        return FFI_ERR_ARGS;
    }
    
    impl->callback = callback;
    impl->callback_user_data = user_data;
    ffi_set_error(FFI_OK);
    return FFI_OK;
}

FFI_API const char* ffi_error_message(ffi_result result) {
    switch (result) {
        case FFI_OK: return "success";
        case FFI_ERR_ARGS: return "invalid arguments";
        case FFI_ERR_MEMORY: return "out of memory";
        case FFI_ERR_NULL_PTR: return "null pointer";
        case FFI_ERR_INVALID_UTF8: return "invalid UTF-8";
        case FFI_ERR_BUFFER_TOO_SMALL: return "buffer too small";
        case FFI_ERR_NOT_INITIALIZED: return "not initialized";
        case FFI_ERR_ALREADY_EXISTS: return "already exists";
        default: return "unknown error";
    }
}

FFI_API ffi_result ffi_last_error(void) {
    return g_last_error;
}

FFI_API void ffi_clear_error(void) {
    g_last_error = FFI_OK;
}

FFI_API void* ffi_alloc(size_t size, size_t align) {
    (void)align;  /* Standard malloc alignment is usually sufficient */
    void* ptr = malloc(size);
    if (!ptr) {
        ffi_set_error(FFI_ERR_MEMORY);
    }
    return ptr;
}

FFI_API void ffi_free(void* ptr, size_t size, size_t align) {
    (void)size;
    (void)align;
    free(ptr);
}

FFI_API ffi_slice_t ffi_slice_from_raw(const uint8_t* data, size_t len) {
    ffi_slice_t slice = {data, len};
    return slice;
}

FFI_API ffi_str_t ffi_str_from_cstr(const char* cstr) {
    ffi_str_t str = {cstr, cstr ? strlen(cstr) : 0};
    return str;
}

FFI_API bool ffi_str_is_valid_utf8(ffi_str_t str) {
    if (!str.data) return false;
    
    const uint8_t* bytes = (const uint8_t*)str.data;
    size_t i = 0;
    
    while (i < str.len) {
        if (bytes[i] <= 0x7F) {
            i++;
        } else if ((bytes[i] & 0xE0) == 0xC0) {
            if (i + 1 >= str.len || (bytes[i + 1] & 0xC0) != 0x80) return false;
            i += 2;
        } else if ((bytes[i] & 0xF0) == 0xE0) {
            if (i + 2 >= str.len || (bytes[i + 1] & 0xC0) != 0x80 || (bytes[i + 2] & 0xC0) != 0x80) return false;
            i += 3;
        } else if ((bytes[i] & 0xF8) == 0xF0) {
            if (i + 3 >= str.len || (bytes[i + 1] & 0xC0) != 0x80 || (bytes[i + 2] & 0xC0) != 0x80 || (bytes[i + 3] & 0xC0) != 0x80) return false;
            i += 4;
        } else {
            return false;
        }
    }
    
    return true;
}

#endif /* FFI_IMPLEMENTATION */
#endif /* FFI_H */
