/* better_byteswap.h v1.0 */
#ifndef BETTER_BYTESWAP_H
#define BETTER_BYTESWAP_H
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
    #define bswap16 __builtin_bswap16
    #define bswap32 __builtin_bswap32
    #define bswap64 __builtin_bswap64
#elif defined(_MSC_VER)
    #include <intrin.h>
    #define bswap16 _byteswap_ushort
    #define bswap32 _byteswap_ulong
    #define bswap64 _byteswap_uint64
#else
    static inline uint16_t bswap16(uint16_t x) { return (x<<8)|(x>>8); }
    static inline uint32_t bswap32(uint32_t x) { return (x>>24)|((x>>8)&0xFF00)|((x<<8)&0xFF0000)|(x<<24); }
    static inline uint64_t bswap64(uint64_t x) {
        return ((x>>56)&0xFF)|((x>>40)&0xFF00)|((x>>24)&0xFF0000)|((x>>8)&0xFF000000ULL)|
               ((x<<8)&0xFF00000000ULL)|((x<<24)&0xFF0000000000ULL)|((x<<40)&0xFF000000000000ULL)|(x<<56);
    }
#endif

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define htobe16(x) (x)
    #define be16toh(x) (x)
    #define htole16(x) bswap16(x)
    #define le16toh(x) bswap16(x)
    /* ... same for 32/64 ... */
#else
    #define htole16(x) (x)
    #define le16toh(x) (x)
    #define htobe16(x) bswap16(x)
    #define be16toh(x) bswap16(x)
#endif

#endif
