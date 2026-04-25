#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_sgemv_baseline
#endif

#ifndef APPLY_VECTORIZATION_AUTOVEC_FUNCTION
#define APPLY_VECTORIZATION_AUTOVEC_FUNCTION cblas_sgemv_autovec
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
void APPLY_VECTORIZATION_AUTOVEC_FUNCTION(
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
    int m,
    int n,
    int iters,
    const float *a,
    const float *x,
    const float *seed_y,
    float *scratch_y
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < iters; ++iter) {
        apply_vectorization_copy_f32(scratch_y, seed_y, (size_t)m);
        kernel(CblasRowMajor, CblasNoTrans, m, n, 1.0f, a, n, x, 1, 0.0f, scratch_y, 1);
    }

    return (apply_vectorization_seconds() - start) / (double)iters;
}

int main(int argc, char **argv) {
    const int m = apply_vectorization_arg_int(
        argc, argv, "-m", APPLY_VECTORIZATION_SGEMV_M
    );
    const int n = apply_vectorization_arg_int(
        argc, argv, "-n", APPLY_VECTORIZATION_SGEMV_N
    );
    const int iters = apply_vectorization_arg_int(
        argc, argv, "-iters", APPLY_VECTORIZATION_SGEMV_ITERS
    );
    const size_t matrix_count = (size_t)m * (size_t)n;
    const float value_tolerance =
        APPLY_VECTORIZATION_SGEMV_TOLERANCE * (1.0f + (float)n);
    const double checksum_tolerance =
        APPLY_VECTORIZATION_SGEMV_CHECKSUM_TOLERANCE * (double)m;
    float *a = (float *)malloc(sizeof(float) * matrix_count);
    float *x = (float *)malloc(sizeof(float) * (size_t)n);
    float *seed_y = (float *)malloc(sizeof(float) * (size_t)m);
    float *baseline_y = (float *)malloc(sizeof(float) * (size_t)m);
    float *autovec_y = (float *)malloc(sizeof(float) * (size_t)m);
    float *optimized_y = (float *)malloc(sizeof(float) * (size_t)m);
    double baseline_seconds;
    double autovec_seconds;
    double optimized_seconds;
    double baseline_sum;
    double autovec_sum;
    double optimized_sum;
    double autovec_gap;
    double optimized_gap;
    float autovec_diff;
    float optimized_diff;

    if (a == NULL || x == NULL || seed_y == NULL || baseline_y == NULL
        || autovec_y == NULL || optimized_y == NULL) {
        free(a);
        free(x);
        free(seed_y);
        free(baseline_y);
        free(autovec_y);
        free(optimized_y);
        return apply_vectorization_report_alloc_failure("cblas_sgemv");
    }

    apply_vectorization_fill_matrix(a, m, n, n, 0.015625f);
    apply_vectorization_fill_vector(x, n, 0.03125f);
    apply_vectorization_fill_vector(seed_y, m, 0.046875f);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION, m, n, iters, a, x, seed_y, baseline_y
    );
    autovec_seconds = run_kernel(
        APPLY_VECTORIZATION_AUTOVEC_FUNCTION, m, n, iters, a, x, seed_y, autovec_y
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION, m, n, iters, a, x, seed_y, optimized_y
    );

    apply_vectorization_copy_f32(baseline_y, seed_y, (size_t)m);
    apply_vectorization_copy_f32(autovec_y, seed_y, (size_t)m);
    apply_vectorization_copy_f32(optimized_y, seed_y, (size_t)m);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(
        CblasRowMajor, CblasNoTrans, m, n, 1.0f, a, n, x, 1, 0.0f, baseline_y, 1
    );
    APPLY_VECTORIZATION_AUTOVEC_FUNCTION(
        CblasRowMajor, CblasNoTrans, m, n, 1.0f, a, n, x, 1, 0.0f, autovec_y, 1
    );
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
        CblasRowMajor, CblasNoTrans, m, n, 1.0f, a, n, x, 1, 0.0f, optimized_y, 1
    );

    baseline_sum = apply_vectorization_checksum_f32(baseline_y, m);
    autovec_sum = apply_vectorization_checksum_f32(autovec_y, m);
    optimized_sum = apply_vectorization_checksum_f32(optimized_y, m);
    autovec_gap = apply_vectorization_absd(baseline_sum - autovec_sum);
    optimized_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    autovec_diff = apply_vectorization_max_abs_diff_f32(baseline_y, autovec_y, m);
    optimized_diff = apply_vectorization_max_abs_diff_f32(baseline_y, optimized_y, m);

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("自动向量化校验和=%.6f\n", autovec_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("自动向量化最大绝对误差=%.8f\n", autovec_diff);
    printf("优化后最大绝对误差=%.8f\n", optimized_diff);

    if (autovec_diff > value_tolerance
        || optimized_diff > value_tolerance
        || autovec_gap > checksum_tolerance
        || optimized_gap > checksum_tolerance) {
        apply_vectorization_print_summary_three(
            baseline_seconds,
            autovec_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(a);
        free(x);
        free(seed_y);
        free(baseline_y);
        free(autovec_y);
        free(optimized_y);
        return 2;
    }

    apply_vectorization_print_summary_three(
        baseline_seconds,
        autovec_seconds,
        optimized_seconds,
        "通过",
        "checksum ok"
    );

    free(a);
    free(x);
    free(seed_y);
    free(baseline_y);
    free(autovec_y);
    free(optimized_y);
    return 0;
}
