#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_scopy_baseline
#endif

#ifndef APPLY_VECTORIZATION_AUTOVEC_FUNCTION
#define APPLY_VECTORIZATION_AUTOVEC_FUNCTION cblas_scopy_autovec
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_scopy_optimized
#endif

#ifndef APPLY_VECTORIZATION_SCOPY_N
#define APPLY_VECTORIZATION_SCOPY_N 4096
#endif

#ifndef APPLY_VECTORIZATION_SCOPY_ITERS
#define APPLY_VECTORIZATION_SCOPY_ITERS 2048
#endif

#ifndef APPLY_VECTORIZATION_SCOPY_TOLERANCE
#define APPLY_VECTORIZATION_SCOPY_TOLERANCE 0.0001f
#endif

#ifndef APPLY_VECTORIZATION_SCOPY_CHECKSUM_TOLERANCE
#define APPLY_VECTORIZATION_SCOPY_CHECKSUM_TOLERANCE 0.001
#endif

void APPLY_VECTORIZATION_BASELINE_FUNCTION(
    int n, const float *x, int incx, float *y, int incy
);
void APPLY_VECTORIZATION_AUTOVEC_FUNCTION(
    int n, const float *x, int incx, float *y, int incy
);
void APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
    int n, const float *x, int incx, float *y, int incy
);

static double run_kernel(
    void (*kernel)(int, const float *, int, float *, int),
    int n,
    int iters,
    const float *x,
    float *scratch_y
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < iters; ++iter) {
        kernel(n, x, 1, scratch_y, 1);
    }

    return (apply_vectorization_seconds() - start) / (double)iters;
}

int main(int argc, char **argv) {
    const int n = apply_vectorization_arg_int(
        argc, argv, "-n", APPLY_VECTORIZATION_SCOPY_N
    );
    const int iters = apply_vectorization_arg_int(
        argc, argv, "-iters", APPLY_VECTORIZATION_SCOPY_ITERS
    );
    const double checksum_tolerance =
        APPLY_VECTORIZATION_SCOPY_CHECKSUM_TOLERANCE * (double)n;
    float *x = (float *)malloc(sizeof(float) * (size_t)n);
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

    if (x == NULL || baseline_y == NULL || autovec_y == NULL || optimized_y == NULL) {
        free(x);
        free(baseline_y);
        free(autovec_y);
        free(optimized_y);
        return apply_vectorization_report_alloc_failure("cblas_scopy");
    }

    apply_vectorization_fill_vector(x, n, 0.0390625f);
    memset(baseline_y, 0, sizeof(float) * (size_t)n);
    memset(autovec_y, 0, sizeof(float) * (size_t)n);
    memset(optimized_y, 0, sizeof(float) * (size_t)n);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION, n, iters, x, baseline_y
    );
    autovec_seconds = run_kernel(
        APPLY_VECTORIZATION_AUTOVEC_FUNCTION, n, iters, x, autovec_y
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION, n, iters, x, optimized_y
    );

    memset(baseline_y, 0, sizeof(float) * (size_t)n);
    memset(autovec_y, 0, sizeof(float) * (size_t)n);
    memset(optimized_y, 0, sizeof(float) * (size_t)n);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(n, x, 1, baseline_y, 1);
    APPLY_VECTORIZATION_AUTOVEC_FUNCTION(n, x, 1, autovec_y, 1);
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(n, x, 1, optimized_y, 1);

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

    if (autovec_diff > APPLY_VECTORIZATION_SCOPY_TOLERANCE
        || optimized_diff > APPLY_VECTORIZATION_SCOPY_TOLERANCE
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
    free(baseline_y);
    free(autovec_y);
    free(optimized_y);
    return 0;
}
