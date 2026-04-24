#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_sger_baseline
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_sger_optimized
#endif

#ifndef APPLY_VECTORIZATION_SGER_M
#define APPLY_VECTORIZATION_SGER_M 64
#endif

#ifndef APPLY_VECTORIZATION_SGER_N
#define APPLY_VECTORIZATION_SGER_N 96
#endif

#ifndef APPLY_VECTORIZATION_SGER_ITERS
#define APPLY_VECTORIZATION_SGER_ITERS 512
#endif

#ifndef APPLY_VECTORIZATION_SGER_TOLERANCE
#define APPLY_VECTORIZATION_SGER_TOLERANCE 0.0001f
#endif

#ifndef APPLY_VECTORIZATION_SGER_CHECKSUM_TOLERANCE
#define APPLY_VECTORIZATION_SGER_CHECKSUM_TOLERANCE 0.001
#endif

void APPLY_VECTORIZATION_BASELINE_FUNCTION(
    CBLAS_ORDER order,
    int m,
    int n,
    float alpha,
    const float *x,
    int incx,
    const float *y,
    int incy,
    float *a,
    int lda
);
void APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
    CBLAS_ORDER order,
    int m,
    int n,
    float alpha,
    const float *x,
    int incx,
    const float *y,
    int incy,
    float *a,
    int lda
);

static double run_kernel(
    void (*kernel)(
        CBLAS_ORDER,
        int,
        int,
        float,
        const float *,
        int,
        const float *,
        int,
        float *,
        int
    ),
    const float *x,
    const float *y,
    const float *seed_a,
    float *scratch_a
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < APPLY_VECTORIZATION_SGER_ITERS; ++iter) {
        apply_vectorization_copy_f32(
            scratch_a,
            seed_a,
            (size_t)APPLY_VECTORIZATION_SGER_M * (size_t)APPLY_VECTORIZATION_SGER_N
        );
        kernel(
            CblasRowMajor,
            APPLY_VECTORIZATION_SGER_M,
            APPLY_VECTORIZATION_SGER_N,
            1.0f,
            x,
            1,
            y,
            1,
            scratch_a,
            APPLY_VECTORIZATION_SGER_N
        );
    }

    return (apply_vectorization_seconds() - start)
        / (double)APPLY_VECTORIZATION_SGER_ITERS;
}

int main(void) {
    const size_t matrix_count =
        (size_t)APPLY_VECTORIZATION_SGER_M * (size_t)APPLY_VECTORIZATION_SGER_N;
    float *x = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SGER_M);
    float *y = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SGER_N);
    float *seed_a = (float *)malloc(sizeof(float) * matrix_count);
    float *baseline_a = (float *)malloc(sizeof(float) * matrix_count);
    float *optimized_a = (float *)malloc(sizeof(float) * matrix_count);
    double baseline_seconds;
    double optimized_seconds;
    double baseline_sum;
    double optimized_sum;
    double checksum_gap;
    float diff;

    if (x == NULL || y == NULL || seed_a == NULL || baseline_a == NULL || optimized_a == NULL) {
        free(x);
        free(y);
        free(seed_a);
        free(baseline_a);
        free(optimized_a);
        return apply_vectorization_report_alloc_failure("cblas_sger");
    }

    apply_vectorization_fill_vector(x, APPLY_VECTORIZATION_SGER_M, 0.03125f);
    apply_vectorization_fill_vector(y, APPLY_VECTORIZATION_SGER_N, 0.046875f);
    apply_vectorization_fill_matrix(
        seed_a,
        APPLY_VECTORIZATION_SGER_M,
        APPLY_VECTORIZATION_SGER_N,
        APPLY_VECTORIZATION_SGER_N,
        0.015625f
    );

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION,
        x,
        y,
        seed_a,
        baseline_a
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION,
        x,
        y,
        seed_a,
        optimized_a
    );

    apply_vectorization_copy_f32(baseline_a, seed_a, matrix_count);
    apply_vectorization_copy_f32(optimized_a, seed_a, matrix_count);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(
        CblasRowMajor,
        APPLY_VECTORIZATION_SGER_M,
        APPLY_VECTORIZATION_SGER_N,
        1.0f,
        x,
        1,
        y,
        1,
        baseline_a,
        APPLY_VECTORIZATION_SGER_N
    );
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
        CblasRowMajor,
        APPLY_VECTORIZATION_SGER_M,
        APPLY_VECTORIZATION_SGER_N,
        1.0f,
        x,
        1,
        y,
        1,
        optimized_a,
        APPLY_VECTORIZATION_SGER_N
    );

    baseline_sum = apply_vectorization_checksum_f32((const float *)baseline_a, (int)matrix_count);
    optimized_sum = apply_vectorization_checksum_f32((const float *)optimized_a, (int)matrix_count);
    checksum_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    diff = apply_vectorization_max_abs_diff_f32((const float *)baseline_a, (const float *)optimized_a, (int)matrix_count);

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("最大绝对误差=%.8f\n", diff);

    if (diff > APPLY_VECTORIZATION_SGER_TOLERANCE
        || checksum_gap > APPLY_VECTORIZATION_SGER_CHECKSUM_TOLERANCE) {
        apply_vectorization_print_summary(
            baseline_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(x);
        free(y);
        free(seed_a);
        free(baseline_a);
        free(optimized_a);
        return 2;
    }

    apply_vectorization_print_summary(
        baseline_seconds,
        optimized_seconds,
        "通过",
        "checksum ok"
    );

    free(x);
    free(y);
    free(seed_a);
    free(baseline_a);
    free(optimized_a);
    return 0;
}
