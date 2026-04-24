#ifndef APPLY_VECTORIZATION_BLAS_CASE_COMMON_H
#define APPLY_VECTORIZATION_BLAS_CASE_COMMON_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
    CblasRowMajor = 101,
    CblasColMajor = 102,
} CBLAS_ORDER;

typedef enum {
    CblasNoTrans = 111,
    CblasTrans = 112,
} CBLAS_TRANSPOSE;

typedef enum {
    CblasUpper = 121,
    CblasLower = 122,
} CBLAS_UPLO;

static inline double apply_vectorization_absd(double value) {
    return value < 0.0 ? -value : value;
}

static inline float apply_vectorization_absf(float value) {
    return value < 0.0f ? -value : value;
}

static inline double apply_vectorization_seconds(void) {
    struct timespec ts;

    timespec_get(&ts, TIME_UTC);
    return (double)ts.tv_sec + ((double)ts.tv_nsec / 1000000000.0);
}

static inline void apply_vectorization_fill_vector(float *data, int count, float scale) {
    for (int index = 0; index < count; ++index) {
        data[index] = (float)((index % 29) - 14) * scale;
    }
}

static inline void apply_vectorization_fill_matrix(
    float *data, int rows, int cols, int ld, float scale
) {
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int base = row * ld + col;
            data[base] = (float)(((row * 13) + (col * 7)) % 41 - 20) * scale;
        }
    }
}

static inline void apply_vectorization_fill_symmetric(
    float *data, int n, int ld, float scale
) {
    for (int row = 0; row < n; ++row) {
        for (int col = row; col < n; ++col) {
            float value = (float)(((row * 11) + (col * 5)) % 37 - 18) * scale;
            data[row * ld + col] = value;
            data[col * ld + row] = value;
        }
    }
}

static inline void apply_vectorization_copy_f32(
    float *dst, const float *src, size_t count
) {
    memcpy(dst, src, sizeof(float) * count);
}

static inline double apply_vectorization_checksum_f32(
    const float *data, int count
) {
    double sum = 0.0;

    for (int index = 0; index < count; ++index) {
        sum += (double)data[index];
    }
    return sum;
}

static inline double apply_vectorization_checksum_upper_f32(
    const float *data, int n, int ld
) {
    double sum = 0.0;

    for (int row = 0; row < n; ++row) {
        for (int col = row; col < n; ++col) {
            sum += (double)data[row * ld + col];
        }
    }
    return sum;
}

static inline float apply_vectorization_max_abs_diff_f32(
    const float *lhs, const float *rhs, int count
) {
    float max_diff = 0.0f;

    for (int index = 0; index < count; ++index) {
        float diff = apply_vectorization_absf(lhs[index] - rhs[index]);
        if (diff > max_diff) {
            max_diff = diff;
        }
    }
    return max_diff;
}

static inline float apply_vectorization_max_abs_diff_upper_f32(
    const float *lhs, const float *rhs, int n, int ld
) {
    float max_diff = 0.0f;

    for (int row = 0; row < n; ++row) {
        for (int col = row; col < n; ++col) {
            float diff = apply_vectorization_absf(
                lhs[row * ld + col] - rhs[row * ld + col]
            );
            if (diff > max_diff) {
                max_diff = diff;
            }
        }
    }
    return max_diff;
}

static inline void apply_vectorization_print_summary(
    double before_seconds,
    double after_seconds,
    const char *correctness,
    const char *remark
) {
    double speedup = 0.0;
    double improvement = 0.0;

    if (after_seconds > 0.0) {
        speedup = before_seconds / after_seconds;
    }
    if (before_seconds > 0.0) {
        improvement = ((before_seconds - after_seconds) / before_seconds) * 100.0;
    }

    printf("使用前(秒)=%.9e\n", before_seconds);
    printf("使用后(秒)=%.9e\n", after_seconds);
    printf("加速比=%.3fx\n", speedup);
    printf("性能提升=%.2f%%\n", improvement);
    printf("正确性=%s\n", correctness);
    printf("备注=%s\n", remark);
}

static inline int apply_vectorization_report_alloc_failure(const char *case_name) {
    fprintf(stderr, "[错误] %s 分配内存失败\n", case_name);
    return 1;
}

#endif
