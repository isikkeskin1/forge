#include "forge_kernel.h"

#include <stddef.h>
#include <stdint.h>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

size_t forge_i64_count_ge_simd(const int64_t *data, size_t length,
                               int64_t threshold) {
    if (data == NULL && length != 0) {
        return 0;
    }

#if defined(__AVX2__)
    size_t count = 0;
    size_t i = 0;
    const __m256i limit = _mm256_set1_epi64x(threshold);
    const __m256i one = _mm256_set1_epi64x(1);
    __m256i lanes = _mm256_setzero_si256();

    for (; i + 4 <= length; i += 4) {
        const __m256i values = _mm256_loadu_si256((const __m256i *)(data + i));
        const __m256i greater = _mm256_cmpgt_epi64(values, limit);
        const __m256i equal = _mm256_cmpeq_epi64(values, limit);
        const __m256i matches = _mm256_or_si256(greater, equal);
        lanes = _mm256_add_epi64(lanes, _mm256_and_si256(matches, one));
    }

    int64_t partial[4];
    _mm256_storeu_si256((__m256i *)partial, lanes);
    count = (size_t)partial[0] + (size_t)partial[1] +
            (size_t)partial[2] + (size_t)partial[3];

    for (; i < length; ++i) {
        if (data[i] >= threshold) {
            ++count;
        }
    }
    return count;
#else
    return forge_i64_count_ge(data, length, threshold);
#endif
}

size_t forge_i64_filter_ge_simd(const int64_t *data, size_t length,
                                int64_t threshold, int64_t *output) {
    if ((data == NULL || output == NULL) && length != 0) {
        return 0;
    }

#if defined(__AVX2__)
    size_t written = 0;
    size_t i = 0;
    const __m256i limit = _mm256_set1_epi64x(threshold);

    for (; i + 4 <= length; i += 4) {
        const __m256i values = _mm256_loadu_si256((const __m256i *)(data + i));
        const __m256i greater = _mm256_cmpgt_epi64(values, limit);
        const __m256i equal = _mm256_cmpeq_epi64(values, limit);
        const __m256i matches = _mm256_or_si256(greater, equal);
        const unsigned mask = (unsigned)_mm256_movemask_pd(_mm256_castsi256_pd(matches));

        if (mask == 0u) {
            continue;
        }
        if (mask == 0x0fu) {
            _mm256_storeu_si256((__m256i *)(output + written), values);
            written += 4;
            continue;
        }

        /* AVX2 has no 64-bit compress-store; compact the matching lanes in order. */
        int64_t lanes[4];
        _mm256_storeu_si256((__m256i *)lanes, values);
        for (unsigned lane = 0; lane < 4; ++lane) {
            if ((mask & (1u << lane)) != 0u) {
                output[written++] = lanes[lane];
            }
        }
    }

    for (; i < length; ++i) {
        if (data[i] >= threshold) {
            output[written++] = data[i];
        }
    }
    return written;
#else
    return forge_i64_filter_ge(data, length, threshold, output);
#endif
}
