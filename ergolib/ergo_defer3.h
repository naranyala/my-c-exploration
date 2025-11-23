#ifndef ERGO_DEFER_H
#define ERGO_DEFER_H

#define _ergo_defer_concat(a,b) a##b
#define _ergo_defer_label(a) _ergo_defer_concat(_ergo_defer_, a)

#define defer(start, end) \
    for (int _ergo_defer_label(__LINE__) = ((start), 0); \
         !_ergo_defer_label(__LINE__); \
         (++_ergo_defer_label(__LINE__), (end)))

#endif

