#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_sger_baseline
#endif

#ifndef APPLY_VECTORIZATION_AUTOVEC_FUNCTION
#define APPLY_VECTORIZATION_AUTOVEC_FUNCTION cblas_sger_autovec
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
void APPLY_VECTORIZATION_AUTOVEC_FUNCTION(
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
    int m,
    int n,
    int iters,
    const float *x,
    const float *y,
    const float *seed_a,
    float *scratch_a
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < iters; ++iter) {
        apply_vectorization_copy_f32(scratch_a, seed_a, (size_t)m * (size_t)n);
        kernel(CblasRowMajor, m, n, 1.0f, x, 1, y, 1, scratch_a, n);
    }

    return (apply_vectorization_seconds() - start) / (double)iters;
}

int main(int argc, char **argv) {
    const int m = apply_vectorization_arg_int(
        argc, argv, "-m", APPLY_VECTORIZATION_SGER_M
    );
    const int n = apply_vectorization_arg_int(
        argc, argv, "-n", APPLY_VECTORIZATION_SGER_N
    );
    const int iters = apply_vectorization_arg_int(
        argc, argv, "-iters", APPLY_VECTORIZATION_SGER_ITERS
    );
    const size_t matrix_count = (size_t)m * (size_t)n;
    const double checksum_tolerance =
        APPLY_VECTORIZATION_SGER_CHECKSUM_TOLERANCE * (double)matrix_count;
    float *x = (float *)malloc(sizeof(float) * (size_t)m);
    float *y = (float *)malloc(sizeof(float) * (size_t)n);
    float *seed_a = (float *)malloc(sizeof(float) * matrix_count);
    float *baseline_a = (float *)malloc(sizeof(float) * matrix_count);
    float *autovec_a = (float *)malloc(sizeof(float) * matrix_count);
    float *optimized_a = (float *)malloc(sizeof(float) * matrix_count);
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

    if (x == NULL || y == NULL || seed_a == NULL || baseline_a == NULL
        || autovec_a == NULL || optimized_a == NULL) {
        free(x);
        free(y);
        free(seed_a);
        free(baseline_a);
        free(autovec_a);
        free(optimized_a);
        return apply_vectorization_report_alloc_failure("cblas_sger");
    }

    apply_vectorization_fill_vector(x, m, 0.03125f);
    apply_vectorization_fill_vector(y, n, 0.046875f);
    apply_vectorization_fill_matrix(seed_a, m, n, n, 0.015625f);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION, m, n, iters, x, y, seed_a, baseline_a
    );
    autovec_seconds = run_kernel(
        APPLY_VECTORIZATION_AUTOVEC_FUNCTION, m, n, iters, x, y, seed_a, autovec_a
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION, m, n, iters, x, y, seed_a, optimized_a
    );

    apply_vectorization_copy_f32(baseline_a, seed_a, matrix_count);
    apply_vectorization_copy_f32(autovec_a, seed_a, matrix_count);
    apply_vectorization_copy_f32(optimized_a, seed_a, matrix_count);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(
        CblasRowMajor, m, n, 1.0f, x, 1, y, 1, baseline_a, n
    );
    APPLY_VECTORIZATION_AUTOVEC_FUNCTION(
        CblasRowMajor, m, n, 1.0f, x, 1, y, 1, autovec_a, n
    );
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
        CblasRowMajor, m, n, 1.0f, x, 1, y, 1, optimized_a, n
    );

    baseline_sum = apply_vectorization_checksum_f32(baseline_a, (int)matrix_count);
    autovec_sum = apply_vectorization_checksum_f32(autovec_a, (int)matrix_count);
    optimized_sum = apply_vectorization_checksum_f32(optimized_a, (int)matrix_count);
    autovec_gap = apply_vectorization_absd(baseline_sum - autovec_sum);
    optimized_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    autovec_diff = apply_vectorization_max_abs_diff_f32(
        baseline_a, autovec_a, (int)matrix_count
    );
    optimized_diff = apply_vectorization_max_abs_diff_f32(
        baseline_a, optimized_a, (int)matrix_count
    );

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("自动向量化校验和=%.6f\n", autovec_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("自动向量化最大绝对误差=%.8f\n", autovec_diff);
    printf("优化后最大绝对误差=%.8f\n", optimized_diff);

    if (autovec_diff > APPLY_VECTORIZATION_SGER_TOLERANCE
        || optimized_diff > APPLY_VECTORIZATION_SGER_TOLERANCE
        || autovec_gap > checksum_tolerance
        || optimized_gap > checksum_tolerance) {
        apply_vectorization_print_summary_three(
            baseline_seconds,
            autovec_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(x);
        free(y);
        free(seed_a);
        free(baseline_a);
        free(autovec_a);
        free(optimized_a);
        return 2;
    }

    apply_vectorization_print_summary_three(
        baseline_seconds,
        autovec_seconds,
        optimized_seconds,
        "通过",
        "checksum ok"
    );

    free(x);
    free(y);
    free(seed_a);
    free(baseline_a);
    free(autovec_a);
    free(optimized_a);
    return 0;
}
