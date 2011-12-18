#include <cassert>
#include <cstdint>
#include <ctime>
#include <iostream>

using namespace std;

#if (_MSC_VER >= 1500)
#   include <intrin.h>
#   define POPCNT(x) __popcnt(x)
#   define POPCNT64(x) __popcnt64(x)
#endif

#if defined(__GNUC__)
#   define POPCNT(x) __builtin_popcount(x)
#   define POPCNT64(x) __builtin_popcountll(x)
#endif

inline int popcnt64(register uint64_t u)
{
    u = (u & 0x5555555555555555) + ((u >> 1) & 0x5555555555555555);
    u = (u & 0x3333333333333333) + ((u >> 2) & 0x3333333333333333);
    u = (u & 0x0f0f0f0f0f0f0f0f) + ((u >> 4) & 0x0f0f0f0f0f0f0f0f);
    u = (u & 0x00ff00ff00ff00ff) + ((u >> 8) & 0x00ff00ff00ff00ff);
    u = (u & 0x0000ffff0000ffff) + ((u >>16) & 0x0000ffff0000ffff);
    u = (u & 0x00000000ffffffff) + ((u >>32) & 0x00000000ffffffff);
    return u;
}

inline int popcnt(register uint32_t u)
{
    u = (u & 0x55555555) + ((u >> 1) & 0x55555555);
    u = (u & 0x33333333) + ((u >> 2) & 0x33333333);
    u = (u & 0x0f0f0f0f) + ((u >> 4) & 0x0f0f0f0f);
    u = (u & 0x00ff00ff) + ((u >> 8) & 0x00ff00ff);
    u = (u & 0x0000ffff) + ((u >>16) & 0x0000ffff);
    return u;
}

inline int popcnt64_nibble(register uint64_t u)
{
    static const uint8_t Table[] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
    };

    int c = 0;
    while (u)
    {
        c += Table[u & 0xf];
        u >>= 4;
    }
    return c;
}

inline int popcnt_nibble(register uint32_t u)
{
    static const uint8_t Table[] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
    };

    int c = 0;
    while (u)
    {
        c += Table[u & 0xf];
        u >>= 4;
    }
    return c;
}

inline int popcnt64_byte(register uint64_t u)
{
#define B2(k) k, k+1, k+1, k+2
#define B4(k) B2(k), B2(k+1), B2(k+1), B2(k+2)
#define B6(k) B4(k), B4(k+1), B4(k+1), B4(k+2)
    static const uint8_t Table[] = {
        B6(0), B6(1), B6(1), B6(2)
    };
#undef B6
#undef B4
#undef B2

    int c = 0;
    while (u)
    {
        c += Table[u & 0xff];
        u >>= 8;
    }
    return c;
}

inline int popcnt_byte(register uint32_t u)
{
#define B2(k) k, k+1, k+1, k+2
#define B4(k) B2(k), B2(k+1), B2(k+1), B2(k+2)
#define B6(k) B4(k), B4(k+1), B4(k+1), B4(k+2)
    static const uint8_t Table[] = {
        B6(0), B6(1), B6(1), B6(2)
    };
#undef B6
#undef B4
#undef B2

    int c = 0;
    while (u)
    {
        c += Table[u & 0xff];
        u >>= 8;
    }
    return c;
}

// compiler options:
// gcc-4.6.1 "g++ -std=c++0x -march=native -O2"
// msvc10 "cl /O2"

// data are median of three repeats rounded to three significant figures

// gcc-4.6.1 // __builtin_popcount 2.13 secs // __builtin_popcountll 50.1 secs
// gcc-4.6.1 (x64) // __builtin_popcount 2.08 secs // __builtin_popcountll 2.09 secs
// msvc10 (x86) // __popcnt 3.25 secs // __popcnt64 error
// msvc10 (x64) // __popcnt 2.08 secs // __popcnt64 2.04 secs

// gcc-4.6.1 // gcc-4.6.1 (x64) // msvc10 (x86) // msvc10 (x64)
// 50.1 secs // 13.1 secs // 36.1 secs // 14.9 secs // popcnt64
// 12.3 secs // 12.8 secs // 13.4 secs // 12.6 secs // popcnt
// 38.6 secs // 21.3 secs // 19.2 secs // 43.8 secs // popcnt64_nibble
// 18.1 secs // 21.3 secs // 24.1 secs // 19.2 secs // popcnt_nibble
// 19.1 secs // 12.5 secs // 21.2 secs // 11.28 secs // popcnt64_byte
// 8.42 secs // 12.7 secs // 12.3 secs // 12.2 secs // popcnt_byte

inline void test()
{
    volatile int r;
    for (uint64_t i = 0; i < (1uLL << 31) - 1; ++i)
    {
        r = popcnt(i); // change
    }
}

int main()
{
    clock_t start = clock();
    test();
    cout << ((clock()-start)/(double)CLOCKS_PER_SEC) << endl;
}

