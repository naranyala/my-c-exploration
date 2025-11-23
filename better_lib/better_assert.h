/* better_assert.h v1.4 - Colored assert with backtrace */
#ifndef BETTER_ASSERT_H
#define BETTER_ASSERT_H
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifdef __GNUC__
#include <execinfo.h>
#define BA_HAVE_BACKTRACE
#endif

static inline void ba_print_backtrace(void) {
#ifdef BA_HAVE_BACKTRACE
    void* buffer[100];
    int nptrs = backtrace(buffer, 100);
    char** strings = backtrace_symbols(buffer, nptrs);
    if (strings) {
        fprintf(stderr, "\033[36mBacktrace:\033[0m\n");
        for (int i = 1; i < nptrs; i++)  // skip this function
            fprintf(stderr, "  %s\n", strings[i]);
        free(strings);
    }
#endif
}

#define ba_assert(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, "\033[31mASSERTION FAILED\033[0m %s:%d in %s\n" \
                        "    %s\n", __FILE__, __LINE__, __func__, #expr); \
        ba_print_backtrace(); \
        raise(SIGTRAP); \
        abort(); \
    } \
} while(0)

#define ba_expect(expr) ba_assert(expr)

#endif /* BETTER_ASSERT_H */
