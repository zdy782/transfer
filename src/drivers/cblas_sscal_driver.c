#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_sscal_baseline
#endif

#ifndef APPLY_VECTORIZATION_AUTOVEC_FUNCTION
#define APPLY_VECTORIZATION_AUTOVEC_FUNCTION cblas_sscal_autovec
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_sscal_optimized
#endif

#ifndef APPLY_VECTORIZATION_SSCAL_N
#define APPLY_VECTORIZATION_SSCAL_N 4096
#endif

#ifndef APPLY_VECTORIZATION_SSCAL_ITERS
#define APPLY_VECTORIZATION_SSCAL_ITERS 2048
#endif

#ifndef APPLY_VECTORIZATION_SSCAL_TOLERANCE
#define APPLY_VECTORIZATION_SSCAL_TOLERANCE 0.0001f
#endif

#ifndef APPLY_VECTORIZATION_SSCAL_CHECKSUM_TOLERANCE
#define APPLY_VECTORIZATION_SSCAL_CHECKSUM_TOLERANCE 0.001
#endif

void APPLY_VECTORIZATION_BASELINE_FUNCTION(int n, float alpha, float *x, int incx);
void APPLY_VECTORIZATION_AUTOVEC_FUNCTION(int n, float alpha, float *x, int incx);
void APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(int n, float alpha, float *x, int incx);

static double run_kernel(
    void (*kernel)(int, float, float *, int),
    int n,
    int iters,
    float alpha,
    const float *seed_x,
    float *scratch_x
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < iters; ++iter) {
        apply_vectorization_copy_f32(scratch_x, seed_x, (size_t)n);
        kernel(n, alpha, scratch_x, 1);
    }

    return (apply_vectorization_seconds() - start) / (double)iters;
}

int main(int argc, char **argv) {
    const int n = apply_vectorization_arg_int(
        argc, argv, "-n", APPLY_VECTORIZATION_SSCAL_N
    );
    const int iters = apply_vectorization_arg_int(
        argc, argv, "-iters", APPLY_VECTORIZATION_SSCAL_ITERS
    );
    const float alpha = -0.75f;
    const double checksum_tolerance =
        APPLY_VECTORIZATION_SSCAL_CHECKSUM_TOLERANCE * (double)n;
    float *seed_x = (float *)malloc(sizeof(float) * (size_t)n);
    float *baseline_x = (float *)malloc(sizeof(float) * (size_t)n);
    float *autovec_x = (float *)malloc(sizeof(float) * (size_t)n);
    float *optimized_x = (float *)malloc(sizeof(float) * (size_t)n);
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

    if (seed_x == NULL || baseline_x == NULL || autovec_x == NULL || optimized_x == NULL) {
        free(seed_x);
        free(baseline_x);
        free(autovec_x);
        free(optimized_x);
        return apply_vectorization_report_alloc_failure("cblas_sscal");
    }

    apply_vectorization_fill_vector(seed_x, n, 0.0546875f);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION, n, iters, alpha, seed_x, baseline_x
    );
    autovec_seconds = run_kernel(
        APPLY_VECTORIZATION_AUTOVEC_FUNCTION, n, iters, alpha, seed_x, autovec_x
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION, n, iters, alpha, seed_x, optimized_x
    );

    apply_vectorization_copy_f32(baseline_x, seed_x, (size_t)n);
    apply_vectorization_copy_f32(autovec_x, seed_x, (size_t)n);
    apply_vectorization_copy_f32(optimized_x, seed_x, (size_t)n);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(n, alpha, baseline_x, 1);
    APPLY_VECTORIZATION_AUTOVEC_FUNCTION(n, alpha, autovec_x, 1);
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(n, alpha, optimized_x, 1);

    baseline_sum = apply_vectorization_checksum_f32(baseline_x, n);
    autovec_sum = apply_vectorization_checksum_f32(autovec_x, n);
    optimized_sum = apply_vectorization_checksum_f32(optimized_x, n);
    autovec_gap = apply_vectorization_absd(baseline_sum - autovec_sum);
    optimized_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    autovec_diff = apply_vectorization_max_abs_diff_f32(baseline_x, autovec_x, n);
    optimized_diff = apply_vectorization_max_abs_diff_f32(baseline_x, optimized_x, n);

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("自动向量化校验和=%.6f\n", autovec_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("自动向量化最大绝对误差=%.8f\n", autovec_diff);
    printf("优化后最大绝对误差=%.8f\n", optimized_diff);

    if (autovec_diff > APPLY_VECTORIZATION_SSCAL_TOLERANCE
        || optimized_diff > APPLY_VECTORIZATION_SSCAL_TOLERANCE
        || autovec_gap > checksum_tolerance
        || optimized_gap > checksum_tolerance) {
        apply_vectorization_print_summary_three(
            baseline_seconds,
            autovec_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(seed_x);
        free(baseline_x);
        free(autovec_x);
        free(optimized_x);
        return 2;
    }

    apply_vectorization_print_summary_three(
        baseline_seconds,
        autovec_seconds,
        optimized_seconds,
        "通过",
        "checksum ok"
    );

    free(seed_x);
    free(baseline_x);
    free(autovec_x);
    free(optimized_x);
    return 0;
}
