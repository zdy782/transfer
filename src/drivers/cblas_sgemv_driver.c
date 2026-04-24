#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_sgemv_baseline
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_sgemv_optimized
#endif

#ifndef APPLY_VECTORIZATION_SGEMV_M
#define APPLY_VECTORIZATION_SGEMV_M 96
#endif

#ifndef APPLY_VECTORIZATION_SGEMV_N
#define APPLY_VECTORIZATION_SGEMV_N 128
#endif

#ifndef APPLY_VECTORIZATION_SGEMV_ITERS
#define APPLY_VECTORIZATION_SGEMV_ITERS 512
#endif

#ifndef APPLY_VECTORIZATION_SGEMV_TOLERANCE
#define APPLY_VECTORIZATION_SGEMV_TOLERANCE 0.0001f
#endif

#ifndef APPLY_VECTORIZATION_SGEMV_CHECKSUM_TOLERANCE
#define APPLY_VECTORIZATION_SGEMV_CHECKSUM_TOLERANCE 0.001
#endif

void APPLY_VECTORIZATION_BASELINE_FUNCTION(
    CBLAS_ORDER order,
    CBLAS_TRANSPOSE trans,
    int m,
    int n,
    float alpha,
    const float *a,
    int lda,
    const float *x,
    int incx,
    float beta,
    float *y,
    int incy
);
void APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
    CBLAS_ORDER order,
    CBLAS_TRANSPOSE trans,
    int m,
    int n,
    float alpha,
    const float *a,
    int lda,
    const float *x,
    int incx,
    float beta,
    float *y,
    int incy
);

static double run_kernel(
    void (*kernel)(
        CBLAS_ORDER,
        CBLAS_TRANSPOSE,
        int,
        int,
        float,
        const float *,
        int,
        const float *,
        int,
        float,
        float *,
        int
    ),
    const float *a,
    const float *x,
    const float *seed_y,
    float *scratch_y
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < APPLY_VECTORIZATION_SGEMV_ITERS; ++iter) {
        apply_vectorization_copy_f32(
            scratch_y,
            seed_y,
            APPLY_VECTORIZATION_SGEMV_M
        );
        kernel(
            CblasRowMajor,
            CblasNoTrans,
            APPLY_VECTORIZATION_SGEMV_M,
            APPLY_VECTORIZATION_SGEMV_N,
            1.0f,
            a,
            APPLY_VECTORIZATION_SGEMV_N,
            x,
            1,
            0.0f,
            scratch_y,
            1
        );
    }

    return (apply_vectorization_seconds() - start)
        / (double)APPLY_VECTORIZATION_SGEMV_ITERS;
}

int main(void) {
    const size_t matrix_count =
        (size_t)APPLY_VECTORIZATION_SGEMV_M * (size_t)APPLY_VECTORIZATION_SGEMV_N;
    float *a = (float *)malloc(sizeof(float) * matrix_count);
    float *x = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SGEMV_N);
    float *seed_y = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SGEMV_M);
    float *baseline_y = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SGEMV_M);
    float *optimized_y = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SGEMV_M);
    double baseline_seconds;
    double optimized_seconds;
    double baseline_sum;
    double optimized_sum;
    double checksum_gap;
    float diff;

    if (a == NULL || x == NULL || seed_y == NULL || baseline_y == NULL || optimized_y == NULL) {
        free(a);
        free(x);
        free(seed_y);
        free(baseline_y);
        free(optimized_y);
        return apply_vectorization_report_alloc_failure("cblas_sgemv");
    }

    apply_vectorization_fill_matrix(
        a,
        APPLY_VECTORIZATION_SGEMV_M,
        APPLY_VECTORIZATION_SGEMV_N,
        APPLY_VECTORIZATION_SGEMV_N,
        0.015625f
    );
    apply_vectorization_fill_vector(x, APPLY_VECTORIZATION_SGEMV_N, 0.03125f);
    apply_vectorization_fill_vector(seed_y, APPLY_VECTORIZATION_SGEMV_M, 0.046875f);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION,
        a,
        x,
        seed_y,
        baseline_y
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION,
        a,
        x,
        seed_y,
        optimized_y
    );

    apply_vectorization_copy_f32(baseline_y, seed_y, APPLY_VECTORIZATION_SGEMV_M);
    apply_vectorization_copy_f32(optimized_y, seed_y, APPLY_VECTORIZATION_SGEMV_M);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(
        CblasRowMajor,
        CblasNoTrans,
        APPLY_VECTORIZATION_SGEMV_M,
        APPLY_VECTORIZATION_SGEMV_N,
        1.0f,
        a,
        APPLY_VECTORIZATION_SGEMV_N,
        x,
        1,
        0.0f,
        baseline_y,
        1
    );
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
        CblasRowMajor,
        CblasNoTrans,
        APPLY_VECTORIZATION_SGEMV_M,
        APPLY_VECTORIZATION_SGEMV_N,
        1.0f,
        a,
        APPLY_VECTORIZATION_SGEMV_N,
        x,
        1,
        0.0f,
        optimized_y,
        1
    );

    baseline_sum = apply_vectorization_checksum_f32(
        baseline_y,
        APPLY_VECTORIZATION_SGEMV_M
    );
    optimized_sum = apply_vectorization_checksum_f32(
        optimized_y,
        APPLY_VECTORIZATION_SGEMV_M
    );
    checksum_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    diff = apply_vectorization_max_abs_diff_f32(
        baseline_y,
        optimized_y,
        APPLY_VECTORIZATION_SGEMV_M
    );

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("最大绝对误差=%.8f\n", diff);

    if (diff > APPLY_VECTORIZATION_SGEMV_TOLERANCE
        || checksum_gap > APPLY_VECTORIZATION_SGEMV_CHECKSUM_TOLERANCE) {
        apply_vectorization_print_summary(
            baseline_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(a);
        free(x);
        free(seed_y);
        free(baseline_y);
        free(optimized_y);
        return 2;
    }

    apply_vectorization_print_summary(
        baseline_seconds,
        optimized_seconds,
        "通过",
        "checksum ok"
    );

    free(a);
    free(x);
    free(seed_y);
    free(baseline_y);
    free(optimized_y);
    return 0;
}
