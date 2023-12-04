#ifndef SIMD_H
#define SIMD_H

#ifdef __x86_64__
 #include <immintrin.h>
#else   
 #include "sse2neon.h"
#endif

#define NUM_ELEMS ((1 << 16) + 10)
#define OUTER_ITERATIONS (1 << 14)

long long int sum(int vals[NUM_ELEMS]);

long long int sum_unrolled(int vals[NUM_ELEMS]);

long long int sum_simd(int vals[NUM_ELEMS]);

long long int sum_simd_unrolled(int vals[NUM_ELEMS]);

#endif
