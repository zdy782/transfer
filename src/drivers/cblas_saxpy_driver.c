#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_saxpy_baseline
#endif

#ifndef APPLY_VECTORIZATION_AUTOVEC_FUNCTION
#define APPLY_VECTORIZATION_AUTOVEC_FUNCTION cblas_saxpy_autovec
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_saxpy_optimized
#endif

#ifndef APPLY_VECTORIZATION_SAXPY_N
#define APPLY_VECTORIZATION_SAXPY_N 4096
#endif

#ifndef APPLY_VECTORIZATION_SAXPY_ITERS
#define APPLY_VECTORIZATION_SAXPY_ITERS 2048
#endif

#ifndef APPLY_VECTORIZATION_SAXPY_TOLERANCE
#define APPLY_VECTORIZATION_SAXPY_TOLERANCE 0.0001f
#endif

#ifndef APPLY_VECTORIZATION_SAXPY_CHECKSUM_TOLERANCE
#define APPLY_VECTORIZATION_SAXPY_CHECKSUM_TOLERANCE 0.001
#endif

void APPLY_VECTORIZATION_BASELINE_FUNCTION(
    int n, float alpha, const float *x, int incx, float *y, int incy
);
void APPLY_VECTORIZATION_AUTOVEC_FUNCTION(
    int n, float alpha, const float *x, int incx, float *y, int incy
);
void APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
    int n, float alpha, const float *x, int incx, float *y, int incy
);

static double run_kernel(
    void (*kernel)(int, float, const float *, int, float *, int),
    int n,
    int iters,
    float alpha,
    const float *x,
    const float *seed_y,
    float *scratch_y
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < iters; ++iter) {
        apply_vectorization_copy_f32(scratch_y, seed_y, (size_t)n);
        kernel(n, alpha, x, 1, scratch_y, 1);
    }

    return (apply_vectorization_seconds() - start) / (double)iters;
}

int main(int argc, char **argv) {
    const int n = apply_vectorization_arg_int(
        argc, argv, "-n", APPLY_VECTORIZATION_SAXPY_N
    );
    const int iters = apply_vectorization_arg_int(
        argc, argv, "-iters", APPLY_VECTORIZATION_SAXPY_ITERS
    );
    const float alpha = 1.25f;
    const double checksum_tolerance =
        APPLY_VECTORIZATION_SAXPY_CHECKSUM_TOLERANCE * (double)n;
    float *x = (float *)malloc(sizeof(float) * (size_t)n);
    float *seed_y = (float *)malloc(sizeof(float) * (size_t)n);
    float *baseline_y = (float *)malloc(sizeof(float) * (size_t)n);
    float *autovec_y = (float *)malloc(sizeof(float) * (size_t)n);
    float *optimized_y = (float *)malloc(sizeof(float) * (size_t)n);
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

    if (x == NULL || seed_y == NULL || baseline_y == NULL
        || autovec_y == NULL || optimized_y == NULL) {
        free(x);
        free(seed_y);
        free(baseline_y);
        free(autovec_y);
        free(optimized_y);
        return apply_vectorization_report_alloc_failure("cblas_saxpy");
    }

    apply_vectorization_fill_vector(x, n, 0.03125f);
    apply_vectorization_fill_vector(seed_y, n, 0.046875f);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION, n, iters, alpha, x, seed_y, baseline_y
    );
    autovec_seconds = run_kernel(
        APPLY_VECTORIZATION_AUTOVEC_FUNCTION, n, iters, alpha, x, seed_y, autovec_y
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION, n, iters, alpha, x, seed_y, optimized_y
    );

    apply_vectorization_copy_f32(baseline_y, seed_y, (size_t)n);
    apply_vectorization_copy_f32(autovec_y, seed_y, (size_t)n);
    apply_vectorization_copy_f32(optimized_y, seed_y, (size_t)n);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(n, alpha, x, 1, baseline_y, 1);
    APPLY_VECTORIZATION_AUTOVEC_FUNCTION(n, alpha, x, 1, autovec_y, 1);
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(n, alpha, x, 1, optimized_y, 1);

    baseline_sum = apply_vectorization_checksum_f32(baseline_y, n);
    autovec_sum = apply_vectorization_checksum_f32(autovec_y, n);
    optimized_sum = apply_vectorization_checksum_f32(optimized_y, n);
    autovec_gap = apply_vectorization_absd(baseline_sum - autovec_sum);
    optimized_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    autovec_diff = apply_vectorization_max_abs_diff_f32(baseline_y, autovec_y, n);
    optimized_diff = apply_vectorization_max_abs_diff_f32(baseline_y, optimized_y, n);

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("自动向量化校验和=%.6f\n", autovec_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("自动向量化最大绝对误差=%.8f\n", autovec_diff);
    printf("优化后最大绝对误差=%.8f\n", optimized_diff);

    if (autovec_diff > APPLY_VECTORIZATION_SAXPY_TOLERANCE
        || optimized_diff > APPLY_VECTORIZATION_SAXPY_TOLERANCE
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

    free(x);
    free(seed_y);
    free(baseline_y);
    free(autovec_y);
    free(optimized_y);
    return 0;
}
